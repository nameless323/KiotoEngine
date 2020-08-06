#include "stdafx.h"

#include "Render/DX12/PsoManager.h"

#include "Render/DX12/KiotoDx12Mapping.h"
#include "Render/DX12/RootSignatureManager.h"
#include "Render/DX12/ShaderManagerDX12.h"
#include "Render/DX12/StateDX.h"
#include "Render/DX12/Texture/TextureManagerDX12.h"
#include "Render/DX12/Texture/TextureDX12.h"
#include "Render/DX12/VertexLayoutManagerDX12.h"
#include "Render/Material.h"
#include "Render/PipelineState.h"
#include "Render/RenderPass/RenderPass.h"
#include "Render/Shader.h"

namespace Kioto::Renderer
{
namespace
{
D3D12_RASTERIZER_DESC ParseRasterizerDesc(const PipelineState& state)
{
    D3D12_RASTERIZER_DESC desc = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    desc.CullMode = KiotoDx12Mapping::CullModes[state.Cull];
    desc.FillMode = KiotoDx12Mapping::FillModes[state.Fill];
    return desc;
}

D3D12_BLEND_DESC ParseBlendState(const PipelineState& state)
{
    D3D12_BLEND_DESC desc = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    desc.IndependentBlendEnable = FALSE;
    desc.RenderTarget[0].BlendEnable = state.LayerType != eRenderLayerType::Opaque;
    desc.RenderTarget[0].BlendOp = KiotoDx12Mapping::BlendOps[state.BlendOp];
    desc.RenderTarget[0].BlendOpAlpha = KiotoDx12Mapping::BlendOps[state.BlendOp];
    desc.RenderTarget[0].DestBlend = KiotoDx12Mapping::BlendModes[state.DstBlend];
    desc.RenderTarget[0].DestBlendAlpha = KiotoDx12Mapping::BlendModes[state.DstBlend];
    desc.RenderTarget[0].SrcBlend = KiotoDx12Mapping::BlendModes[state.SrcBlend];
    desc.RenderTarget[0].SrcBlendAlpha = KiotoDx12Mapping::BlendModes[state.SrcBlend] ;
    desc.RenderTarget[0].RenderTargetWriteMask = KiotoDx12Mapping::ColorWrites[state.ColorMask]; // [a_vorontcov] TODO
    return desc;
}

D3D12_DEPTH_STENCIL_DESC ParseDepthStencil(const PipelineState& state)
{
    D3D12_DEPTH_STENCIL_DESC desc = {};
    desc.BackFace.StencilDepthFailOp = KiotoDx12Mapping::StencilOps[state.BackFaceStencilDesc.StencilDepthFailOp];
    desc.BackFace.StencilFailOp = KiotoDx12Mapping::StencilOps[state.BackFaceStencilDesc.StencilFailOp];
    desc.BackFace.StencilFunc = KiotoDx12Mapping::StencilFunc[state.BackFaceStencilDesc.StencilFunc];
    desc.BackFace.StencilPassOp = KiotoDx12Mapping::StencilOps[state.BackFaceStencilDesc.StencilPassOp];

    desc.FrontFace.StencilDepthFailOp = KiotoDx12Mapping::StencilOps[state.FrontFaceStencilDesc.StencilDepthFailOp];
    desc.FrontFace.StencilFailOp = KiotoDx12Mapping::StencilOps[state.FrontFaceStencilDesc.StencilFailOp];
    desc.FrontFace.StencilFunc = KiotoDx12Mapping::StencilFunc[state.FrontFaceStencilDesc.StencilFunc];
    desc.FrontFace.StencilPassOp = KiotoDx12Mapping::StencilOps[state.FrontFaceStencilDesc.StencilPassOp];

    desc.DepthWriteMask = state.Zwrite ? D3D12_DEPTH_WRITE_MASK_ALL : D3D12_DEPTH_WRITE_MASK_ZERO;
    desc.DepthEnable = state.EnableDepth;
    desc.DepthFunc = KiotoDx12Mapping::DepthFunc[state.Ztest];
    desc.StencilEnable = state.EnableStencill;
    desc.StencilReadMask = state.StencilReadMask;
    desc.StencilWriteMask = state.StencilWriteMask;

    return desc;
}

D3D12_GRAPHICS_PIPELINE_STATE_DESC ParsePipelineState(Material* mat, const RenderPass* pass, const RootSignatureManager& sigManager, TextureManagerDX12* textureManager, ShaderManagerDX12* shaderManager, VertexLayoutManagerDX12* vertexLayoutManager, DXGI_FORMAT backBufferFromat, DXGI_FORMAT defaultDepthStencilFormat)
{
    const PipelineState& state = mat->GetPipelineState(pass->GetName());

    D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = {};
    desc.pRootSignature = sigManager.GetRootSignature(state.Shader->GetHandle());
    const auto& shaders = shaderManager->GetDxShaders(state.Shader->GetHandle());
    for (const auto& shader : *shaders)
    {
        if (shader.GetType() == ShaderProgramType::Fragment)
            desc.PS = shader.GetBytecode();
        else if (shader.GetType() == ShaderProgramType::Vertex)
            desc.VS = shader.GetBytecode();
    }
    auto currentLayout = vertexLayoutManager->FindVertexLayout(state.Shader->GetHandle());
    desc.InputLayout = { currentLayout->data(), static_cast<UINT>(currentLayout->size()) };
    desc.RasterizerState = ParseRasterizerDesc(state);
    desc.BlendState = ParseBlendState(state);
    desc.DepthStencilState = ParseDepthStencil(state);

    DXGI_FORMAT dsvFormat = defaultDepthStencilFormat;
    if (pass->GetDepthStencil().GetHandle() != InvalidHandle)
        dsvFormat = textureManager->FindTexture(pass->GetDepthStencil().GetHandle())->GetFormat();
 
    desc.DSVFormat = dsvFormat;
    desc.SampleMask = UINT_MAX;
    desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    desc.NumRenderTargets = pass->GetRenderTargetCount();
    for (uint32 i = 0; i < pass->GetRenderTargetCount(); ++i)
    {
        DXGI_FORMAT rtvFormat = backBufferFromat;
        if (pass->GetRenderTarget(i).GetHandle() != InvalidHandle)
            rtvFormat = textureManager->FindTexture(pass->GetRenderTarget(i).GetHandle())->GetFormat();

        desc.RTVFormats[i] = rtvFormat;
    }
    desc.SampleDesc.Count = 1;

    return desc;
}
}

void PsoManager::BuildPipelineState(const StateDX& state, Material* mat, const RenderPass* pass, const RootSignatureManager& sigManager, TextureManagerDX12* textureManager, ShaderManagerDX12* shaderManager, VertexLayoutManagerDX12* vertexLayoutManager, DXGI_FORMAT backBufferFromat, DXGI_FORMAT defaultDepthStencilFormat)
{
    uint64 key = GetKey(mat->GetHandle(), pass->GetHandle());
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