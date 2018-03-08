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
#include "Render/Material.h"

#include "Component/CameraComponent.h"
#include "Systems/CameraSystem.h"
#include "Core/Scene.h"
#include "Core/KiotoEngine.h" // [a_vorontsov] For now. TODO: render pass with render target and so on. This class shouldn't know 'bout camera and so on.

#include "Render/DX12/Buffers/UploadBufferDX12.h"
#include "Render/Texture/Texture.h"
#include "Render/ShaderData.h"

namespace Kioto::Renderer
{

using Microsoft::WRL::ComPtr;
using std::wstring;

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
}

void RendererDX12::Init(uint16 width, uint16 height)
{
    engineBuffers.Init();

    m_renderPacketListPool.resize(PacketListPoolSize);
    for (uint32 i = 0; i < PacketListPoolSize; ++i)
    {
        m_renderPacketListPool.emplace_back();
        m_renderPacketListPool.back().reserve(PacketListSize);
    }

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
    ThrowIfFailed(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&m_state.Factory)));

    BOOL allowTearing = FALSE;
    ComPtr<IDXGIFactory5> factory5;
    ThrowIfFailed(m_state.Factory.As(&factory5));
    HRESULT hr = factory5->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING, &allowTearing, sizeof(allowTearing));
    m_isTearingSupported = SUCCEEDED(hr) && allowTearing;

    ComPtr<IDXGIAdapter1> hardwareAdapter;
    GetHardwareAdapter(m_state.Factory.Get(), &hardwareAdapter);

    ThrowIfFailed(D3D12CreateDevice(hardwareAdapter.Get(), D3D_FEATURE_LEVEL_12_1, IID_PPV_ARGS(&m_state.Device)));

    D3D12_COMMAND_QUEUE_DESC queueDesc = {};
    queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

    ThrowIfFailed(m_state.Device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_state.CommandQueue)));
    NAME_D3D12_OBJECT(m_state.CommandQueue);

    m_swapChain.Init(m_state, m_isTearingSupported, width, height);
    for (uint8 i = 0; i < StateDX::FrameCount; ++i)
        m_textureManager.RegisterTextureWithoutOwnership(m_swapChain.GetBackBuffer(i));
    m_textureManager.RegisterTextureWithoutOwnership(m_swapChain.GetDepthStencil());

    for (uint32 i = 0; i < StateDX::FrameCount; ++i)
    {
        ThrowIfFailed(m_state.Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_state.CommandAllocators[i])));
        NAME_D3D12_OBJECT(m_state.CommandAllocators[i]);
    }
    ThrowIfFailed(m_state.Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_state.CommandAllocators[0].Get(), nullptr, IID_PPV_ARGS(&m_state.CommandList))); // [a_vorontsov] Maybe not 0 as mask?
    NAME_D3D12_OBJECT(m_state.CommandList);
    m_state.CommandList->Close();

    ThrowIfFailed(m_state.Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_state.Fence)));
    NAME_D3D12_OBJECT(m_state.Fence);

    m_state.CbvSrvUavDescriptorSize = m_state.Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    m_state.RtvDescriptorSize = m_state.Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    m_state.DsvDescriptorSize = m_state.Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
    m_state.SamplerDescriptorSize = m_state.Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);

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
    m_state.CommandList->Reset(m_state.CommandAllocators[0].Get(), nullptr);

    std::string path = AssetsSystem::GetAssetFullPath(R"(Models\Teapot.fbx)");
    m_box = new Mesh(path);

    m_vertexBuffer = std::make_unique<VertexBufferDX12>(m_box->GetVertexData(), m_box->GetVertexDataSize(), m_box->GetVertexDataStride(), m_state.CommandList.Get(), m_state.Device.Get());
    m_indexBuffer = std::make_unique<IndexBufferDX12>(m_box->GetIndexData(), m_box->GetIndexDataSize(), m_state.CommandList.Get(), m_state.Device.Get(), IndexFormatToDXGI(m_box->GetIndexFormat()));

    m_state.CommandList->Close();
    ID3D12CommandList* cmdLists[] = { m_state.CommandList.Get() };
    m_state.CommandQueue->ExecuteCommandLists(_countof(cmdLists), cmdLists);

    UpdateTimeCB();
    m_timeBuffer = new UploadBufferDX12(StateDX::FrameCount, engineBuffers.TimeCB.GetBufferData(), engineBuffers.TimeCB.GetDataSize(), true, m_state.Device.Get());

    m_timeBuffer->UploadData(0, engineBuffers.TimeCB.GetBufferData());
    m_timeBuffer->UploadData(1, engineBuffers.TimeCB.GetBufferData());
    m_timeBuffer->UploadData(2, engineBuffers.TimeCB.GetBufferData());

    m_passBuffer = new UploadBufferDX12(StateDX::FrameCount, engineBuffers.PassCB.GetBufferData(), engineBuffers.PassCB.GetDataSize(), true, m_state.Device.Get());

    m_passBuffer->UploadData(0, engineBuffers.PassCB.GetBufferData());
    m_passBuffer->UploadData(1, engineBuffers.PassCB.GetBufferData());
    m_passBuffer->UploadData(2, engineBuffers.PassCB.GetBufferData());

    m_renderObjectBuffer = new UploadBufferDX12(StateDX::FrameCount, engineBuffers.RenderObjectCB.GetBufferData(), engineBuffers.RenderObjectCB.GetDataSize(), true, m_state.Device.Get());
    UpdateRenderObjectCB();
    m_renderObjectBuffer->UploadData(0, engineBuffers.RenderObjectCB.GetBufferData());
    m_renderObjectBuffer->UploadData(1, engineBuffers.RenderObjectCB.GetBufferData());
    m_renderObjectBuffer->UploadData(2, engineBuffers.RenderObjectCB.GetBufferData());

    WaitForGPU();
}

