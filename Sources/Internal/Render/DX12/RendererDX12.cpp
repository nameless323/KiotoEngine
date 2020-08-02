#include "stdafx.h"

#include "Render/DX12/RendererDX12.h"

#include <array>
#include <string>
#include <vector>

#include "Sources/External/Dx12Helpers/DDSTextureLoader.h"
#include "Sources/External/IMGUI/imgui.h"
#include "Sources/External/IMGUI/imgui_impl_dx12.h"
#include "Sources/External/IMGUI/imgui_impl_win32.h"

#include "Core/WindowsApplication.h"
#include "Render/Buffers/EngineBuffers.h"
#include "Render/DX12/Geometry/MeshDX12.h"
#include "Render/DX12/KiotoDx12Mapping.h"
#include "Render/Shader.h"
#include "Render/Material.h"
#include "Render/RenderPass/RenderPass.h"

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
    m_frameCommands.reserve(4096);

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

    D3D12_FEATURE_DATA_D3D12_OPTIONS options;
    m_state.Device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS, &options, sizeof(D3D12_FEATURE_DATA_D3D12_OPTIONS));

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
    ThrowIfFailed(m_state.Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_state.CommandAllocators[0].Get(), nullptr, IID_PPV_ARGS(&m_state.CommandList))); // [a_vorontcov] Maybe not 0 as mask?
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

    InitImGui();

    m_textureManager.InitRtvHeap(m_state);

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
    WaitForGPU();
}

void RendererDX12::ResourceTransition(StateDX& dxState, TextureHandle resourceHandle, eResourceState destState)
{
    TextureDX12* tex = m_textureManager.FindTexture(resourceHandle);
    D3D12_RESOURCE_STATES dxSrcState = tex->GetCurrentState();
    D3D12_RESOURCE_STATES dxDstState = KiotoDx12Mapping::ResourceStates[destState];
    auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(tex->Resource.Get(), dxSrcState, dxDstState);
    m_state.CommandList->ResourceBarrier(1, &barrier);
    tex->SetCurrentState(dxDstState);
}

void RendererDX12::Shutdown()
{
    if (m_state.Device != nullptr)
        WaitForGPU();

    if (!m_isTearingSupported)
        ThrowIfFailed(m_swapChain.SetFullscreenState(false, nullptr));

    ShutdownImGui();
#ifdef _DEBUG
    ComPtr<IDXGIDebug1> dxgiDebug;
    if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&dxgiDebug))))
        dxgiDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_FLAGS(DXGI_DEBUG_RLO_SUMMARY | DXGI_DEBUG_RLO_IGNORE_INTERNAL));
#endif
    OutputDebugStringA("************************Shutdown\n");
}

void RendererDX12::InitImGui()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui::ImplWinInit(WindowsApplication::GetHWND());
    {
        D3D12_DESCRIPTOR_HEAP_DESC desc = {};
        desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        desc.NumDescriptors = 1;
        desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
        m_state.Device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&m_imguiDescriptorHeap));
    }

    ImGui::ImplDX12Init(m_state.Device.Get(), StateDX::FrameCount, m_swapChain.GetBackBufferFormat(), m_imguiDescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
        m_imguiDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
}

void RendererDX12::RenderImGui()
{
    m_profiler.BeginGpuEvent(m_state.CommandList.Get(), "ImGUI");

    m_state.CommandList->OMSetRenderTargets(1, &m_swapChain.GetCurrentBackBufferCPUHandle(m_state), false, &m_swapChain.GetDepthStencilCPUHandle());
    m_state.CommandList->SetDescriptorHeaps(1, &m_imguiDescriptorHeap);
    ImGui::Render();
    ImGui::ImplDX12RenderDrawData(ImGui::GetDrawData(), m_state.CommandList.Get());

    m_profiler.EndGpuEvent(m_state.CommandList.Get());
}

void RendererDX12::ShutdownImGui()
{
    ImGui::ImplDX12Shutdown();
    ImGui::ImplWinShutdown();
    ImGui::DestroyContext();
}

void RendererDX12::GetHardwareAdapter(IDXGIFactory4* factory, IDXGIAdapter1** adapter)
{
    *adapter = nullptr;
    ComPtr<IDXGIAdapter1> currAdapter;
    UINT deviceIndex = -1;
    UINT bestVendorIndex = -1;

    static const std::array<UINT, 3> preferredVendors = // [a_vorontcov] http://pcidatabase.com/vendors.php?sort=id
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

    ImGui::ImplDX12InvalidateDeviceObjects();

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

    ImGui::ImplDX12CreateDeviceObjects();

    WaitForGPU();
}

void RendererDX12::Update(float32 dt)
{
}

void RendererDX12::StartFrame()
{
    ImGui::ImplDX12NewFrame();
    ImGui::ImplWinNewFrame();
    ImGui::NewFrame();
}

