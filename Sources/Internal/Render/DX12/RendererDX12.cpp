//
// Copyright (C) Alexandr Vorontsov. 2017
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#include "stdafx.h"

#include "Render/DX12/RendererDX12.h"

#include "Sources/External/Dx12Helpers/DDSTextureLoader.h"

#include <array>
#include <string>
#include <vector>

#include "AssetsSystem/AssetsSystem.h"
#include "Core/FPSCounter.h"
#include "Core/Timer/GlobalTimer.h"
#include "Core/WindowsApplication.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"
#include "Render/Geometry/GeometryGenerator.h"
#include "Render/Geometry/Mesh.h"
#include "Render/VertexLayout.h"
#include "Render/DX12/VertexLayoutDX12.h"
#include "Render/DX12/Shader/ShaderParser.h"

#include "Component/CameraComponent.h"
#include "Systems/CameraSystem.h"
#include "Core/Scene.h"
#include "Core/KiotoEngine.h" // [a_vorontsov] For now. TODO: render pass with render target and so on. This class shouldn't know 'bout camera and so on.

#include "Render/DX12/Buffers/UploadBufferDX12.h"
#include "Render/Texture/Texture.h"

namespace Kioto::Renderer
{

using Microsoft::WRL::ComPtr;
using std::wstring;

static uint32 CurrentHandle;

namespace
{
D3D12_RECT DXRectFromKioto(const RectI& source)
{
    return { source.Left, source.Top, source.Right, source.Bottom };
}

D3D12_VIEWPORT DXViewportFromKioto(const RectI& source)
{
    D3D12_VIEWPORT viewport = {};
    viewport.TopLeftX = static_cast<float>(source.Left);
    viewport.TopLeftY = static_cast<float>(source.Top);
    viewport.Width = static_cast<float>(source.Right);
    viewport.Height = static_cast<float>(source.Bottom);
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    return viewport;
}
}

RendererDX12::RendererDX12()
{
    m_inputLayouts.reserve(256);
}

void RendererDX12::Init(uint16 width, uint16 height)
{
    engineBuffers.Init();
    for (auto& res : m_backBuffers)
        res.Handle = CurrentHandle++;
    m_depthStencil.Handle = CurrentHandle++;

    UINT dxgiFactoryFlags = 0;
#ifdef _DEBUG
    {
        ComPtr<ID3D12Debug> debugController;
        if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
        {
            debugController->EnableDebugLayer();
            dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
        }
    }
#endif
    ThrowIfFailed(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&m_factory)));

    BOOL allowTearing = FALSE;
    ComPtr<IDXGIFactory5> factory5;
    ThrowIfFailed(m_factory.As(&factory5));
    HRESULT hr = factory5->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING, &allowTearing, sizeof(allowTearing));
    m_isTearingSupported = SUCCEEDED(hr) && allowTearing;

    ComPtr<IDXGIAdapter1> hardwareAdapter;
    GetHardwareAdapter(m_factory.Get(), &hardwareAdapter);

    ThrowIfFailed(D3D12CreateDevice(hardwareAdapter.Get(), D3D_FEATURE_LEVEL_12_1, IID_PPV_ARGS(&m_device)));

    D3D12_COMMAND_QUEUE_DESC queueDesc = {};
    queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

    ThrowIfFailed(m_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_commandQueue)));
    NAME_D3D12_OBJECT(m_commandQueue);

    DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
    swapChainDesc.BufferCount = FrameCount;
    swapChainDesc.Width = width;
    swapChainDesc.Height = height;
    swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swapChainDesc.SampleDesc.Count = 1;

    swapChainDesc.Flags = m_isTearingSupported ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0;

    ComPtr<IDXGISwapChain1> swapChain;
    ThrowIfFailed(m_factory->CreateSwapChainForHwnd(m_commandQueue.Get(), WindowsApplication::GetHWND(), &swapChainDesc, nullptr, nullptr, &swapChain)); // [a_vorontsov] Fullscreen desc?

    if (m_isTearingSupported)
        m_factory->MakeWindowAssociation(WindowsApplication::GetHWND(), DXGI_MWA_NO_ALT_ENTER);
    ThrowIfFailed(swapChain.As(&m_swapChain));

    m_currentFrameIndex = m_swapChain->GetCurrentBackBufferIndex();

    for (uint32 i = 0; i < FrameCount; ++i)
    {
        ThrowIfFailed(m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_commandAllocators[i])));
        NAME_D3D12_OBJECT(m_commandAllocators[i]);
    }
    ThrowIfFailed(m_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_commandAllocators[0].Get(), nullptr, IID_PPV_ARGS(&m_commandList))); // [a_vorontsov] Maybe not 0 as mask?
    NAME_D3D12_OBJECT(m_commandList);
    m_commandList->Close();

    ThrowIfFailed(m_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence)));
    NAME_D3D12_OBJECT(m_fence);

    D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
    rtvHeapDesc.NumDescriptors = FrameCount;
    rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;

    ThrowIfFailed(m_device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_rtvHeap)));

    D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
    dsvHeapDesc.NumDescriptors = 1;
    dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;

    ThrowIfFailed(m_device->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&m_dsvHeap)));

    m_cbvSrvUavDescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    m_rtvDescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    m_dsvDescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
    m_samplerDescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);

    LoadPipeline();
    Resize(width, height);