void RendererDX12::Shutdown()
{
    if (m_state.Device != nullptr)
        WaitForGPU();

    if (!m_isTearingSupported)
        ThrowIfFailed(m_swapChain.SetFullscreenState(false, nullptr));

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
    ThrowIfFailed(m_state.CommandQueue->Signal(m_state.Fence.Get(), m_state.FenceValues[m_swapChain.GetCurrentFrameIndex()]));

    if (m_state.Fence->GetCompletedValue() < m_state.FenceValues[m_swapChain.GetCurrentFrameIndex()])
    {
        HANDLE fenceEventHandle = CreateEvent(nullptr, FALSE, FALSE, nullptr);
        if (fenceEventHandle == nullptr)
        {
            ThrowIfFailed(HRESULT_FROM_WIN32(GetLastError()));
        }
        ThrowIfFailed(m_state.Fence->SetEventOnCompletion(m_state.FenceValues[m_swapChain.GetCurrentFrameIndex()], fenceEventHandle));

        WaitForSingleObjectEx(fenceEventHandle, INFINITE, false);
        CloseHandle(fenceEventHandle);
    }
}

void RendererDX12::Resize(uint16 width, uint16 height)
{
    if (m_width == width && m_height == height)
        return;
    m_width = width;
    m_height = height;

    WaitForGPU();

    for (auto& fenceVal : m_state.FenceValues)
        fenceVal = m_state.CurrentFence;

    ThrowIfFailed(m_state.CommandList->Reset(m_state.CommandAllocators[m_swapChain.GetCurrentFrameIndex()].Get(), nullptr));

    m_swapChain.Resize(m_state, width, height);

    ThrowIfFailed(m_state.CommandList->Close());
    ID3D12CommandList* cmdLists[] = { m_state.CommandList.Get() };
    m_state.CommandQueue->ExecuteCommandLists(_countof(cmdLists), cmdLists);

    WaitForGPU();
    for (auto& v : m_renderPasses)
        v.clear();
}

void RendererDX12::Update(float32 dt)
{
    UpdateTimeCB();
    m_timeBuffer->UploadData(m_swapChain.GetCurrentFrameIndex(), engineBuffers.TimeCB.GetBufferData());

    UpdateRenderObjectCB();
    m_renderObjectBuffer->UploadData(m_swapChain.GetCurrentFrameIndex(), engineBuffers.RenderObjectCB.GetBufferData());

    UpdatePassCB();
    m_passBuffer->UploadData(m_swapChain.GetCurrentFrameIndex(), engineBuffers.PassCB.GetBufferData());
}

