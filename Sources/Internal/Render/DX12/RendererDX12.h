#pragma once

#include <array>
#include <cstdio>
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

    std::vector<RenderCommand> m_frameCommands;
    GpuProfiler<PixProfiler> m_profiler;

    TextureManagerDX12 m_textureManager;
    StateDX m_state;
    SwapChain m_swapChain;
    RootSignatureManager m_rootSignatureManager;
    MeshManagerDX12 m_meshManager;
    ShaderManagerDX12 m_shaderManager;
    PsoManager m_piplineStateManager;
    VertexLayoutManagerDX12 m_vertexLayoutManager;
    ConstantBufferManagerDX12 m_constantBufferManager;

    bool m_isTearingSupported = false; // [a_vorontcov] TODO: Properly handle when tearing is not supported.
    UINT m_width = -1;
    UINT m_height = -1;
    bool m_isFullScreen = false;

    ID3D12DescriptorHeap* m_imguiDescriptorHeap = nullptr;
};

inline TextureHandle RendererDX12::GetCurrentBackBufferHandle() const
{
    return m_swapChain.GetCurrentBackBufferHandle();
}

inline TextureHandle RendererDX12::GetDepthStencilHandle() const
{
    return m_swapChain.GetDepthStencilHandle();
}
}