#ifdef _DEBUG
    LogAdapters();

    DXGI_ADAPTER_DESC adapterDesc;
    hardwareAdapter->GetDesc(&adapterDesc);

    std::wstring text = L"*****\n!!SELECTED ADAPTER: ";
    text += adapterDesc.Description;
    text += L"\n";
    text += L"****\n";
    OutputDebugString(text.c_str());
#endif
}

void RendererDX12::LoadPipeline()
{
#ifdef _DEBUG
    UINT shaderFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
    UINT shaderFlags = 0;
#endif
    wstring shaderPath = AssetsSystem::GetAssetFullPath(L"Shaders\\Fallback.hlsl");
    ShaderDX12* vs = new ShaderDX12();
    ShaderDX12* ps = new ShaderDX12();
    vs->SetHandle(CurrentHandle++);
    ps->SetHandle(CurrentHandle++);
    ShaderHandle vsHandle = vs->GetHandle();
    ShaderHandle psHandle = ps->GetHandle();

    ShaderParser::ParseResult parseResult;
    parseResult = ShaderParser::ParseShader(WstrToStr(shaderPath), nullptr);
    parseResult.textureSet.SetHandle(CurrentHandle++);

    std::string shaderStr = parseResult.output;
    OutputDebugStringA(shaderStr.c_str());
    HRESULT hr = vs->Compile(shaderStr.c_str(), shaderStr.length() * sizeof(char), "vs", "vs_5_1", shaderFlags);

    if (!vs->GetIsCompiled())
        OutputDebugStringA(vs->GetErrorMsg());
    ThrowIfFailed(hr);
    m_shaders.push_back(vs);

    hr = ps->Compile(shaderStr.c_str(), shaderStr.length() * sizeof(char), "ps", "ps_5_1", shaderFlags);
    if (!ps->GetIsCompiled())
        OutputDebugStringA(ps->GetErrorMsg());
    ThrowIfFailed(hr);
    m_shaders.push_back(ps);

    CreateRootSignature(parseResult); // [a_vorontsov] Root sig and shader 1 to 1 connection.

    VertexLayoutHandle vertexLayoutHandle = GenerateVertexLayout(parseResult.vertexLayout);

    D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = {};

    auto currentLayout = FindVertexLayout(vertexLayoutHandle);

    desc.InputLayout = { currentLayout->data(), static_cast<UINT>(currentLayout->size()) };
    desc.pRootSignature = m_rootSignature.Get();
    desc.VS = *GetShaderBytecode(vsHandle);
    desc.PS = *GetShaderBytecode(psHandle);
    desc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);

    desc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    desc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
    desc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
    desc.SampleMask = UINT_MAX;
    desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    desc.NumRenderTargets = 1;
    desc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.SampleDesc.Count = 1;

    ThrowIfFailed(m_device->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&m_fallbackPSO)));

    m_commandList->Reset(m_commandAllocators[0].Get(), nullptr);

    m_box = GeometryGenerator::GetUnitCube();
    m_vertexBuffer = std::make_unique<VertexBufferDX12>(m_box->GetVertexData(), m_box->GetVertexDataSize(), m_box->GetVertexDataStride(), m_commandList.Get(), m_device.Get());
    m_indexBuffer = std::make_unique<IndexBufferDX12>(m_box->GetIndexData(), m_box->GetIndexDataSize(), m_commandList.Get(), m_device.Get(), IndexFormatToDXGI(m_box->GetIndexFormat()));

    m_texture = new Texture(WstrToStr(AssetsSystem::GetAssetFullPath(L"Textures\\rick_and_morty.dds")));
    m_textureDX = std::make_unique<TextureDX12>();
    m_textureDX->Path = AssetsSystem::GetAssetFullPath(L"Textures\\rick_and_morty.dds");
    HRESULT texRes = DirectX::CreateDDSTextureFromFile12(m_device.Get(), m_commandList.Get(), m_textureDX->Path.c_str(), m_textureDX->Resource, m_textureDX->UploadResource);
    ThrowIfFailed(texRes);

    D3D12_DESCRIPTOR_HEAP_DESC heapDescr = {};
    heapDescr.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    heapDescr.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    heapDescr.NumDescriptors = 1;
    ThrowIfFailed(m_device->CreateDescriptorHeap(&heapDescr, IID_PPV_ARGS(&m_textureHeap)));

    D3D12_SHADER_RESOURCE_VIEW_DESC texDescr = {};
    texDescr.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    texDescr.Format = m_textureDX->Resource->GetDesc().Format;
    texDescr.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    texDescr.Texture2D.MipLevels = m_textureDX->Resource->GetDesc().MipLevels;
    texDescr.Texture2D.MostDetailedMip = 0;
    texDescr.Texture2D.ResourceMinLODClamp = 0.0f;

    m_device->CreateShaderResourceView(m_textureDX->Resource.Get(), &texDescr, m_textureHeap->GetCPUDescriptorHandleForHeapStart());

    m_commandList->Close();
    ID3D12CommandList* cmdLists[] = { m_commandList.Get() };
    m_commandQueue->ExecuteCommandLists(_countof(cmdLists), cmdLists);

    UpdateTimeCB();
    m_timeBuffer = new UploadBufferDX12(FrameCount, engineBuffers.TimeCB.GetBufferData(), engineBuffers.TimeCB.GetDataSize(), true, m_device.Get());

    m_timeBuffer->UploadData(0, engineBuffers.TimeCB.GetBufferData());
    m_timeBuffer->UploadData(1, engineBuffers.TimeCB.GetBufferData());
    m_timeBuffer->UploadData(2, engineBuffers.TimeCB.GetBufferData());

    m_passBuffer = new UploadBufferDX12(FrameCount, engineBuffers.PassCB.GetBufferData(), engineBuffers.PassCB.GetDataSize(), true, m_device.Get());

    m_passBuffer->UploadData(0, engineBuffers.PassCB.GetBufferData());
    m_passBuffer->UploadData(1, engineBuffers.PassCB.GetBufferData());
    m_passBuffer->UploadData(2, engineBuffers.PassCB.GetBufferData());

    m_renderObjectBuffer = new UploadBufferDX12(FrameCount, engineBuffers.RenderObjectCB.GetBufferData(), engineBuffers.RenderObjectCB.GetDataSize(), true, m_device.Get());
    UpdateRenderObjectCB();
    m_renderObjectBuffer->UploadData(0, engineBuffers.RenderObjectCB.GetBufferData());
    m_renderObjectBuffer->UploadData(1, engineBuffers.RenderObjectCB.GetBufferData());
    m_renderObjectBuffer->UploadData(2, engineBuffers.RenderObjectCB.GetBufferData());

    WaitForGPU();
}

