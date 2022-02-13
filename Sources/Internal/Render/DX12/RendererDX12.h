#pragma once

#include <array>
#include <cstdio>
#include <dxgi1_6.h>
#include <exception>
#include <memory>

#include "Render/DX12/Buffers/ConstantBufferManagerDX12.h"
#include "Render/DX12/MeshManagerDX12.h"
#include "Render/DX12/PixProfiler.h"
#include "Render/DX12/PsoManager.h"
#include "Render/DX12/RootSignatureManager.h"
#include "Render/DX12/ShaderManagerDX12.h"
#include "Render/DX12/StateDX.h"
#include "Render/DX12/SwapChain.h"
#include "Render/DX12/VertexLayoutManagerDX12.h"
#include "Render/DX12/Texture/TextureManagerDX12.h"
#include "Render/RenderCommand.h"
#include "Render/RendererPublic.h"

namespace Kioto
{
class Mesh;
}

namespace Kioto::Renderer
{
class Texture;
class VertexLayout;
class UploadBufferDX12;

namespace ShaderParser
{
struct ShaderData;
}

class RendererDX12 final
{
public:
    RendererDX12();
    RendererDX12(const RendererDX12&) = delete;
    RendererDX12(RendererDX12&&) = delete;
    RendererDX12& operator= (const RendererDX12&) = delete;
    RendererDX12& operator= (const RendererDX12&&) = delete;
    ~RendererDX12() = default;

    void Init(uint16 width, uint16 height);
    void Resize(uint16 width, uint16 height);
    void ChangeFullScreenMode(bool fullScreen);
    void Shutdown();
    void Present();
    void Update(float32 dt);
    void StartFrame();

    void RegisterTexture(Texture* texture);
    void RegisterShader(Shader* shader);
    void RegisterMaterial(Material* material);
    void BuildMaterialForPass(Material& mat, const RenderPass* pass);
    void RegisterMesh(Mesh* mesh);

    void RegisterRenderPass(RenderPass* renderPass);
    void RegisterRenderObject(RenderObject& renderObject);

    void RegisterTextureSet(TextureSet& set);
    void QueueTextureSetForUpdate(const TextureSet& set);

    void RegisterConstantBuffer(ConstantBuffer& buffer);
    void QueueConstantBufferForUpdate(ConstantBuffer& buffer);

    void SubmitRenderCommands(const std::vector<RenderCommand>& commandList);

    TextureHandle GetCurrentBackBufferHandle() const;
    TextureHandle GetDepthStencilHandle() const;

    void SetTimeBuffer(ConstantBufferHandle handle);

private:
    void InitImGui();
    void RenderImGui();
    void ShutdownImGui();
    void GetHardwareAdapter(IDXGIFactory4* factory, IDXGIAdapter1** adapter);
    void WaitForGPU();
#ifdef _DEBUG
    void LogAdapters();
    void LogAdapterOutputs(IDXGIAdapter* adapter);
    void LogOutputDisplayModes(IDXGIOutput* output, DXGI_FORMAT format);
#endif
    void LoadPipeline();
    void ResourceTransition(StateDX& dxState, TextureHandle resourceHandle, eResourceState destState);

    std::vector<RenderCommand> mFrameCommands;
    GpuProfiler<PixProfiler> mProfiler;

    TextureManagerDX12 mTextureManager;
    StateDX mState;
    SwapChain mSwapChain;
    RootSignatureManager mRootSignatureManager;
    MeshManagerDX12 mMeshManager;
    ShaderManagerDX12 mShaderManager;
    PsoManager mPiplineStateManager;
    VertexLayoutManagerDX12 mVertexLayoutManager;
    ConstantBufferManagerDX12 mConstantBufferManager;

    bool mIsTearingSupported = false; // [a_vorontcov] TODO: Properly handle when tearing is not supported.
    UINT mWidth = -1;
    UINT mHeight = -1;
    bool mIsFullScreen = false;

    ID3D12DescriptorHeap* mImguiDescriptorHeap = nullptr;
};

inline TextureHandle RendererDX12::GetCurrentBackBufferHandle() const
{
    return mSwapChain.GetCurrentBackBufferHandle();
}

inline TextureHandle RendererDX12::GetDepthStencilHandle() const
{
    return mSwapChain.GetDepthStencilHandle();
}
}