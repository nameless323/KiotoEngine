//
// Copyright (C) Alexandr Vorontsov 2017.
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#include "stdafx.h"

#include "Render/DX12/PsoManager.h"
#include "Render/PipelineState.h"
#include "Render/Texture/TextureManagerDX12.h"
#include "Render/RenderPass/RenderPass.h"
#include "Render/Texture/TextureManagerDX12.h"
#include "Render/Texture/TextureDX12.h"
#include "Render/Material.h"
#include "Render/DX12/ShaderManagerDX12.h"
#include "Render/DX12/VertexLayoutManagerDX12.h"
#include "Render/DX12/StateDX.h"
#include "Render/DX12/RootSignatureManager.h"

namespace Kioto::Renderer
{
namespace
{
D3D12_RASTERIZER_DESC ParseRasterizerDesc(const PipelineState& state)
{
    static std::map<eCullMode, D3D12_CULL_MODE> cullModes
    {
        { eCullMode::Front, D3D12_CULL_MODE_FRONT },
        { eCullMode::Back, D3D12_CULL_MODE_BACK },
        { eCullMode::None, D3D12_CULL_MODE_NONE }
    };
    static std::map<eFillMode, D3D12_FILL_MODE> fillModes
    {
        { eFillMode::Solid, D3D12_FILL_MODE_SOLID},
        { eFillMode::Wireframe, D3D12_FILL_MODE_WIREFRAME}
    };
    D3D12_RASTERIZER_DESC desc = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    desc.CullMode = cullModes[state.Cull];
    desc.FillMode = fillModes[state.Fill];
    return desc;
}

D3D12_BLEND_DESC ParseBlendState(const PipelineState& state)
{
    static std::map<eBlendModes, D3D12_BLEND> blends
    {
        { eBlendModes::DstAlpha, D3D12_BLEND_DEST_ALPHA },
        { eBlendModes::DstColor, D3D12_BLEND_DEST_COLOR },
        { eBlendModes::One, D3D12_BLEND_ONE },
        { eBlendModes::OneMinusDstAlpha, D3D12_BLEND_INV_DEST_ALPHA },
        { eBlendModes::OneMinusDstColor, D3D12_BLEND_INV_DEST_COLOR },
        { eBlendModes::OneMinusSrcAlpha, D3D12_BLEND_INV_SRC_ALPHA },
        { eBlendModes::OneMinusSrcColor, D3D12_BLEND_INV_SRC_COLOR },
        { eBlendModes::SrcAlpha, D3D12_BLEND_SRC_ALPHA },
        { eBlendModes::SrcAlphaSat, D3D12_BLEND_SRC_ALPHA_SAT },
        { eBlendModes::SrcColor, D3D12_BLEND_SRC_COLOR },
        { eBlendModes::Zero, D3D12_BLEND_ZERO },
    };
    static std::map<eBlendOps, D3D12_BLEND_OP> blendOps
    {
        { eBlendOps::Add, D3D12_BLEND_OP_ADD },
        { eBlendOps::Substract, D3D12_BLEND_OP_SUBTRACT },
        { eBlendOps::Max, D3D12_BLEND_OP_MAX },
        { eBlendOps::Min, D3D12_BLEND_OP_MIN },
        { eBlendOps::ReverseSubstract, D3D12_BLEND_OP_REV_SUBTRACT },
    };
    static std::map<eColorMask, D3D12_COLOR_WRITE_ENABLE> colorWrites
    {
        { eColorMask::All, D3D12_COLOR_WRITE_ENABLE_ALL },
        { eColorMask::Red, D3D12_COLOR_WRITE_ENABLE_RED },
        { eColorMask::Green, D3D12_COLOR_WRITE_ENABLE_GREEN },
        { eColorMask::Blue, D3D12_COLOR_WRITE_ENABLE_BLUE },
        { eColorMask::Alpha, D3D12_COLOR_WRITE_ENABLE_ALPHA }
    };
    D3D12_BLEND_DESC desc = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    desc.IndependentBlendEnable = FALSE;
    desc.RenderTarget[0].BlendEnable = state.LayerType != eRenderLayerType::Opaque;
    desc.RenderTarget[0].BlendOp = blendOps[state.BlendOp];
    desc.RenderTarget[0].BlendOpAlpha = blendOps[state.BlendOp];
    desc.RenderTarget[0].DestBlend = blends[state.DstBlend];
    desc.RenderTarget[0].DestBlendAlpha = blends[state.DstBlend];
    desc.RenderTarget[0].SrcBlend = blends[state.SrcBlend];
    desc.RenderTarget[0].SrcBlendAlpha = blends[state.SrcBlend];
    desc.RenderTarget[0].RenderTargetWriteMask = colorWrites[state.ColorMask]; // [a_vorontsov] TODO
    return desc;
}

D3D12_DEPTH_STENCIL_DESC ParseDepthStencil(const PipelineState& state)
{
    static std::map<eStencilOp, D3D12_STENCIL_OP> stencilOps
    {
        { eStencilOp::Keep, D3D12_STENCIL_OP_KEEP },
        { eStencilOp::Zero, D3D12_STENCIL_OP_ZERO },
        { eStencilOp::Replace, D3D12_STENCIL_OP_REPLACE },
        { eStencilOp::IncrSat, D3D12_STENCIL_OP_INCR_SAT },
        { eStencilOp::DectSat, D3D12_STENCIL_OP_DECR_SAT },
        { eStencilOp::Invert, D3D12_STENCIL_OP_INVERT },
        { eStencilOp::Incr, D3D12_STENCIL_OP_INCR },
        { eStencilOp::Decr, D3D12_STENCIL_OP_DECR }
    };
    static std::map<eStencilTest, D3D12_COMPARISON_FUNC> stencilFunc
    {
        { eStencilTest::Never, D3D12_COMPARISON_FUNC_NEVER },
        { eStencilTest::Less, D3D12_COMPARISON_FUNC_LESS },
        { eStencilTest::Equal, D3D12_COMPARISON_FUNC_EQUAL },
        { eStencilTest::LEqual, D3D12_COMPARISON_FUNC_LESS_EQUAL },
        { eStencilTest::Greater, D3D12_COMPARISON_FUNC_GREATER },
        { eStencilTest::NotEqual, D3D12_COMPARISON_FUNC_NOT_EQUAL },
        { eStencilTest::GEqual, D3D12_COMPARISON_FUNC_GREATER_EQUAL },
        { eStencilTest::Always, D3D12_COMPARISON_FUNC_ALWAYS }
    };
    static std::map<eZTest, D3D12_COMPARISON_FUNC> depthFunc
    {
        { eZTest::Never, D3D12_COMPARISON_FUNC_NEVER },
        { eZTest::Less, D3D12_COMPARISON_FUNC_LESS },
        { eZTest::Equal, D3D12_COMPARISON_FUNC_EQUAL },
        { eZTest::LEqual, D3D12_COMPARISON_FUNC_LESS_EQUAL },
        { eZTest::Greater, D3D12_COMPARISON_FUNC_GREATER },
        { eZTest::NotEqual, D3D12_COMPARISON_FUNC_NOT_EQUAL },
        { eZTest::GEqual, D3D12_COMPARISON_FUNC_GREATER_EQUAL },
        { eZTest::Always, D3D12_COMPARISON_FUNC_ALWAYS }
    };
    D3D12_DEPTH_STENCIL_DESC desc = {};
    desc.BackFace.StencilDepthFailOp = stencilOps[state.BackFaceStencilDesc.StencilDepthFailOp];
    desc.BackFace.StencilFailOp = stencilOps[state.BackFaceStencilDesc.StencilFailOp];
    desc.BackFace.StencilFunc = stencilFunc[state.BackFaceStencilDesc.StencilFunc];
    desc.BackFace.StencilPassOp = stencilOps[state.BackFaceStencilDesc.StencilPassOp];

    desc.FrontFace.StencilDepthFailOp = stencilOps[state.FrontFaceStencilDesc.StencilDepthFailOp];
    desc.FrontFace.StencilFailOp = stencilOps[state.FrontFaceStencilDesc.StencilFailOp];
    desc.FrontFace.StencilFunc = stencilFunc[state.FrontFaceStencilDesc.StencilFunc];
    desc.FrontFace.StencilPassOp = stencilOps[state.FrontFaceStencilDesc.StencilPassOp];

    desc.DepthWriteMask = state.Zwrite ? D3D12_DEPTH_WRITE_MASK_ALL : D3D12_DEPTH_WRITE_MASK_ZERO;
    desc.DepthEnable = state.EnableDepth;
    desc.DepthFunc = depthFunc[state.Ztest];
    desc.StencilEnable = state.EnableStencill;
    desc.StencilReadMask = state.StencilReadMask;
    desc.StencilWriteMask = state.StencilWriteMask;

    return desc;
}

D3D12_GRAPHICS_PIPELINE_STATE_DESC ParsePipelineState(Material* mat, const RenderPass& pass, const RootSignatureManager& sigManager, TextureManagerDX12* textureManager, ShaderManagerDX12* shaderManager, VertexLayoutManagerDX12* vertexLayoutManager, DXGI_FORMAT backBufferFromat, DXGI_FORMAT defaultDepthStencilFormat)
{
    D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = {};
    desc.pRootSignature = sigManager.GetRootSignature(mat->GetShader()->GetHandle());
    const auto& shaders = shaderManager->GetDxShaders(mat->GetShader()->GetHandle());
    for (const auto& shader : *shaders)
    {
        if (shader.GetType() == ShaderProgramType::Fragment)
            desc.PS = shader.GetBytecode();
        else if (shader.GetType() == ShaderProgramType::Vertex)
            desc.VS = shader.GetBytecode();
    }
    auto currentLayout = vertexLayoutManager->FindVertexLayout(mat->GetShader()->GetHandle());
    desc.InputLayout = { currentLayout->data(), static_cast<UINT>(currentLayout->size()) };
    const PipelineState& state = mat->GetShaderData().pipelineState;
    desc.RasterizerState = ParseRasterizerDesc(state);
    desc.BlendState = ParseBlendState(state);
    desc.DepthStencilState = ParseDepthStencil(state);

    DXGI_FORMAT dsvFormat = defaultDepthStencilFormat;
    if (pass.GetDepthStencil().GetHandle() != InvalidHandle)
        dsvFormat = textureManager->FindTexture(pass.GetDepthStencil().GetHandle())->GetFormat();
 
    desc.DSVFormat = dsvFormat;
    desc.SampleMask = UINT_MAX;
    desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    desc.NumRenderTargets = pass.GetRenderTargetCount();
    for (uint32 i = 0; i < pass.GetRenderTargetCount(); ++i)
    {
        DXGI_FORMAT rtvFormat = backBufferFromat;
        if (pass.GetRenderTarget(i).GetHandle() != InvalidHandle)
            rtvFormat = textureManager->FindTexture(pass.GetRenderTarget(i).GetHandle())->GetFormat();

        desc.RTVFormats[i] = rtvFormat;
    }
    desc.SampleDesc.Count = 1;

    return desc;
}
}

void PsoManager::BuildPipelineState(const StateDX& state, Material* mat, const RenderPass& pass, const RootSignatureManager& sigManager, TextureManagerDX12* textureManager, ShaderManagerDX12* shaderManager, VertexLayoutManagerDX12* vertexLayoutManager, DXGI_FORMAT backBufferFromat, DXGI_FORMAT defaultDepthStencilFormat)
{
    uint64 key = GetKey(mat->GetHandle(), pass.GetHandle());
    if (m_psos.find(key) != m_psos.end())
        return;
    D3D12_GRAPHICS_PIPELINE_STATE_DESC stateDesc = ParsePipelineState(mat, pass, sigManager, textureManager, shaderManager, vertexLayoutManager, backBufferFromat, defaultDepthStencilFormat);
    ID3D12PipelineState** pipeState = m_psos[key].GetAddressOf();
    ThrowIfFailed(state.Device->CreateGraphicsPipelineState(&stateDesc, IID_PPV_ARGS(pipeState)));
}

ID3D12PipelineState* PsoManager::GetPipelineState(MaterialHandle matHandle, RenderPassHandle renderPassHandle)
{
    uint64 key = GetKey(matHandle, renderPassHandle);
    auto it = m_psos.find(key);
    if (it == m_psos.end())
        return nullptr;
    return it->second.Get();
}

uint64 PsoManager::GetKey(MaterialHandle matHandle, RenderPassHandle renderPassHandle)
{
    uint64 tmp = renderPassHandle.GetHandle();
    return matHandle.GetHandle() | tmp << 32;
}

}