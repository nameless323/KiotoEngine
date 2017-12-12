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

#include "Component/CameraComponent.h"
#include "Systems/CameraSystem.h"
#include "Core/Scene.h"
#include "Core/KiotoEngine.h" // [a_vorontsov] For now. TODO: render pass with render target and so on. This class shouldn't know 'bout camera and so on.

namespace Kioto::Renderer
{

using Microsoft::WRL::ComPtr;
using std::wstring;

RendererDX12::RendererDX12()
{
    m_inputLayouts.reserve(256);
}

void RendererDX12::Init(uint16 width, uint16 height)
{
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

    m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();

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
    ComPtr<ID3DBlob> shaderError;
    HRESULT hr = D3DCompileFromFile(shaderPath.c_str(), nullptr, nullptr, "vs", "vs_5_1", shaderFlags, 0, &m_vsFallbackByteCode, &shaderError);
    if (shaderError != nullptr)
        OutputDebugStringA(reinterpret_cast<char*>(shaderError->GetBufferPointer()));
    ThrowIfFailed(hr);

    hr = D3DCompileFromFile(shaderPath.c_str(), nullptr, nullptr, "ps", "ps_5_1", shaderFlags, 0, &m_psFallbackByteCode, &shaderError);
    if (shaderError != nullptr)
        OutputDebugStringA(reinterpret_cast<char*>(shaderError->GetBufferPointer()));
    ThrowIfFailed(hr);

    D3D12_INPUT_ELEMENT_DESC inputElementDesc[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
    };

    D3D12_FEATURE_DATA_ROOT_SIGNATURE signatureData = {};
    signatureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;

    if (FAILED(m_device->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &signatureData, sizeof(signatureData))))
    {
        signatureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
    }

    D3D12_ROOT_SIGNATURE_FLAGS flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
    CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;

    D3D12_DESCRIPTOR_RANGE1 texRange = {};
    texRange.NumDescriptors = 1;
    texRange.BaseShaderRegister = 0;
    texRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
    texRange.Flags = D3D12_DESCRIPTOR_RANGE_FLAG_NONE;
    texRange.RegisterSpace = 0;
    texRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;

    CD3DX12_ROOT_PARAMETER1 rootParam[4];
    rootParam[0].InitAsConstantBufferView(0);
    rootParam[1].InitAsConstantBufferView(1);
    rootParam[2].InitAsConstantBufferView(2);
    rootParam[3].InitAsDescriptorTable(1, &texRange, D3D12_SHADER_VISIBILITY_PIXEL);
    auto staticSamplers = GetStaticSamplers();
    rootSignatureDesc.Init_1_1(4, rootParam, static_cast<UINT>(staticSamplers.size()), staticSamplers.data(), flags);

