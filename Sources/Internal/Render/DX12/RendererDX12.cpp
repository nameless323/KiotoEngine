#include "Render/DX12/RendererDX12.h"

#include <array>
#include <dxgidebug.h>
#include <string>
#include <vector>

#include "DXHelpers.h"
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

#ifdef _DEBUG
#include "Core/Logger/Logger.h"
#endif

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
    mFrameCommands.reserve(4096);

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
    ThrowIfFailed(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&mState.Factory)));

    BOOL allowTearing = FALSE;
    ComPtr<IDXGIFactory5> factory5;
    ThrowIfFailed(mState.Factory.As(&factory5));
    HRESULT hr = factory5->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING, &allowTearing, sizeof(allowTearing));
    mIsTearingSupported = SUCCEEDED(hr) && allowTearing;

    ComPtr<IDXGIAdapter1> hardwareAdapter;
    GetHardwareAdapter(mState.Factory.Get(), &hardwareAdapter);

    ThrowIfFailed(D3D12CreateDevice(hardwareAdapter.Get(), D3D_FEATURE_LEVEL_12_1, IID_PPV_ARGS(&mState.Device)));

    D3D12_FEATURE_DATA_D3D12_OPTIONS options;
    mState.Device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS, &options, sizeof(D3D12_FEATURE_DATA_D3D12_OPTIONS));

    D3D12_COMMAND_QUEUE_DESC queueDesc = {};
    queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

    ThrowIfFailed(mState.Device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&mState.CommandQueue)));
    NAME_D3D12_OBJECT(mState.CommandQueue);

    mSwapChain.Init(mState, mIsTearingSupported, width, height);
    for (uint8 i = 0; i < StateDX::FrameCount; ++i)
        mTextureManager.RegisterTextureWithoutOwnership(mSwapChain.GetBackBuffer(i));
    mTextureManager.RegisterTextureWithoutOwnership(mSwapChain.GetDepthStencil());

    for (uint32 i = 0; i < StateDX::FrameCount; ++i)
    {
        ThrowIfFailed(mState.Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&mState.CommandAllocators[i])));
        NAME_D3D12_OBJECT(mState.CommandAllocators[i]);
    }
    ThrowIfFailed(mState.Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, mState.CommandAllocators[0].Get(), nullptr, IID_PPV_ARGS(&mState.CommandList))); // [a_vorontcov] Maybe not 0 as mask?
    NAME_D3D12_OBJECT(mState.CommandList);
    mState.CommandList->Close();

    ThrowIfFailed(mState.Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&mState.Fence)));
    NAME_D3D12_OBJECT(mState.Fence);

    mState.CbvSrvUavDescriptorSize = mState.Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    mState.RtvDescriptorSize = mState.Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    mState.DsvDescriptorSize = mState.Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
    mState.SamplerDescriptorSize = mState.Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);

    LoadPipeline();
    Resize(width, height);

    InitImGui();

    mTextureManager.InitRtvHeap(mState);
    mTextureManager.InitDsvHeap(mState);

#if defined(_DEBUG) && defined(LOG_ADAPTERS)
    LogAdapters();

    DXGI_ADAPTER_DESC adapterDesc;
    hardwareAdapter->GetDesc(&adapterDesc);

    std::wstring text = L"*****\n!!SELECTED ADAPTER: ";
    text += adapterDesc.Description;
    text += L"\n";
    text += L"****\n";
    LOG(WstrToStr(text));
#endif
}

void RendererDX12::LoadPipeline()
{
    WaitForGPU();
}

void RendererDX12::ResourceTransition(StateDX& dxState, TextureHandle resourceHandle, eResourceState destState)
{
    TextureDX12* tex = mTextureManager.FindTexture(resourceHandle);
    D3D12_RESOURCE_STATES dxSrcState = tex->GetCurrentState();
    D3D12_RESOURCE_STATES dxDstState = KiotoDx12Mapping::ResourceStates[destState];
    if (dxSrcState == dxDstState)
        return;

    auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(tex->Resource.Get(), dxSrcState, dxDstState);
    mState.CommandList->ResourceBarrier(1, &barrier);
    tex->SetCurrentState(dxDstState);

#if defined(_DEBUG) && defined(DUMP_RESOURCE_TRANSITIONS)
    LOG("Resource name: ", tex->GetDebugName(), " State before: ", dxSrcState, " State after: ", dxDstState);
#endif

}

