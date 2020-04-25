#pragma once

#include <map>

#include "Render/PipelineStateParams.h"

#include "Sources/External/Dx12Helpers/d3dx12.h"

namespace Kioto::Renderer::KiotoDx12Mapping
{
std::map<eCullMode, D3D12_CULL_MODE> CullModes
{
    { eCullMode::Front, D3D12_CULL_MODE_FRONT },
    { eCullMode::Back, D3D12_CULL_MODE_BACK },
    { eCullMode::None, D3D12_CULL_MODE_NONE }
};
std::map<eFillMode, D3D12_FILL_MODE> FillModes
{
    { eFillMode::Solid, D3D12_FILL_MODE_SOLID},
    { eFillMode::Wireframe, D3D12_FILL_MODE_WIREFRAME}
};
std::map<eBlendModes, D3D12_BLEND> BlendModes
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
std::map<eBlendOps, D3D12_BLEND_OP> BlendOps
{
    { eBlendOps::Add, D3D12_BLEND_OP_ADD },
    { eBlendOps::Substract, D3D12_BLEND_OP_SUBTRACT },
    { eBlendOps::Max, D3D12_BLEND_OP_MAX },
    { eBlendOps::Min, D3D12_BLEND_OP_MIN },
    { eBlendOps::ReverseSubstract, D3D12_BLEND_OP_REV_SUBTRACT },
};
std::map<eColorMask, D3D12_COLOR_WRITE_ENABLE> ColorWrites
{
    { eColorMask::All, D3D12_COLOR_WRITE_ENABLE_ALL },
    { eColorMask::Red, D3D12_COLOR_WRITE_ENABLE_RED },
    { eColorMask::Green, D3D12_COLOR_WRITE_ENABLE_GREEN },
    { eColorMask::Blue, D3D12_COLOR_WRITE_ENABLE_BLUE },
    { eColorMask::Alpha, D3D12_COLOR_WRITE_ENABLE_ALPHA }
};
static std::map<eStencilOp, D3D12_STENCIL_OP> StencilOps
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
static std::map<eStencilTest, D3D12_COMPARISON_FUNC> StencilFunc
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
static std::map<eZTest, D3D12_COMPARISON_FUNC> DepthFunc
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
}