    ComPtr<ID3DBlob> signature;
    ComPtr<ID3DBlob> rootSignatureCreationError;
    ThrowIfFailed(D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, signatureData.HighestVersion, &signature, &rootSignatureCreationError));
    hr = m_device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&m_rootSignature));
    if (hr != S_OK)
        OutputDebugStringA(reinterpret_cast<char*>(rootSignatureCreationError->GetBufferPointer()));
    ThrowIfFailed(hr);

    NAME_D3D12_OBJECT(m_rootSignature);

    D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = {};

    desc.InputLayout = { inputElementDesc, _countof(inputElementDesc) };
    desc.pRootSignature = m_rootSignature.Get();
    desc.VS = CD3DX12_SHADER_BYTECODE(m_vsFallbackByteCode.Get());
    desc.PS = CD3DX12_SHADER_BYTECODE(m_psFallbackByteCode.Get());
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

    m_box = GeometryGenerator::GenerateCube();
    m_vertexBuffer = std::make_unique<VertexBufferDX12>(m_box.GetVertexData(), m_box.GetVertexDataSize(), m_box.GetVertexDataStride(), m_commandList.Get(), m_device.Get());
    m_indexBuffer = std::make_unique<IndexBufferDX12>(m_box.GetIndexData(), m_box.GetIndexDataSize(), m_commandList.Get(), m_device.Get(), IndexFormatToDXGI(m_box.GetIndexFormat()));

    m_texture = std::make_unique<Texture>();
    m_texture->Path = AssetsSystem::GetAssetFullPath(L"Textures\\rick_and_morty.dds");
    HRESULT texRes = DirectX::CreateDDSTextureFromFile12(m_device.Get(), m_commandList.Get(), m_texture->Path.c_str(), m_texture->Resource, m_texture->UploadResource);
    ThrowIfFailed(texRes);

    D3D12_DESCRIPTOR_HEAP_DESC heapDescr = {};
    heapDescr.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    heapDescr.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    heapDescr.NumDescriptors = 1;
    ThrowIfFailed(m_device->CreateDescriptorHeap(&heapDescr, IID_PPV_ARGS(&m_textureHeap)));

    D3D12_SHADER_RESOURCE_VIEW_DESC texDescr = {};
    texDescr.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    texDescr.Format = m_texture->Resource->GetDesc().Format;
    texDescr.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    texDescr.Texture2D.MipLevels = m_texture->Resource->GetDesc().MipLevels;
    texDescr.Texture2D.MostDetailedMip = 0;
    texDescr.Texture2D.ResourceMinLODClamp = 0.0f;

    m_device->CreateShaderResourceView(m_texture->Resource.Get(), &texDescr, m_textureHeap->GetCPUDescriptorHandleForHeapStart());

    m_commandList->Close();
    ID3D12CommandList* cmdLists[] = { m_commandList.Get() };
    m_commandQueue->ExecuteCommandLists(_countof(cmdLists), cmdLists);

    m_timeBuffer = std::make_unique<UploadBuffer<TimeConstantBuffer>>(FrameCount, true, m_device.Get());
    TimeConstantBuffer timeBuffer;
    UpdateTimeCB(timeBuffer);

    m_timeBuffer->UploadData(0, timeBuffer);
    m_timeBuffer->UploadData(1, timeBuffer);
    m_timeBuffer->UploadData(2, timeBuffer);

    m_passBuffer = std::make_unique<UploadBuffer<PassBuffer>>(FrameCount, true, m_device.Get());
    PassBuffer passBuffer;

    passBuffer.View = Matrix4::Identity;
    passBuffer.ViewProjection = Matrix4::Identity;
    m_passBuffer->UploadData(0, passBuffer);
    m_passBuffer->UploadData(1, passBuffer);
    m_passBuffer->UploadData(2, passBuffer);

    m_renderObjectBuffer = std::make_unique<UploadBuffer<RenderObjectBuffer>>(FrameCount, true, m_device.Get());
    RenderObjectBuffer roBuff;
    UpdateRenderObjectCB(roBuff);
    m_renderObjectBuffer->UploadData(0, roBuff);
    m_renderObjectBuffer->UploadData(1, roBuff);
    m_renderObjectBuffer->UploadData(2, roBuff);

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
    ThrowIfFailed(m_commandQueue->Signal(m_fence.Get(), m_fenceValues[m_frameIndex]));

    if (m_fence->GetCompletedValue() < m_fenceValues[m_frameIndex])
    {
        HANDLE fenceEventHandle = CreateEvent(nullptr, FALSE, FALSE, nullptr);
        if (fenceEventHandle == nullptr)
        {
            ThrowIfFailed(HRESULT_FROM_WIN32(GetLastError()));
        }
        ThrowIfFailed(m_fence->SetEventOnCompletion(m_fenceValues[m_frameIndex], fenceEventHandle));

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

    m_frameIndex = 0;
    ThrowIfFailed(m_commandList->Reset(m_commandAllocators[m_frameIndex].Get(), nullptr));
    m_depthStencilBuffer.Reset();
    for (auto& swapChainBuffer : m_swapChainBuffers)
        swapChainBuffer.Reset();

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
        ThrowIfFailed(m_swapChain->GetBuffer(i, IID_PPV_ARGS(&m_swapChainBuffers[i])));
        m_device->CreateRenderTargetView(m_swapChainBuffers[i].Get(), nullptr, rtvHandle);
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

    ThrowIfFailed(m_device->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE, &depthStencilDesc, D3D12_RESOURCE_STATE_COMMON, &dsClear, IID_PPV_ARGS(m_depthStencilBuffer.GetAddressOf())));

    D3D12_DEPTH_STENCIL_VIEW_DESC dsViewDesc = {};
    dsViewDesc.Flags = D3D12_DSV_FLAG_NONE;
    dsViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    dsViewDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
    dsViewDesc.Texture2D.MipSlice = 0;

    m_device->CreateDepthStencilView(m_depthStencilBuffer.Get(), &dsViewDesc, GetDepthStencilView());

    auto transition = CD3DX12_RESOURCE_BARRIER::Transition(m_depthStencilBuffer.Get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_DEPTH_WRITE);
    m_commandList->ResourceBarrier(1, &transition);

    ThrowIfFailed(m_commandList->Close());
    ID3D12CommandList* cmdLists[] = { m_commandList.Get() };
    m_commandQueue->ExecuteCommandLists(_countof(cmdLists), cmdLists);

    WaitForGPU();

    m_viewport.TopLeftX = 0;
    m_viewport.TopLeftY = 0;
    m_viewport.Width = static_cast<float>(width);
    m_viewport.Height = static_cast<float>(heigth);
    m_viewport.MinDepth = 0.0f;
    m_viewport.MaxDepth = 1.0f;

    m_scissor = { 0, 0, width, heigth };
}

