//
// Copyright (C) Alexandr Vorontsov. 2017
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#pragma once

#include <array>
#include <cstdio>
#include <exception>
#include <memory>

// [a_vorontsov] To fwd decl.
#include "Render/DX12/Buffers/EngineBuffers.h"
#include "Render/DX12/Buffers/UploadBuffer.h"
#include "Render/DX12/Buffers/VertexBufferDX12.h"
#include "Render/DX12/Buffers/IndexBufferDX12.h"
#include "Render/DX12/Buffers/ResourceDX12.h"
#include "Render/DX12/VertexLayoutDX12.h"
#include "Render/DX12/ShaderDX12.h"
#include "Render/DX12/ShaderManagerDX12.h"
#include "Render/DX12/MeshManagerDX12.h"
#include "Render/Texture/TextureDX12.h"
#include "Render/RendererPublic.h"
#include "Render/RenderPass/RenderPass.h"
#include "Render/Texture/TextureSet.h"
#include "Render/Texture/TextureManagerDX12.h"
#include "Render/DX12/StateDX.h"
#include "Render/DX12/SwapChain.h"
#include "Render/DX12/RootSignatureManager.h"
#include "Render/DX12/PsoManager.h"
#include "Render/DX12/VertexLayoutManagerDX12.h"
#include "Render/DX12/RenderPacket.h"
#include "Render/DX12/Buffers/ConstantBufferManagerDX12.h"

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

    void AddRenderPass(const RenderPass& renderPass);

    void RegisterTexture(Texture* texture);
    void RegisterShader(Shader* shader);
    void RegisterMaterial(Material* material);
    void BuildMaterialForPass(Material& mat, const RenderPass& pass);
    void RegisterMesh(Mesh* mesh);

    void RegisterRenderPass(RenderPass* renderPass);
    void RegisterTextureSet(TextureSet& set);

    void QueueTextureSetForUpdate(const TextureSet& set);

    void AllocateRenderPacketList(RenderPassHandle handle);
    void AddRenderPacket(RenderPassHandle handle, RenderPacket packet);

    TextureHandle GetCurrentBackBufferHandle() const;
    TextureHandle GetDepthStencilHandle() const;

private:
    static constexpr uint32 PacketListPoolSize = 64;
    static constexpr uint32 PacketListSize = 4096;

    TextureManagerDX12 m_textureManager;
    StateDX m_state;
    SwapChain m_swapChain;
    RootSignatureManager m_rootSignatureManager;
    MeshManagerDX12 m_meshManager;
    ShaderManagerDX12 m_shaderManager;
    PsoManager m_piplineStateManager;
    VertexLayoutManagerDX12 m_vertexLayoutManager;
    ConstantBufferManagerDX12 m_constantBufferManager;

    std::unordered_map<uint32, ResourceDX12> m_resources;
    std::array<std::vector<RenderPass>, StateDX::FrameCount> m_renderPasses;

    ResourceDX12* FindDxResource(uint32 handle);
    
    void GetHardwareAdapter(IDXGIFactory4* factory, IDXGIAdapter1** adapter);
    void WaitForGPU();
    void LogAdapters();
    void LogAdapterOutputs(IDXGIAdapter* adapter);
    void LogOutputDisplayModes(IDXGIOutput* output, DXGI_FORMAT format);

    void LoadPipeline();
    void UpdateTimeCB();
    void UpdateRenderObjectCB();
    void UpdatePassCB();

    bool m_isTearingSupported = false; // [a_vorontsov] TODO: Properly handle when tearing is not supported.
    UINT m_width = -1;
    UINT m_height = -1;
    bool m_isFullScreen = false;

    EngineBuffers engineBuffers;
    UploadBufferDX12* m_timeBuffer = nullptr;
    UploadBufferDX12* m_passBuffer = nullptr;
    UploadBufferDX12* m_renderObjectBuffer = nullptr;

    std::vector<RenderPacketList> m_renderPacketListPool;
    std::map<RenderPassHandle, RenderPacketList*> m_passesRenderPackets;
    uint32 m_packetListPoolIndex = 0;
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