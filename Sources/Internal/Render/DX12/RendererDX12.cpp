//
// Copyright (C) Alexandr Vorontsov. 2017
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#include "stdafx.h"

#include <array>
#include <string>
#include <vector>

#include "Core/WindowsApplication.h"
#include "Render/DX12/RendererDX12.h"
#include "AssetsSystem/AssetsSystem.h"

namespace Kioto::Renderer
{

using Microsoft::WRL::ComPtr;
using std::wstring;
using DirectX::XMFLOAT3;

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
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
    };

    D3D12_FEATURE_DATA_ROOT_SIGNATURE signatureData = {};
    signatureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;

    if (FAILED(m_device->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &signatureData, sizeof(signatureData))))
    {
        signatureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
    }

    D3D12_ROOT_SIGNATURE_FLAGS flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
    CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
    rootSignatureDesc.Init_1_1(0, nullptr, 0, nullptr, flags);

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
    desc.DepthStencilState.DepthEnable = FALSE;
    desc.DepthStencilState.StencilEnable = FALSE;
    desc.SampleMask = UINT_MAX;
    desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    desc.NumRenderTargets = 1;
    desc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.SampleDesc.Count = 1;

    ThrowIfFailed(m_device->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&m_fallbackPSO)));

    DirectX::XMFLOAT3 verts[] =
    {
        { 0.0f, 0.0f, 0.1f },
        { 0.5f, 1.0f, 0.1f },
        { 1.0f, 0.0f, 0.1f }
    };
    CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_UPLOAD);
    CD3DX12_RESOURCE_DESC vertBufferDesc = CD3DX12_RESOURCE_DESC::Buffer(sizeof(verts));
    ThrowIfFailed(m_device->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &vertBufferDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&m_vertexBuffer)));
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

    ThrowIfFailed(m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_commandAllocator)));
    NAME_D3D12_OBJECT(m_commandAllocator);
    ThrowIfFailed(m_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_commandAllocator.Get(), nullptr, IID_PPV_ARGS(&m_commandList))); // [a_vorontsov] Maybe not 0 as mask?
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

    LoadPipeline();
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
    m_currFenceValue++;
    ThrowIfFailed(m_commandQueue->Signal(m_fence.Get(), m_currFenceValue));

    if (m_fence->GetCompletedValue() < m_currFenceValue)
    {
        HANDLE fenceEventHandle = CreateEvent(nullptr, FALSE, FALSE, nullptr);
        if (fenceEventHandle == nullptr)
        {
            ThrowIfFailed(HRESULT_FROM_WIN32(GetLastError()));
        }
        ThrowIfFailed(m_fence->SetEventOnCompletion(m_currFenceValue, fenceEventHandle));

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

    ThrowIfFailed(m_commandList->Reset(m_commandAllocator.Get(), nullptr));
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

    m_frameIndex = 0;

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

void RendererDX12::Present()
{
    WaitForGPU();

    m_commandAllocator->Reset();
    m_commandList->Reset(m_commandAllocator.Get(), nullptr);

    auto toRt = CD3DX12_RESOURCE_BARRIER::Transition(GetCurrentBackBuffer(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
    m_commandList->ResourceBarrier(1, &toRt);

    m_commandList->RSSetScissorRects(1, &m_scissor);
    m_commandList->RSSetViewports(1, &m_viewport);
    m_commandList->ClearRenderTargetView(GetCurrentBackBufferView(), DirectX::Colors::Aqua, 0, nullptr);
    m_commandList->ClearDepthStencilView(GetDepthStencilView(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

    auto toPresent = CD3DX12_RESOURCE_BARRIER::Transition(GetCurrentBackBuffer(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
    m_commandList->ResourceBarrier(1, &toPresent);

    m_commandList->OMSetRenderTargets(1, &GetCurrentBackBufferView(), false, &GetDepthStencilView());

    m_commandList->Close();
    ID3D12CommandList* cmdLists[] = { m_commandList.Get() };
    m_commandQueue->ExecuteCommandLists(_countof(cmdLists), cmdLists);

    UINT presentFlags = (m_isTearingSupported && !m_isSwapChainChainInFullScreen) ? DXGI_PRESENT_ALLOW_TEARING : 0;
    ThrowIfFailed(m_swapChain->Present(0, presentFlags));

    m_frameIndex = (m_frameIndex + 1) % FrameCount;
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

}