void RendererDX12::Shutdown()
{
    if (mState.Device != nullptr)
        WaitForGPU();

    if (!mIsTearingSupported)
        ThrowIfFailed(mSwapChain.SetFullscreenState(false, nullptr));

    ShutdownImGui();
#ifdef _DEBUG
    ComPtr<IDXGIDebug1> dxgiDebug;
    if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&dxgiDebug))))
        dxgiDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_FLAGS(DXGI_DEBUG_RLO_SUMMARY | DXGI_DEBUG_RLO_IGNORE_INTERNAL));
    LOG("************************Shutdown\n");
#endif
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
        mState.Device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&mImguiDescriptorHeap));
    }

    ImGui::ImplDX12Init(mState.Device.Get(), StateDX::FrameCount, mSwapChain.GetBackBufferFormat(), mImguiDescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
        mImguiDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
}

void RendererDX12::RenderImGui()
{
    mProfiler.BeginGpuEvent(mState.CommandList.Get(), "ImGUI");

    auto chainBackBufferCPUHandle = mSwapChain.GetCurrentBackBufferCPUHandle(mState);
    auto chainDepthStencilHandle = mSwapChain.GetDepthStencilCPUHandle();
    mState.CommandList->OMSetRenderTargets(1, &chainBackBufferCPUHandle, false, &chainDepthStencilHandle);
    mState.CommandList->SetDescriptorHeaps(1, &mImguiDescriptorHeap);
    ImGui::Render();
    ImGui::ImplDX12RenderDrawData(ImGui::GetDrawData(), mState.CommandList.Get());

    mProfiler.EndGpuEvent(mState.CommandList.Get());
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
    ThrowIfFailed(mState.CommandQueue->Signal(mState.Fence.Get(), mState.FenceValues[mSwapChain.GetCurrentFrameIndex()]));

    if (mState.Fence->GetCompletedValue() < mState.FenceValues[mSwapChain.GetCurrentFrameIndex()])
    {
        HANDLE fenceEventHandle = CreateEvent(nullptr, FALSE, FALSE, nullptr);
        if (fenceEventHandle == nullptr)
        {
            ThrowIfFailed(HRESULT_FROM_WIN32(GetLastError()));
        }
        ThrowIfFailed(mState.Fence->SetEventOnCompletion(mState.FenceValues[mSwapChain.GetCurrentFrameIndex()], fenceEventHandle));

        WaitForSingleObjectEx(fenceEventHandle, INFINITE, false);
        CloseHandle(fenceEventHandle);
    }
}