ID3D12Resource* RendererDX12::GetCurrentBackBuffer() const
{
    return m_swapChainBuffers[m_frameIndex].Get();
}

D3D12_CPU_DESCRIPTOR_HANDLE RendererDX12::GetDepthStencilView() const
{
    return m_dsvHeap->GetCPUDescriptorHandleForHeapStart();
}

D3D12_CPU_DESCRIPTOR_HANDLE RendererDX12::GetCurrentBackBufferView() const
{
    return CD3DX12_CPU_DESCRIPTOR_HANDLE(m_rtvHeap->GetCPUDescriptorHandleForHeapStart(), m_frameIndex, m_rtvDescriptorSize);
}

void RendererDX12::Update(float32 dt)
{
    TimeConstantBuffer timeBuffer;
    UpdateTimeCB(timeBuffer);
    m_timeBuffer->UploadData(m_frameIndex, timeBuffer);

    RenderObjectBuffer roBuffer;
    UpdateRenderObjectCB(roBuffer);
    m_renderObjectBuffer->UploadData(m_frameIndex, roBuffer);

    PassBuffer pBuffer;
    UpdatePassCB(pBuffer);
    m_passBuffer->UploadData(m_frameIndex, pBuffer);
}

void RendererDX12::Present()
{
    m_commandAllocators[m_frameIndex]->Reset();
    m_commandList->Reset(m_commandAllocators[m_frameIndex].Get(), m_fallbackPSO.Get());

    auto toRt = CD3DX12_RESOURCE_BARRIER::Transition(GetCurrentBackBuffer(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
    m_commandList->ResourceBarrier(1, &toRt);

    m_commandList->RSSetScissorRects(1, &m_scissor);
    m_commandList->RSSetViewports(1, &m_viewport);
    m_commandList->ClearRenderTargetView(GetCurrentBackBufferView(), DirectX::Colors::Aqua, 0, nullptr);
    m_commandList->ClearDepthStencilView(GetDepthStencilView(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

    m_commandList->OMSetRenderTargets(1, &GetCurrentBackBufferView(), false, &GetDepthStencilView());

    m_commandList->SetGraphicsRootSignature(m_rootSignature.Get());
    m_commandList->SetGraphicsRootConstantBufferView(0, m_timeBuffer->GetElementGpuAddress(m_frameIndex));
    m_commandList->SetGraphicsRootConstantBufferView(1, m_passBuffer->GetElementGpuAddress(m_frameIndex));
    m_commandList->SetGraphicsRootConstantBufferView(2, m_renderObjectBuffer->GetElementGpuAddress(m_frameIndex));

    ID3D12DescriptorHeap* descHeap[] = { m_textureHeap.Get() };
    m_commandList->SetDescriptorHeaps(_countof(descHeap), descHeap);

    m_commandList->SetGraphicsRootDescriptorTable(3, m_textureHeap->GetGPUDescriptorHandleForHeapStart());

    m_commandList->IASetVertexBuffers(0, 1, &m_vertexBuffer->GetVertexBufferView());
    m_commandList->IASetIndexBuffer(&m_indexBuffer->GetIndexBufferView());
    m_commandList->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    m_commandList->DrawIndexedInstanced(m_box.GetIndexCount(), 1, 0, 0, 0);

    auto toPresent = CD3DX12_RESOURCE_BARRIER::Transition(GetCurrentBackBuffer(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
    m_commandList->ResourceBarrier(1, &toPresent);
    m_commandList->Close();
    ID3D12CommandList* cmdLists[] = { m_commandList.Get() };
    m_commandQueue->ExecuteCommandLists(_countof(cmdLists), cmdLists);

    UINT presentFlags = (m_isTearingSupported && !m_isSwapChainChainInFullScreen) ? DXGI_PRESENT_ALLOW_TEARING : 0;
    ThrowIfFailed(m_swapChain->Present(0, presentFlags));

    m_fenceValues[m_frameIndex] = ++m_currentFence;
    m_commandQueue->Signal(m_fence.Get(), m_currentFence);

    // [a_vorontsov] Check if we can move to next frame.
    m_frameIndex = (m_frameIndex + 1) % FrameCount;
    if (m_fenceValues[m_frameIndex] != 0 && m_fence->GetCompletedValue() < m_fenceValues[m_frameIndex])
    {
        HANDLE fenceEventHandle = CreateEvent(nullptr, FALSE, FALSE, nullptr);
        if (fenceEventHandle == nullptr)
        {
            ThrowIfFailed(HRESULT_FROM_WIN32(GetLastError()));
        }
        ThrowIfFailed(m_fence->SetEventOnCompletion(m_fenceValues[m_frameIndex], fenceEventHandle));

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

void RendererDX12::UpdateTimeCB(TimeConstantBuffer& buffer)
{
    float32 timeFromStart = static_cast<float32>(GlobalTimer::GetTimeFromStart());
    buffer.Time = Vector4(timeFromStart / 20.0f, timeFromStart, timeFromStart * 2, timeFromStart * 3);
    buffer.SinTime = Vector4(sin(timeFromStart / 4.0f), sin(timeFromStart / 2.0f), sin(timeFromStart), sin(timeFromStart * 2.0f));
    buffer.CosTime = Vector4(cos(timeFromStart / 4.0f), cos(timeFromStart / 2.0f), cos(timeFromStart), cos(timeFromStart * 2.0f));
    float32 dt = static_cast<float32>(GlobalTimer::GetDeltaTime());
    float32 smoothDt = static_cast<float32>(GlobalTimer::GetSmoothDt());
    buffer.DeltaTime = Vector4(dt, 1.0f / dt, smoothDt, 1.0f / smoothDt);
}

void RendererDX12::UpdateRenderObjectCB(RenderObjectBuffer& buffer)
{
    static float32 angle = 0.0f;
    //angle += GlobalTimer::GetDeltaTime();
    angle += 0.001f;
    Matrix4 toWorld = Matrix4::BuildRotation(Vector3(1.0f, 1.0f, 0.0f).Normalize(), angle);
    //toWorld = Matrix4::Identity();
    toWorld.SetTranslation({ 0.0f, 0.0f, 3.0f });

    Matrix4 toModel;
    toWorld.Inversed(toModel);

    buffer.ToModel = toModel.Tranposed();
    buffer.ToWorld = toWorld.Tranposed();
}

void RendererDX12::UpdatePassCB(PassBuffer& buffer)
{
    CameraComponent* cc = Kioto::GetScene()->GetCameraSystem()->GetMainCamera();
    buffer.View = cc->GetView().Tranposed();
    buffer.ViewProjection = cc->GetVP().Tranposed();
}

Handle RendererDX12::GenerateVertexLayout(const VertexLayout& layout) const
{
    Handle res = static_cast<Handle>(m_inputLayouts.size());
    std::vector<D3D12_INPUT_ELEMENT_DESC> currentLayout;
    currentLayout.reserve(16);
    for (const auto& e : layout.GetElements()) // [a_vorontsov] TODO: Check if layout exist.
    {
        currentLayout.push_back(D3D12_INPUT_ELEMENT_DESC{ SemanticNames[e.Semantic].c_str(), e.SemanticIndex, VertexDataFormats[e.Format], 0, e.Offset, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
    }
    return res;
}

}