void RendererDX12::Present()
{
    m_state.CommandAllocators[m_swapChain.GetCurrentFrameIndex()]->Reset();
    m_state.CommandList->Reset(m_state.CommandAllocators[m_swapChain.GetCurrentFrameIndex()].Get(), nullptr);

    m_textureManager.ProcessRegistationQueue(m_state);
    m_textureManager.ProcessTextureSetUpdates(m_state);

    m_meshManager.ProcessRegistrationQueue(m_state);

    m_constantBufferManager.ProcessRegistrationQueue(m_state);
    m_constantBufferManager.ProcessBufferUpdates(m_swapChain.GetCurrentFrameIndex());

    auto toRt = CD3DX12_RESOURCE_BARRIER::Transition(m_swapChain.GetCurrentBackBuffer()->Resource.Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
    m_state.CommandList->ResourceBarrier(1, &toRt);

    for (auto& cmd : m_frameCommands)
    {
        assert(cmd.CommandType != eRenderCommandType::eInvalidCommand);
        if (cmd.CommandType == eRenderCommandType::eSetRenderTargets)
        {
            const SetRenderTargetsCommand& srtCommand = std::get<SetRenderTargetsCommand>(cmd.Command);
            D3D12_CPU_DESCRIPTOR_HANDLE rtHandle;
            D3D12_CPU_DESCRIPTOR_HANDLE dsHandle;
            if (srtCommand.GetRenderTarget(0) == DefaultBackBufferHandle)
                rtHandle = m_swapChain.GetCurrentBackBufferCPUHandle(m_state);
            else
                rtHandle = m_textureManager.GetRtvHandle(srtCommand.GetRenderTarget(0));

            if (srtCommand.GetDepthStencil() == DefaultDepthStencilHandle)
                dsHandle = m_swapChain.GetDepthStencilCPUHandle();
            else
            {
                assert(false);
                // [a_vorontcov] TODO: ToBeImplemented dsHandle = m_textureManager.GetDsvHandle(srtCommand.GetDepthStencil());
            }

            m_state.CommandList->RSSetScissorRects(1, &DXRectFromKioto(srtCommand.Scissor));
            m_state.CommandList->RSSetViewports(1, &DXViewportFromKioto(srtCommand.Viewport));

            if (srtCommand.ClearColor)
                m_state.CommandList->ClearRenderTargetView(rtHandle, srtCommand.ClearColorValue.data, 0, nullptr);
            if (srtCommand.ClearDepth)
                m_state.CommandList->ClearDepthStencilView(dsHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

            m_state.CommandList->OMSetRenderTargets(1, &rtHandle, false, &dsHandle);
        }
        else if (cmd.CommandType == eRenderCommandType::eEndRenderPass)
        {
        }
        else if (cmd.CommandType == eRenderCommandType::eResourceTransitonCommand)
        {
            const ResourceTransitonCommand& transitionCommand = std::get<ResourceTransitonCommand>(cmd.Command);
            ResourceTransition(m_state, transitionCommand.ResourceHandle, transitionCommand.DestState);
        }
        else if (cmd.CommandType == eRenderCommandType::eSubmitRenderPacket)
        {
            const RenderPacket& packet = std::get<SubmitRenderPacketCommand>(cmd.Command).Packet;

            ID3D12PipelineState* pipelineState = m_piplineStateManager.GetPipelineState(packet.Material.GetHandle(), packet.Pass);
            m_state.CommandList->SetPipelineState(pipelineState);

            ID3D12RootSignature* rootSig = m_rootSignatureManager.GetRootSignature(packet.Shader);
            m_state.CommandList->SetGraphicsRootSignature(rootSig);

            UINT currFrameInd = m_swapChain.GetCurrentFrameIndex();
            UINT buffersCount = static_cast<UINT>(packet.ConstantBufferHandles.size());
            for (uint32 i = 0; i < buffersCount; ++i)
            {
                UploadBufferDX12* buffer = m_constantBufferManager.FindBuffer(packet.ConstantBufferHandles[i]);
                if (!buffer->HasDescriptorHeap())
                {
                    m_state.CommandList->SetGraphicsRootConstantBufferView(static_cast<UINT>(i), buffer->GetFrameDataGpuAddress(currFrameInd));
                }
                else
                {
                    ID3D12DescriptorHeap* heaps[] = { buffer->GetDescriptorHeap() };
                    m_state.CommandList->SetDescriptorHeaps(_countof(heaps), heaps);

                    m_state.CommandList->SetGraphicsRootDescriptorTable(i, buffer->GetGpuDescriptorHandleForFrame(currFrameInd));
                }
            }
            UINT constantsCount = static_cast<UINT>(packet.UniformConstants.size());
            for (uint32 i = 0; i < constantsCount; ++i)
                m_state.CommandList->SetGraphicsRoot32BitConstant(buffersCount + i, packet.UniformConstants[i], 0);

            ID3D12DescriptorHeap* currTexDescriptorHeap = m_textureManager.GetTextureHeap(packet.TextureSet);
            if (currTexDescriptorHeap != nullptr) // [a_vorontcov] TODO: No difference if one messed up with texset or if there is no textures for the draw. Not good at all. Rethink.
            {
                ID3D12DescriptorHeap* descHeap[] = { currTexDescriptorHeap };
                m_state.CommandList->SetDescriptorHeaps(_countof(descHeap), descHeap);

                m_state.CommandList->SetGraphicsRootDescriptorTable(buffersCount + constantsCount, currTexDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
            }

            MeshDX12* currGeometry = m_meshManager.Find(packet.Mesh);

            m_state.CommandList->IASetVertexBuffers(0, 1, &currGeometry->GetVertexBufferView());
            m_state.CommandList->IASetIndexBuffer(&currGeometry->GetIndexBufferView());
            m_state.CommandList->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

            m_state.CommandList->DrawIndexedInstanced(currGeometry->GetIndexCount(), 1, 0, 0, 0);
        }
        else if (cmd.CommandType == eRenderCommandType::eBeginGpuEvent)
        {
            const BeginGpuEventCommand& evCommand = std::get<BeginGpuEventCommand>(cmd.Command);
            m_profiler.BeginGpuEvent(m_state.CommandList.Get(), evCommand.Name.c_str());
        }
        else if (cmd.CommandType == eRenderCommandType::eEndGpuEvent)
        {
            m_profiler.EndGpuEvent(m_state.CommandList.Get());
        }
        else if (cmd.CommandType == eRenderCommandType::eSetGpuMarker)
        {
            const SetGpuMarkerCommand& smCommand = std::get<SetGpuMarkerCommand>(cmd.Command);
            m_profiler.SetMarker(m_state.CommandList.Get(), smCommand.Name.c_str());
        }
        else
        {
            assert(false);
        }

    }

    RenderImGui();

    auto toPresent = CD3DX12_RESOURCE_BARRIER::Transition(m_swapChain.GetCurrentBackBuffer()->Resource.Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT); // [a_vorontcov] WRONG WRONG WRONG, but ok for now.
    m_state.CommandList->ResourceBarrier(1, &toPresent);

    m_state.CommandList->Close();
    ID3D12CommandList* cmdLists[] = { m_state.CommandList.Get() };
    m_state.CommandQueue->ExecuteCommandLists(_countof(cmdLists), cmdLists);
    m_swapChain.Present();

    m_state.FenceValues[m_swapChain.GetCurrentFrameIndex()] = ++m_state.CurrentFence;
    m_state.CommandQueue->Signal(m_state.Fence.Get(), m_state.CurrentFence);

    m_frameCommands.clear();

    // [a_vorontcov] Check if we can move to next frame.
    m_swapChain.ProceedToNextFrame();

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

void RendererDX12::RegisterTexture(Texture* texture)
{
    m_textureManager.RegisterTexture(texture);
}

void RendererDX12::RegisterShader(Shader* shader)
{
    m_shaderManager.RegisterShader(shader);
    m_rootSignatureManager.CreateRootSignature(m_state, shader->GetShaderData(), shader->GetBufferLayoutTemplate(), shader->GetRenderObjectConstants(), shader->GetHandle());
    m_vertexLayoutManager.GenerateVertexLayout(shader);
}

void RendererDX12::BuildMaterialForPass(Material& mat, const RenderPass* pass)
{
    ID3D12PipelineState* ps = m_piplineStateManager.GetPipelineState(mat.GetHandle(), pass->GetHandle());
    if (ps == nullptr)
        m_piplineStateManager.BuildPipelineState(m_state, &mat, pass, m_rootSignatureManager, &m_textureManager, &m_shaderManager, &m_vertexLayoutManager, m_swapChain.GetBackBufferFormat(), m_swapChain.GetDepthStencilFormat());
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

void RendererDX12::RegisterMesh(Mesh* mesh)
{
    m_meshManager.RegisterMesh(mesh);
}

void RendererDX12::SubmitRenderCommands(const std::vector<RenderCommand>& commandList) // [a_vorontcov] Not const and splice would be better. https://stackoverflow.com/questions/1449703/how-to-append-a-listt-object-to-another
{
    m_frameCommands.insert(m_frameCommands.end(), commandList.begin(), commandList.end());
}

void RendererDX12::QueueConstantBufferForUpdate(ConstantBuffer& buffer)
{
    m_constantBufferManager.QueueConstantBufferForUpdate(buffer);
}

void RendererDX12::RegisterConstantBuffer(ConstantBuffer& buffer)
{
    m_constantBufferManager.RegisterConstantBuffer(&buffer);
}

void RendererDX12::SetTimeBuffer(ConstantBufferHandle handle)
{
}

void RendererDX12::RegisterRenderObject(RenderObject& renderObject)
{
    m_constantBufferManager.RegisterRenderObject(renderObject);
}

}