void RendererDX12::Shutdown()
{
    if (m_device != nullptr)
        WaitForGPU();

    if (!m_isTearingSupported)
    {
        ThrowIfFailed(m_swapChain->SetFullscreenState(false, nullptr));
    }
    for (auto& shader : m_shaders)
    {
        delete shader;
    }
    m_shaders.clear();
    for (auto& tex : m_textures)
    {
        delete tex;
    }

    SafeDelete(m_texture);
    SafeDelete(m_timeBuffer);

#ifdef _DEBUG
    ComPtr<IDXGIDebug1> dxgiDebug;
    if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&dxgiDebug))))
        dxgiDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_FLAGS(DXGI_DEBUG_RLO_SUMMARY | DXGI_DEBUG_RLO_IGNORE_INTERNAL));
#endif
    OutputDebugStringA("************************Shutdown\n");
}

void RendererDX12::GetHardwareAdapter(IDXGIFactory4* factory, IDXGIAdapter1** adapter)
{
    *adapter = nullptr;
    ComPtr<IDXGIAdapter1> currAdapter;
    UINT deviceIndex = -1;
    UINT bestVendorIndex = -1;

    static const std::array<UINT, 3> preferredVendors = // [a_vorontsov] http://pcidatabase.com/vendors.php?sort=id
    {
        0x10DE, // NVidia
        0x1002, // AMD
        0x8086 // Intel
    };

    for (UINT index = 0; factory->EnumAdapters1(index, &currAdapter) != DXGI_ERROR_NOT_FOUND; ++index)
    {
        DXGI_ADAPTER_DESC1 desc;
        currAdapter->GetDesc1(&desc);

        if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
            continue;

        if (SUCCEEDED(D3D12CreateDevice(currAdapter.Get(), D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr)))
        {
            for (UINT i = 0; i < preferredVendors.size(); ++i)
            {
                if (desc.VendorId == preferredVendors[i] && i < bestVendorIndex)
                {
                    bestVendorIndex = i;
                    deviceIndex = index;
                    break;
                }
            }
        }
    }
    if (deviceIndex != -1)
        factory->EnumAdapters1(deviceIndex, adapter);
}

