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
#include "Render/Texture/TextureDX12.h"
#include "Render/RendererPublic.h"
#include "Render/RenderPass/RenderPass.h"
#include "Render/Texture/TextureSet.h"
#include "Render/DX12/StateDX.h"
#include "Render/DX12/SwapChain.h"

namespace Kioto::Renderer
{

class Texture;
class VertexLayout;
class UploadBufferDX12;
namespace ShaderParser
{
struct ParseResult;
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

    VertexLayoutHandle GenerateVertexLayout(const VertexLayout& layout);

    TextureHandle GetCurrentBackBufferHandle() const;
    TextureHandle GetDepthStencilHandle() const;

private:
    StateDX m_state;
    SwapChain m_swapChain;

    std::unordered_map<uint32, ResourceDX12> m_resources;
    std::array<std::vector<RenderPass>, StateDX::FrameCount> m_renderPasses;

    ResourceDX12* FindDxResource(uint32 handle);
    const CD3DX12_SHADER_BYTECODE* GetShaderBytecode(ShaderHandle handle) const;
    const std::vector<D3D12_INPUT_ELEMENT_DESC>* FindVertexLayout(VertexLayoutHandle handle) const;

    void GetHardwareAdapter(IDXGIFactory4* factory, IDXGIAdapter1** adapter);
    void WaitForGPU();
    void LogAdapters();
    void LogAdapterOutputs(IDXGIAdapter* adapter);
    void LogOutputDisplayModes(IDXGIOutput* output, DXGI_FORMAT format);

    void LoadPipeline();
    void UpdateTimeCB();
    void UpdateRenderObjectCB();
    void UpdatePassCB();

    void CreateRootSignature(const ShaderParser::ParseResult& parseResult, ShaderHandle handle);
    void UpdateTextureSetHeap(const TextureSet& texSet);
    ID3D12DescriptorHeap* GetTextureHeap(TextureSetHandle handle) const;

    bool m_isTearingSupported = false; // [a_vorontsov] TODO: Properly handle when tearing is not supported.
    UINT m_width = -1;
    UINT m_height = -1;
    bool m_isFullScreen = false;


    std::map<ShaderHandle, Microsoft::WRL::ComPtr<ID3D12RootSignature>> m_rootSignature;

    Microsoft::WRL::ComPtr<ID3D12PipelineState> m_fallbackPSO;
    ShaderHandle m_vs;
    ShaderHandle m_ps;
    std::vector<ShaderDX12*> m_shaders; // [a_vorontsov] To map or set.
    std::vector<TextureDX12*> m_textures;
    std::map<TextureSetHandle, Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>> m_textureHeaps; // [a_vorontsov] One tex heap for all textures?

    std::unique_ptr<VertexBufferDX12> m_vertexBuffer;
    std::unique_ptr<IndexBufferDX12> m_indexBuffer;

    Mesh* m_box;
    //std::unique_ptr<TextureDX12> m_textureDX;
    //Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_textureHeap;

    std::vector<VertexLayoutDX12> m_inputLayouts;

    EngineBuffers engineBuffers;
    UploadBufferDX12* m_timeBuffer = nullptr;
    UploadBufferDX12* m_passBuffer = nullptr;
    UploadBufferDX12* m_renderObjectBuffer = nullptr;

    Texture* m_texture = nullptr;
    TextureSet m_textureSet;
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