void RendererDX12::Present()
{
    m_state.CommandAllocators[m_swapChain.GetCurrentFrameIndex()]->Reset();
    m_state.CommandList->Reset(m_state.CommandAllocators[m_swapChain.GetCurrentFrameIndex()].Get(), nullptr);

    m_textureManager.ProcessRegistationQueue(m_state);
    m_textureManager.ProcessTextureSetUpdates(m_state);

    std::vector<RenderPass> thisFramePasses = m_renderPasses[m_swapChain.GetCurrentFrameIndex()];
    for (auto& renderPass : thisFramePasses)
    {
        auto passPacketsIt = m_passesRenderPackets.find(renderPass.GetHandle());
        if (passPacketsIt == m_passesRenderPackets.cend())
            continue;
        std::vector<RenderPacket>* passPackets = passPacketsIt->second;

        TextureDX12* currentRenderTarget = nullptr;
        if (renderPass.GetRenderTarget(0).GetHandle() == InvalidHandle)
            currentRenderTarget = m_swapChain.GetCurrentBackBuffer();
        else
            currentRenderTarget = m_textureManager.FindTexture(renderPass.GetRenderTarget(0).GetHandle());

        TextureDX12* currentDS = nullptr;
        if (renderPass.GetDepthStencil().GetHandle() == InvalidHandle)
            currentDS = m_swapChain.GetDepthStencil();
        else
            currentDS = m_textureManager.FindTexture(renderPass.GetDepthStencil().GetHandle());
        
        if (currentRenderTarget == nullptr || currentDS == nullptr)
            return;

        auto toRt = CD3DX12_RESOURCE_BARRIER::Transition(currentRenderTarget->Resource.Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
        m_state.CommandList->ResourceBarrier(1, &toRt);

        m_state.CommandList->RSSetScissorRects(1, &DXRectFromKioto(renderPass.GetScissor()));
        m_state.CommandList->RSSetViewports(1, &DXViewportFromKioto(renderPass.GetViewport()));
        m_state.CommandList->ClearRenderTargetView(currentRenderTarget->GetCPUHandle(), DirectX::Colors::Aqua, 0, nullptr);
        m_state.CommandList->ClearDepthStencilView(currentDS->GetCPUHandle(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

        m_state.CommandList->OMSetRenderTargets(1, &currentRenderTarget->GetCPUHandle(), false, &currentDS->GetCPUHandle());

        for (const RenderPacket& packet : *passPackets)
        {
            ID3D12PipelineState* pipelineState = m_piplineStateManager.GetPipelineState(packet.Material.GetHandle(), renderPass.GetHandle());
            m_state.CommandList->SetPipelineState(pipelineState);

            ID3D12RootSignature* rootSig = m_rootSignatureManager.GetRootSignature(packet.Shader);
            m_state.CommandList->SetGraphicsRootSignature(rootSig);
            m_state.CommandList->SetGraphicsRootConstantBufferView(0, m_timeBuffer->GetFrameDataGpuAddress(m_swapChain.GetCurrentFrameIndex()));
            m_state.CommandList->SetGraphicsRootConstantBufferView(1, m_passBuffer->GetFrameDataGpuAddress(m_swapChain.GetCurrentFrameIndex()));
            m_state.CommandList->SetGraphicsRootConstantBufferView(2, m_renderObjectBuffer->GetFrameDataGpuAddress(m_swapChain.GetCurrentFrameIndex()));

            ID3D12DescriptorHeap* currHeap = m_textureManager.GetTextureHeap(packet.TextureSet);
            ID3D12DescriptorHeap* descHeap[] = { currHeap };
            m_state.CommandList->SetDescriptorHeaps(_countof(descHeap), descHeap);

            m_state.CommandList->SetGraphicsRootDescriptorTable(3, currHeap->GetGPUDescriptorHandleForHeapStart());

            m_state.CommandList->IASetVertexBuffers(0, 1, &m_vertexBuffer->GetVertexBufferView());
            m_state.CommandList->IASetIndexBuffer(&m_indexBuffer->GetIndexBufferView());
            m_state.CommandList->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

            m_state.CommandList->DrawIndexedInstanced(m_box->GetIndexCount(), 1, 0, 0, 0);

        }
        auto toPresent = CD3DX12_RESOURCE_BARRIER::Transition(currentRenderTarget->Resource.Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
        m_state.CommandList->ResourceBarrier(1, &toPresent);
    }

    m_state.CommandList->Close();
    ID3D12CommandList* cmdLists[] = { m_state.CommandList.Get() };
    m_state.CommandQueue->ExecuteCommandLists(_countof(cmdLists), cmdLists);
    m_swapChain.Present();

    m_state.FenceValues[m_swapChain.GetCurrentFrameIndex()] = ++m_state.CurrentFence;
    m_state.CommandQueue->Signal(m_state.Fence.Get(), m_state.CurrentFence);

    m_renderPasses[m_swapChain.GetCurrentFrameIndex()].clear();
    // [a_vorontsov] Check if we can move to next frame.
    m_swapChain.ProceedToNextFrame();
    for (uint32 i = 0; i < m_packetListPoolIndex; ++i)
        m_renderPacketListPool[i].clear();

    m_packetListPoolIndex = 0;
    m_passesRenderPackets.clear();
    if (m_state.FenceValues[m_swapChain.GetCurrentFrameIndex()] != 0 && m_state.Fence->GetCompletedValue() < m_state.FenceValues[m_swapChain.GetCurrentFrameIndex()])
    {
        HANDLE fenceEventHandle = CreateEvent(nullptr, FALSE, FALSE, nullptr);
        if (fenceEventHandle == nullptr)
        {
            ThrowIfFailed(HRESULT_FROM_WIN32(GetLastError()));
        }
        ThrowIfFailed(m_state.Fence->SetEventOnCompletion(m_state.FenceValues[m_swapChain.GetCurrentFrameIndex()], fenceEventHandle));

        WaitForSingleObjectEx(fenceEventHandle, INFINITE, false);
        CloseHandle(fenceEventHandle);
    }
}

void RendererDX12::LogAdapters()
{
    UINT i = 0;
    IDXGIAdapter* adapter = nullptr;
    std::vector<IDXGIAdapter*> adapterList;
    while (m_state.Factory->EnumAdapters(i, &adapter) != DXGI_ERROR_NOT_FOUND)
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
        LogOutputDisplayModes(output, m_swapChain.GetBackBufferFormat());
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


void RendererDX12::AddRenderPass(const RenderPass& renderPass)
{
    m_renderPasses[m_swapChain.GetCurrentFrameIndex()].push_back(renderPass);
}

ResourceDX12* RendererDX12::FindDxResource(uint32 handle)
{
    return nullptr;
}

void RendererDX12::RegisterTexture(Texture* texture)
{
    m_textureManager.RegisterTexture(texture);
}

void RendererDX12::RegisterShader(Shader* shader)
{
    m_shaderManager.RegisterShader(shader);
    m_rootSignatureManager.CreateRootSignature(m_state, shader->GetShaderData(), shader->GetHandle());
    m_vertexLayoutManager.GenerateVertexLayout(shader);
}

void RendererDX12::BuildMaterialForPass(const Material& mat, const RenderPass& pass)
{
    ID3D12PipelineState* ps = m_piplineStateManager.GetPipelineState(mat.GetHandle(), pass.GetHandle());
    if (ps == nullptr)
        m_piplineStateManager.BuildPipelineState(m_state, &mat, pass, m_rootSignatureManager, &m_textureManager, &m_shaderManager, &m_vertexLayoutManager, m_swapChain.GetBackBufferFormat(), m_swapChain.GetDepthStencilFormat());
}

void RendererDX12::AllocateRenderPacketList(RenderPassHandle handle)
{
    m_passesRenderPackets[handle] = &m_renderPacketListPool[m_packetListPoolIndex++];
}

void RendererDX12::AddRenderPacket(RenderPassHandle handle, RenderPacket packet)
{
    auto it = m_passesRenderPackets.find(handle);
    if (it == m_passesRenderPackets.cend())
        throw "No render packet list were allocated.";
    m_passesRenderPackets[handle]->push_back(packet);
}

void RendererDX12::RegisterMaterial(Material* material)
{
    material->SetHandle(GetNewHandle());
}

void RendererDX12::RegisterRenderPass(RenderPass* renderPass)
{
    renderPass->SetHandle(GetNewHandle());
}

void RendererDX12::RegisterTextureSet(TextureSet& set)
{
    set.SetHandle(GetNewHandle());
}

void RendererDX12::QueueTextureSetForUpdate(const TextureSet& set)
{
    m_textureManager.QueueTextureSetForUpdate(set);
}

}