void RendererDX12::WaitForGPU()
{
    ThrowIfFailed(m_commandQueue->Signal(m_fence.Get(), m_fenceValues[m_currentFrameIndex]));

    if (m_fence->GetCompletedValue() < m_fenceValues[m_currentFrameIndex])
    {
        HANDLE fenceEventHandle = CreateEvent(nullptr, FALSE, FALSE, nullptr);
        if (fenceEventHandle == nullptr)
        {
            ThrowIfFailed(HRESULT_FROM_WIN32(GetLastError()));
        }
        ThrowIfFailed(m_fence->SetEventOnCompletion(m_fenceValues[m_currentFrameIndex], fenceEventHandle));

        WaitForSingleObjectEx(fenceEventHandle, INFINITE, false);
        CloseHandle(fenceEventHandle);
    }
}

void RendererDX12::Resize(uint16 width, uint16 heigth)
{
    if (m_width == width && m_height == heigth)
        return;
    m_width = width;
    m_height = heigth;

    WaitForGPU();

    for (auto& fenceVal : m_fenceValues)
        fenceVal = m_currentFence;

    m_currentFrameIndex = 0;
    ThrowIfFailed(m_commandList->Reset(m_commandAllocators[m_currentFrameIndex].Get(), nullptr));
    m_depthStencil.Resource.Reset();
    for (auto& swapChainBuffer : m_backBuffers)
        swapChainBuffer.Resource.Reset();

    UINT flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    if (m_isTearingSupported)
        flags |= DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;
    ThrowIfFailed(m_swapChain->ResizeBuffers(FrameCount, width, heigth, m_backBufferFormat, flags));

    BOOL fullscreenState;
    ThrowIfFailed(m_swapChain->GetFullscreenState(&fullscreenState, nullptr));
    m_isSwapChainChainInFullScreen = fullscreenState;

    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart());
    for (UINT i = 0; i < FrameCount; ++i)
    {
        ThrowIfFailed(m_swapChain->GetBuffer(i, IID_PPV_ARGS(&m_backBuffers[i].Resource)));
        m_device->CreateRenderTargetView(m_backBuffers[i].Resource.Get(), nullptr, rtvHandle);
        m_backBuffers[i].CPUdescriptorHandle = rtvHandle;
        rtvHandle.Offset(m_rtvDescriptorSize);
    }

    D3D12_RESOURCE_DESC depthStencilDesc = {};
    depthStencilDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    depthStencilDesc.Alignment = 0;
    depthStencilDesc.Width = width;
    depthStencilDesc.Height = heigth;
    depthStencilDesc.DepthOrArraySize = 1;
    depthStencilDesc.MipLevels = 1;
    depthStencilDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
    depthStencilDesc.SampleDesc.Count = 1;
    depthStencilDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    depthStencilDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

    D3D12_CLEAR_VALUE dsClear = {};
    dsClear.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    dsClear.DepthStencil.Depth = 1.0f;
    dsClear.DepthStencil.Stencil = 0;

    ThrowIfFailed(m_device->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE, &depthStencilDesc, D3D12_RESOURCE_STATE_COMMON, &dsClear, IID_PPV_ARGS(m_depthStencil.Resource.GetAddressOf())));

    D3D12_DEPTH_STENCIL_VIEW_DESC dsViewDesc = {};
    dsViewDesc.Flags = D3D12_DSV_FLAG_NONE;
    dsViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    dsViewDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
    dsViewDesc.Texture2D.MipSlice = 0;

    m_depthStencil.CPUdescriptorHandle = m_dsvHeap->GetCPUDescriptorHandleForHeapStart();
    m_device->CreateDepthStencilView(m_depthStencil.Resource.Get(), &dsViewDesc, m_depthStencil.CPUdescriptorHandle);

    auto transition = CD3DX12_RESOURCE_BARRIER::Transition(m_depthStencil.Resource.Get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_DEPTH_WRITE);
    m_commandList->ResourceBarrier(1, &transition);

    ThrowIfFailed(m_commandList->Close());
    ID3D12CommandList* cmdLists[] = { m_commandList.Get() };
    m_commandQueue->ExecuteCommandLists(_countof(cmdLists), cmdLists);

    WaitForGPU();
    for (auto& v : m_renderPasses)
        v.clear();
}

