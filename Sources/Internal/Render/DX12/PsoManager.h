//
// Copyright (C) Aleksandr Vorontcov 2017.
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#pragma once

#include <d3d12.h>
#include <map>
#include <wrl/client.h>

#include "Render/RendererPublic.h"
#include "Core/CoreTypes.h"

namespace Kioto::Renderer
{
class Material;
class RenderPass;
class TextureManagerDX12;
class ShaderManagerDX12;
class RootSignatureManager;
class VertexLayoutManagerDX12;
struct StateDX;

class PsoManager
{
public:
    PsoManager() = default;

    void BuildPipelineState(const StateDX& state, Material* mat, const RenderPass* pass, const RootSignatureManager& sigManager, TextureManagerDX12* textureManager, ShaderManagerDX12* shaderManager, VertexLayoutManagerDX12* vertexLayoutManager, DXGI_FORMAT backBufferFromat, DXGI_FORMAT defaultDepthStencilFormat);
    ID3D12PipelineState* GetPipelineState(MaterialHandle matHandle, RenderPassHandle renderPassHandle);

private:
    static uint64 GetKey(MaterialHandle matHandle, RenderPassHandle renderPassHandle);

    std::map<uint64, Microsoft::WRL::ComPtr<ID3D12PipelineState>> m_psos;
};
}