void RendererDX12::Resize(uint16 width, uint16 height)
{
    if (mWidth == width && mHeight == height)
        return;

    ImGui::ImplDX12InvalidateDeviceObjects();

    mWidth = width;
    mHeight = height;

    WaitForGPU();

    for (auto& fenceVal : mState.FenceValues)
        fenceVal = mState.CurrentFence;

    ThrowIfFailed(mState.CommandList->Reset(mState.CommandAllocators[mSwapChain.GetCurrentFrameIndex()].Get(), nullptr));

    mSwapChain.Resize(mState, width, height);

    ThrowIfFailed(mState.CommandList->Close());
    ID3D12CommandList* cmdLists[] = { mState.CommandList.Get() };
    mState.CommandQueue->ExecuteCommandLists(_countof(cmdLists), cmdLists);

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
    mState.CommandAllocators[mSwapChain.GetCurrentFrameIndex()]->Reset();
    mState.CommandList->Reset(mState.CommandAllocators[mSwapChain.GetCurrentFrameIndex()].Get(), nullptr);

    mTextureManager.ProcessRegistationQueue(mState);
    mTextureManager.ProcessTextureSetUpdates(mState);

    mMeshManager.ProcessRegistrationQueue(mState);

    mConstantBufferManager.ProcessRegistrationQueue(mState);
    mConstantBufferManager.ProcessBufferUpdates(mSwapChain.GetCurrentFrameIndex());

    auto toRt = CD3DX12_RESOURCE_BARRIER::Transition(mSwapChain.GetCurrentBackBuffer()->Resource.Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
    mState.CommandList->ResourceBarrier(1, &toRt);

    for (auto& cmd : mFrameCommands)
    {
        assert(cmd.CommandType != eRenderCommandType::eInvalidCommand);
        if (cmd.CommandType == eRenderCommandType::eSetRenderTargets)
        {
            const SetRenderTargetsCommand& srtCommand = std::get<SetRenderTargetsCommand>(cmd.Command);
            UINT renderTargetCount = srtCommand.RenderTargetCount;
            D3D12_CPU_DESCRIPTOR_HANDLE rtHandle;
            D3D12_CPU_DESCRIPTOR_HANDLE dsHandle;
            D3D12_CPU_DESCRIPTOR_HANDLE* rtHandlePtr = nullptr;
            D3D12_CPU_DESCRIPTOR_HANDLE* dsHandlePtr = nullptr;

            TextureHandle currentRTHandle = srtCommand.GetRenderTarget(0);
            if (currentRTHandle != InvalidHandle)
            {
                if (currentRTHandle == DefaultBackBufferHandle)
                    rtHandle = mSwapChain.GetCurrentBackBufferCPUHandle(mState);
                else
                    rtHandle = mTextureManager.GetRtvHandle(currentRTHandle);
                rtHandlePtr = &rtHandle;
            }

            TextureHandle currentDSHandle = srtCommand.GetDepthStencil();
            if (currentDSHandle != InvalidHandle)
            {
                if (currentDSHandle == DefaultDepthStencilHandle)
                    dsHandle = mSwapChain.GetDepthStencilCPUHandle();
                else
                    dsHandle = mTextureManager.GetDsvHandle(currentDSHandle);
                dsHandlePtr = &dsHandle;
            }

            auto scissorRect = DXRectFromKioto(srtCommand.Scissor);
            mState.CommandList->RSSetScissorRects(1, &scissorRect);
            auto viewport = DXViewportFromKioto(srtCommand.Viewport);
            mState.CommandList->RSSetViewports(1, &viewport);

            if (srtCommand.ClearColor && currentRTHandle != InvalidHandle)
                mState.CommandList->ClearRenderTargetView(rtHandle, srtCommand.ClearColorValue.data, 0, nullptr);
            if (srtCommand.ClearDepth && currentDSHandle != InvalidHandle)
                mState.CommandList->ClearDepthStencilView(dsHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

            mState.CommandList->OMSetRenderTargets(renderTargetCount, rtHandlePtr, false, dsHandlePtr);
        }
        else if (cmd.CommandType == eRenderCommandType::eEndRenderPass)
        {
        }
        else if (cmd.CommandType == eRenderCommandType::eResourceTransitonCommand)
        {
            const ResourceTransitonCommand& transitionCommand = std::get<ResourceTransitonCommand>(cmd.Command);
            ResourceTransition(mState, transitionCommand.ResourceHandle, transitionCommand.DestState);
        }
        else if (cmd.CommandType == eRenderCommandType::eSubmitRenderPacket)
        {
            const RenderPacket& packet = std::get<SubmitRenderPacketCommand>(cmd.Command).Packet;

            ID3D12PipelineState* pipelineState = mPiplineStateManager.GetPipelineState(packet.Material.GetHandle(), packet.Pass);
            mState.CommandList->SetPipelineState(pipelineState);

            ID3D12RootSignature* rootSig = mRootSignatureManager.GetRootSignature(packet.Shader);
            mState.CommandList->SetGraphicsRootSignature(rootSig);

            UINT currFrameInd = mSwapChain.GetCurrentFrameIndex();
            UINT buffersCount = static_cast<UINT>(packet.ConstantBufferHandles.size());
            for (uint32 i = 0; i < buffersCount; ++i)
            {
                UploadBufferDX12* buffer = mConstantBufferManager.FindBuffer(packet.ConstantBufferHandles[i]);
                if (!buffer->HasDescriptorHeap())
                {
                    mState.CommandList->SetGraphicsRootConstantBufferView(static_cast<UINT>(i), buffer->GetFrameDataGpuAddress(currFrameInd));
                }
                else
                {
                    ID3D12DescriptorHeap* heaps[] = { buffer->GetDescriptorHeap() };
                    mState.CommandList->SetDescriptorHeaps(_countof(heaps), heaps);

                    mState.CommandList->SetGraphicsRootDescriptorTable(i, buffer->GetGpuDescriptorHandleForFrame(currFrameInd));
                }
            }
            UINT constantsCount = static_cast<UINT>(packet.UniformConstants.size());
            for (uint32 i = 0; i < constantsCount; ++i)
                mState.CommandList->SetGraphicsRoot32BitConstant(buffersCount + i, packet.UniformConstants[i], 0);

            ID3D12DescriptorHeap* currTexDescriptorHeap = mTextureManager.GetTextureHeap(packet.TextureSet);
            if (currTexDescriptorHeap != nullptr) // [a_vorontcov] TODO: No difference if one messed up with texset or if there is no textures for the draw. Not good at all. Rethink.
            {
                ID3D12DescriptorHeap* descHeap[] = { currTexDescriptorHeap };
                mState.CommandList->SetDescriptorHeaps(_countof(descHeap), descHeap);

                mState.CommandList->SetGraphicsRootDescriptorTable(buffersCount + constantsCount, currTexDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
            }

            MeshDX12* currGeometry = mMeshManager.Find(packet.Mesh);

            mState.CommandList->IASetVertexBuffers(0, 1, &currGeometry->GetVertexBufferView());
            mState.CommandList->IASetIndexBuffer(&currGeometry->GetIndexBufferView());
            mState.CommandList->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

            mState.CommandList->DrawIndexedInstanced(currGeometry->GetIndexCount(), 1, 0, 0, 0);
        }
        else if (cmd.CommandType == eRenderCommandType::eBeginGpuEvent)
        {
            const BeginGpuEventCommand& evCommand = std::get<BeginGpuEventCommand>(cmd.Command);
            mProfiler.BeginGpuEvent(mState.CommandList.Get(), evCommand.Name.c_str());
        }
        else if (cmd.CommandType == eRenderCommandType::eEndGpuEvent)
        {
            mProfiler.EndGpuEvent(mState.CommandList.Get());
        }
        else if (cmd.CommandType == eRenderCommandType::eSetGpuMarker)
        {
            const SetGpuMarkerCommand& smCommand = std::get<SetGpuMarkerCommand>(cmd.Command);
            mProfiler.SetMarker(mState.CommandList.Get(), smCommand.Name.c_str());
        }
        else
        {
            assert(false);
        }

    }

    RenderImGui();

    auto toPresent = CD3DX12_RESOURCE_BARRIER::Transition(mSwapChain.GetCurrentBackBuffer()->Resource.Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT); // [a_vorontcov] WRONG WRONG WRONG, but ok for now.
    mState.CommandList->ResourceBarrier(1, &toPresent);

    mState.CommandList->Close();
    ID3D12CommandList* cmdLists[] = { mState.CommandList.Get() };
    mState.CommandQueue->ExecuteCommandLists(_countof(cmdLists), cmdLists);
    mSwapChain.Present();

    mState.FenceValues[mSwapChain.GetCurrentFrameIndex()] = ++mState.CurrentFence;
    mState.CommandQueue->Signal(mState.Fence.Get(), mState.CurrentFence);

    mFrameCommands.clear();

    // [a_vorontcov] Check if we can move to next frame.
    mSwapChain.ProceedToNextFrame();

    if (mState.FenceValues[mSwapChain.GetCurrentFrameIndex()] != 0 && mState.Fence->GetCompletedValue() < mState.FenceValues[mSwapChain.GetCurrentFrameIndex()])
    {
        HANDLE fenceEventHandle = CreateEvent(nullptr, FALSE, FALSE, nullptr);
        if (fenceEventHandle == nullptr)
        {
            ThrowIfFailed(HRESULT_FROM_WIN32(GetLastError()));
        }
        ThrowIfFailed(mState.Fence->SetEventOnCompletion(mState.FenceValues[mSwapChain.GetCurrentFrameIndex()], fenceEventHandle));

        WaitForSingleObjectEx(fenceEventHandle, INFINITE, false);
        CloseHandle(fenceEventHandle);
    }
}

#ifdef _DEBUG
void RendererDX12::LogAdapters()
{
    UINT i = 0;
    IDXGIAdapter* adapter = nullptr;
    std::vector<IDXGIAdapter*> adapterList;
    while (mState.Factory->EnumAdapters(i, &adapter) != DXGI_ERROR_NOT_FOUND)
    {
        DXGI_ADAPTER_DESC adapterDesc;
        adapter->GetDesc(&adapterDesc);

        std::wstring text = L"--ADAPTER: ";
        text += adapterDesc.Description;
        text += L"\n";
        LOG(WstrToStr(text));
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
        LogOutputDisplayModes(output, mSwapChain.GetBackBufferFormat());
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
        std::string text =
            "Width = " + std::to_string(x.Width) + "  " +
            "Height = " + std::to_string(x.Height) + "  " +
            "Refresh = " + std::to_string(n) + "/" + std::to_string(d) +
            "\n";

        LOG(text);
    }
}
#endif

void RendererDX12::ChangeFullScreenMode(bool fullScreen)
{
    if (fullScreen == mIsFullScreen)
        return;
    mIsFullScreen = fullScreen;
    Resize(mWidth, mHeight);
}

void RendererDX12::RegisterTexture(Texture* texture)
{
    mTextureManager.RegisterTexture(texture);
}

void RendererDX12::RegisterShader(Shader* shader)
{
    mShaderManager.RegisterShader(shader);
    mRootSignatureManager.CreateRootSignature(mState, shader->GetShaderData(), shader->GetBufferLayoutTemplate(), shader->GetRenderObjectConstants(), shader->GetHandle());
    mVertexLayoutManager.GenerateVertexLayout(shader);
}

void RendererDX12::BuildMaterialForPass(Material& mat, const RenderPass* pass)
{
    ID3D12PipelineState* ps = mPiplineStateManager.GetPipelineState(mat.GetHandle(), pass->GetHandle());
    if (ps == nullptr)
        mPiplineStateManager.BuildPipelineState(mState, &mat, pass, mRootSignatureManager, &mTextureManager, &mShaderManager, &mVertexLayoutManager, mSwapChain.GetBackBufferFormat(), mSwapChain.GetDepthStencilFormat());
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
    mTextureManager.QueueTextureSetForUpdate(set);
}

void RendererDX12::RegisterMesh(Mesh* mesh)
{
    mMeshManager.RegisterMesh(mesh);
}

void RendererDX12::SubmitRenderCommands(const std::vector<RenderCommand>& commandList) // [a_vorontcov] Not const and splice would be better. https://stackoverflow.com/questions/1449703/how-to-append-a-listt-object-to-another
{
    mFrameCommands.insert(mFrameCommands.end(), commandList.begin(), commandList.end());
}

void RendererDX12::QueueConstantBufferForUpdate(ConstantBuffer& buffer)
{
    mConstantBufferManager.QueueConstantBufferForUpdate(buffer);
}

void RendererDX12::RegisterConstantBuffer(ConstantBuffer& buffer)
{
    mConstantBufferManager.RegisterConstantBuffer(&buffer);
}

void RendererDX12::SetTimeBuffer(ConstantBufferHandle handle)
{
}

void RendererDX12::RegisterRenderObject(RenderObject& renderObject)
{
    mConstantBufferManager.RegisterRenderObject(renderObject);
}

}