void RendererDX12::Update(float32 dt)
{
    UpdateTimeCB();
    m_timeBuffer->UploadData(m_currentFrameIndex, engineBuffers.TimeCB.GetBufferData());

    UpdateRenderObjectCB();
    m_renderObjectBuffer->UploadData(m_currentFrameIndex, engineBuffers.RenderObjectCB.GetBufferData());

    UpdatePassCB();
    m_passBuffer->UploadData(m_currentFrameIndex, engineBuffers.PassCB.GetBufferData());
}

void RendererDX12::Present()
{
    m_commandAllocators[m_currentFrameIndex]->Reset();
    m_commandList->Reset(m_commandAllocators[m_currentFrameIndex].Get(), m_fallbackPSO.Get());
    std::vector<RenderPass> thisFramePasses = m_renderPasses[m_currentFrameIndex];

    for (auto& renderPass : thisFramePasses)
    {
        ResourceDX12* currentBackBuffer = FindDxResource(renderPass.GetRenderTarget(0).GetHandle());
        ResourceDX12* currentDS = FindDxResource(renderPass.GetDepthStencil().GetHandle());
        if (currentBackBuffer == nullptr || currentDS == nullptr)
            return;

        auto toRt = CD3DX12_RESOURCE_BARRIER::Transition(currentBackBuffer->Resource.Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
        m_commandList->ResourceBarrier(1, &toRt);

        m_commandList->RSSetScissorRects(1, &DXRectFromKioto(renderPass.GetScissor()));
        m_commandList->RSSetViewports(1, &DXViewportFromKioto(renderPass.GetViewport()));
        m_commandList->ClearRenderTargetView(currentBackBuffer->CPUdescriptorHandle, DirectX::Colors::Aqua, 0, nullptr);
        m_commandList->ClearDepthStencilView(currentDS->CPUdescriptorHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

        m_commandList->OMSetRenderTargets(1, &currentBackBuffer->CPUdescriptorHandle, false, &currentDS->CPUdescriptorHandle);

        m_commandList->SetGraphicsRootSignature(m_rootSignature.Get());
        m_commandList->SetGraphicsRootConstantBufferView(0, m_timeBuffer->GetFrameDataGpuAddress(m_currentFrameIndex));
        m_commandList->SetGraphicsRootConstantBufferView(1, m_passBuffer->GetFrameDataGpuAddress(m_currentFrameIndex));
        m_commandList->SetGraphicsRootConstantBufferView(2, m_renderObjectBuffer->GetFrameDataGpuAddress(m_currentFrameIndex));

        ID3D12DescriptorHeap* descHeap[] = { m_textureHeap.Get() };
        m_commandList->SetDescriptorHeaps(_countof(descHeap), descHeap);

        m_commandList->SetGraphicsRootDescriptorTable(3, m_textureHeap->GetGPUDescriptorHandleForHeapStart());

        m_commandList->IASetVertexBuffers(0, 1, &m_vertexBuffer->GetVertexBufferView());
        m_commandList->IASetIndexBuffer(&m_indexBuffer->GetIndexBufferView());
        m_commandList->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        m_commandList->DrawIndexedInstanced(m_box->GetIndexCount(), 1, 0, 0, 0);

        auto toPresent = CD3DX12_RESOURCE_BARRIER::Transition(currentBackBuffer->Resource.Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
        m_commandList->ResourceBarrier(1, &toPresent);
    }

    m_commandList->Close();
    ID3D12CommandList* cmdLists[] = { m_commandList.Get() };
    m_commandQueue->ExecuteCommandLists(_countof(cmdLists), cmdLists);

    UINT presentFlags = (m_isTearingSupported && !m_isSwapChainChainInFullScreen) ? DXGI_PRESENT_ALLOW_TEARING : 0;
    ThrowIfFailed(m_swapChain->Present(0, presentFlags));

    m_fenceValues[m_currentFrameIndex] = ++m_currentFence;
    m_commandQueue->Signal(m_fence.Get(), m_currentFence);

    m_renderPasses[m_currentFrameIndex].clear();
    // [a_vorontsov] Check if we can move to next frame.
    m_currentFrameIndex = (m_currentFrameIndex + 1) % FrameCount;
    if (m_fenceValues[m_currentFrameIndex] != 0 && m_fence->GetCompletedValue() < m_fenceValues[m_currentFrameIndex])
    {
        HANDLE fenceEventHandle = CreateEvent(nullptr, FALSE, FALSE, nullptr);
        if (fenceEventHandle == nullptr)
        {
            ThrowIfFailed(HRESULT_FROM_WIN32(GetLastError()));
        }
        ThrowIfFailed(m_fence->SetEventOnCompletion(m_fenceValues[m_currentFrameIndex], fenceEventHandle));

        WaitForSingleObjectEx(fenceEventHandle, INFINITE, false);
        CloseHandle(fenceEventHandle);
    }
}

void RendererDX12::LogAdapters()
{
    UINT i = 0;
    IDXGIAdapter* adapter = nullptr;
    std::vector<IDXGIAdapter*> adapterList;
    while (m_factory->EnumAdapters(i, &adapter) != DXGI_ERROR_NOT_FOUND)
    {
        DXGI_ADAPTER_DESC adapterDesc;
        adapter->GetDesc(&adapterDesc);

        std::wstring text = L"--ADAPTER: ";
        text += adapterDesc.Description;
        text += L"\n";
        OutputDebugString(text.c_str());
        adapterList.push_back(adapter);
        ++i;
    }
    for (auto& adapter : adapterList)
    {
        LogAdapterOutputs(adapter);
        ReleaseComPtr(adapter);
    }
}

void RendererDX12::LogAdapterOutputs(IDXGIAdapter* adapter)
{
    UINT i = 0;
    IDXGIOutput* output = nullptr;
    while (adapter->EnumOutputs(i, &output) != DXGI_ERROR_NOT_FOUND)
    {
        DXGI_OUTPUT_DESC desc;
        output->GetDesc(&desc);

        std::wstring text = L"----OUTPUT: ";
        text += desc.DeviceName;
        text += L"\n";
        OutputDebugString(text.c_str());
        LogOutputDisplayModes(output, m_backBufferFormat);
        ReleaseComPtr(output);
        ++i;
    }
}

void RendererDX12::LogOutputDisplayModes(IDXGIOutput* output, DXGI_FORMAT format)
{
    UINT count = 0;
    UINT flags = 0;

    output->GetDisplayModeList(format, flags, &count, nullptr);
    std::vector<DXGI_MODE_DESC> modeList(count);
    output->GetDisplayModeList(format, flags, &count, &modeList[0]);
    for (auto& x : modeList)
    {
        UINT n = x.RefreshRate.Numerator;
        UINT d = x.RefreshRate.Denominator;
        std::wstring text =
            L"Width = " + std::to_wstring(x.Width) + L"  " +
            L"Height = " + std::to_wstring(x.Height) + L"  " +
            L"Refresh = " + std::to_wstring(n) + L"/" + std::to_wstring(d) +
            L"\n";

        OutputDebugString(text.c_str());
    }
}

void RendererDX12::ChangeFullScreenMode(bool fullScreen)
{
    if (fullScreen == m_isFullScreen)
        return;
    m_isFullScreen = fullScreen;
    Resize(m_width, m_height);
}

void RendererDX12::UpdateTimeCB()
{
    float32 timeFromStart = static_cast<float32>(GlobalTimer::GetTimeFromStart());
    engineBuffers.TimeCB.Set("Time", Vector4(timeFromStart / 20.0f, timeFromStart, timeFromStart * 2, timeFromStart * 3));
    engineBuffers.TimeCB.Set("SinTime", Vector4(sin(timeFromStart / 4.0f), sin(timeFromStart / 2.0f), sin(timeFromStart), sin(timeFromStart * 2.0f)));
    engineBuffers.TimeCB.Set("CosTime", Vector4(cos(timeFromStart / 4.0f), cos(timeFromStart / 2.0f), cos(timeFromStart), cos(timeFromStart * 2.0f)));
    float32 dt = static_cast<float32>(GlobalTimer::GetDeltaTime());
    float32 smoothDt = static_cast<float32>(GlobalTimer::GetSmoothDt());
    engineBuffers.TimeCB.Set("DeltaTime", Vector4(dt, 1.0f / dt, smoothDt, 1.0f / smoothDt));
}

void RendererDX12::UpdateRenderObjectCB()
{
    static float32 angle = 0.0f;
    //angle += GlobalTimer::GetDeltaTime();
    angle += 0.001f;
    Matrix4 toWorld = Matrix4::BuildRotation(Vector3(1.0f, 1.0f, 0.0f).Normalize(), angle);
    //toWorld = Matrix4::Identity();
    toWorld.SetTranslation({ 0.0f, 0.0f, 3.0f });

    Matrix4 toModel;
    toWorld.Inversed(toModel);

    engineBuffers.RenderObjectCB.Set("ToModel", toModel.Tranposed());
    engineBuffers.RenderObjectCB.Set("ToWorld", toWorld.Tranposed());
}

void RendererDX12::UpdatePassCB()
{
    CameraComponent* cc = Kioto::GetScene()->GetCameraSystem()->GetMainCamera();
    engineBuffers.PassCB.Set("View", cc->GetView().Tranposed());
    engineBuffers.PassCB.Set("ViewProjection", cc->GetVP().Tranposed());
}

VertexLayoutHandle RendererDX12::GenerateVertexLayout(const VertexLayout& layout)
{
    for (auto& l : m_inputLayouts)
    {
        if (layout == l.LayoutKioto)
            return l.Handle;
    }
    VertexLayoutDX12 res;
    res.Handle = CurrentHandle++;
    res.LayoutDX.reserve(layout.GetElements().size());
    res.LayoutKioto = layout;
    for (const auto& e : layout.GetElements())
    {
        res.LayoutDX.push_back(D3D12_INPUT_ELEMENT_DESC{ SemanticNames[e.Semantic].c_str(), e.SemanticIndex, VertexDataFormats[e.Format], 0, e.Offset, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
    }
    m_inputLayouts.push_back(res);
    return res.Handle;
}

void RendererDX12::AddRenderPass(const RenderPass& renderPass)
{
    m_renderPasses[m_currentFrameIndex].emplace_back(renderPass);
}

ResourceDX12* RendererDX12::FindDxResource(uint32 handle)
{
    if (m_depthStencil.Handle.GetHandle() == handle)
        return &m_depthStencil;
    for (auto& backBuffer : m_backBuffers)
    {
        if (backBuffer.Handle.GetHandle() == handle)
            return &backBuffer;
    }
    return nullptr;
}

const CD3DX12_SHADER_BYTECODE* RendererDX12::GetShaderBytecode(ShaderHandle handle) const
{
    auto it = std::find_if(m_shaders.cbegin(), m_shaders.cend(), [&handle](const ShaderDX12* s) { return s->GetHandle() == handle; });
    if (it != m_shaders.cend() && (*it)->GetIsCompiled())
        return &(*it)->GetBytecode();
    return nullptr;
}

const std::vector<D3D12_INPUT_ELEMENT_DESC>* RendererDX12::FindVertexLayout(VertexLayoutHandle handle) const
{
    for (const auto& l : m_inputLayouts)
    {
        if (l.Handle == handle)
            return &l.LayoutDX;
    }
    return nullptr;
}

void RendererDX12::CreateRootSignature(const ShaderParser::ParseResult& parseResult)
{
    std::vector<CD3DX12_ROOT_PARAMETER1> rootParams;
    for (size_t i = 0; i < parseResult.constantBuffers.size(); ++i)
    {
        CD3DX12_ROOT_PARAMETER1 param;
        param.InitAsConstantBufferView(parseResult.constantBuffers[i].GetIndex(), parseResult.constantBuffers[i].GetSpace());
        rootParams.push_back(std::move(param));
    }
    std::vector<D3D12_DESCRIPTOR_RANGE1> ranges; // [a_vorontsov] Careful, table remembers pointer to range.
    ranges.reserve(parseResult.textureSet.GetTexturesCount());
    if (parseResult.textureSet.GetTexturesCount() > 0)
    {
        ranges.emplace_back();
        D3D12_DESCRIPTOR_RANGE1* texRange = &ranges.back();
        texRange->NumDescriptors = 1;
        texRange->BaseShaderRegister = 0;
        texRange->OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
        texRange->Flags = D3D12_DESCRIPTOR_RANGE_FLAG_NONE;
        texRange->RegisterSpace = 0;
        texRange->RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;

        CD3DX12_ROOT_PARAMETER1 table;
        table.InitAsDescriptorTable(1, texRange, D3D12_SHADER_VISIBILITY_PIXEL);
        rootParams.push_back(std::move(table));
    }

    D3D12_FEATURE_DATA_ROOT_SIGNATURE signatureData = {};
    signatureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;

    if (FAILED(m_device->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &signatureData, sizeof(signatureData))))
        signatureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;

    D3D12_ROOT_SIGNATURE_FLAGS flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
    CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
    auto staticSamplers = GetStaticSamplers();
    rootSignatureDesc.Init_1_1(static_cast<UINT>(rootParams.size()), rootParams.data(), static_cast<UINT>(staticSamplers.size()), staticSamplers.data(), flags);

    ComPtr<ID3DBlob> signature;
    ComPtr<ID3DBlob> rootSignatureCreationError;
    ThrowIfFailed(D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, signatureData.HighestVersion, &signature, &rootSignatureCreationError));
    HRESULT hr = m_device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&m_rootSignature));
    if (hr != S_OK)
        OutputDebugStringA(reinterpret_cast<char*>(rootSignatureCreationError->GetBufferPointer()));
    ThrowIfFailed(hr);

    NAME_D3D12_OBJECT(m_rootSignature);
}

void RendererDX12::RegisterTexture(Texture* texture)
{
    auto it = std::find_if(m_textures.cbegin(), m_textures.cend(), [texture](const TextureDX12* tex) { return tex->GetTextureHandle() == texture->GetTextureHandle(); });
    if (it != m_textures.cend())
        throw "Texture Already Registred";
    m_textures.emplace_back();
    m_textures.back()->Path = StrToWstr(texture->GetAssetPath());
    m_textures.back()->Create(m_device.Get(), m_commandList.Get());
    m_textures.back()->SetTextureHandle(CurrentHandle++);
    texture->SetTextureHandle(m_textures.back()->GetTextureHandle());
}

void RendererDX12::UpdateTextureSetHeap(const TextureSet& texSet)
{
    if (m_textureHeaps.find(texSet.GetHandle()) != m_textureHeaps.end())
        m_textureHeaps[texSet.GetHandle()].Reset();

    D3D12_DESCRIPTOR_HEAP_DESC heapDescr = {};
    heapDescr.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    heapDescr.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    heapDescr.NumDescriptors = texSet.GetTexturesCount();
    ThrowIfFailed(m_device->CreateDescriptorHeap(&heapDescr, IID_PPV_ARGS(&m_textureHeaps[texSet.GetHandle()])));

    CD3DX12_CPU_DESCRIPTOR_HANDLE handle(m_textureHeaps[texSet.GetHandle()]->GetCPUDescriptorHandleForHeapStart());
    for (uint32 i = 0; i < texSet.GetTexturesCount(); ++i)
    {
        const Texture* kiotoTex = texSet.GetTexture(i);
        auto it = std::find_if(m_textures.begin(), m_textures.end(), [&kiotoTex](const TextureDX12* tex) { return kiotoTex->GetTextureHandle() == tex->GetTextureHandle(); });
        if (it == m_textures.end())
            throw "ololo";
        TextureDX12* dxTex = *it;

        D3D12_SHADER_RESOURCE_VIEW_DESC texDescr = {};
        texDescr.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        texDescr.Format = dxTex->Resource->GetDesc().Format;
        texDescr.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
        texDescr.Texture2D.MipLevels = dxTex->Resource->GetDesc().MipLevels;
        texDescr.Texture2D.MostDetailedMip = 0;
        texDescr.Texture2D.ResourceMinLODClamp = 0.0f;

        m_device->CreateShaderResourceView(dxTex->Resource.Get(), &texDescr, handle);
        handle.Offset(m_cbvSrvUavDescriptorSize);
    }
}

ID3D12DescriptorHeap* RendererDX12::GetTextureHeap(TextureSetHandle handle)
{
    return m_textureHeaps[handle].Get();
}

}