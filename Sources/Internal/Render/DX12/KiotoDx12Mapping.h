#pragma once

#include <map>

#include "Render/PipelineStateParams.h"
#include "Render/Texture/Texture.h"

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
static std::map<eTextureFormat, DXGI_FORMAT> ResourceFormats
{
    { eTextureFormat::Format_UNKNOWN, DXGI_FORMAT_UNKNOWN },
    { eTextureFormat::Format_R32G32B32A32_TYPELESS, DXGI_FORMAT_R32G32B32A32_TYPELESS },
    { eTextureFormat::Format_R32G32B32A32_TYPELESS, DXGI_FORMAT_R32G32B32A32_TYPELESS },
    { eTextureFormat::Format_R32G32B32A32_UINT, DXGI_FORMAT_R32G32B32A32_UINT },
    { eTextureFormat::Format_R32G32B32A32_SINT, DXGI_FORMAT_R32G32B32A32_SINT },
    { eTextureFormat::Format_R32G32B32_TYPELESS, DXGI_FORMAT_R32G32B32_TYPELESS },
    { eTextureFormat::Format_R32G32B32_FLOAT, DXGI_FORMAT_R32G32B32_FLOAT },
    { eTextureFormat::Format_R32G32B32_UINT, DXGI_FORMAT_R32G32B32_UINT },
    { eTextureFormat::Format_R32G32B32_SINT, DXGI_FORMAT_R32G32B32_SINT },
    { eTextureFormat::Format_R16G16B16A16_TYPELESS, DXGI_FORMAT_R16G16B16A16_TYPELESS },
    { eTextureFormat::Format_R16G16B16A16_FLOAT, DXGI_FORMAT_R16G16B16A16_FLOAT },
    { eTextureFormat::Format_R16G16B16A16_UNORM, DXGI_FORMAT_R16G16B16A16_UNORM },
    { eTextureFormat::Format_R16G16B16A16_UINT, DXGI_FORMAT_R16G16B16A16_UINT },
    { eTextureFormat::Format_R16G16B16A16_SNORM, DXGI_FORMAT_R16G16B16A16_SNORM },
    { eTextureFormat::Format_R16G16B16A16_SINT, DXGI_FORMAT_R16G16B16A16_SINT },
    { eTextureFormat::Format_R32G32_TYPELESS, DXGI_FORMAT_R32G32_TYPELESS },
    { eTextureFormat::Format_R32G32_FLOAT, DXGI_FORMAT_R32G32_FLOAT },
    { eTextureFormat::Format_R32G32_UINT, DXGI_FORMAT_R32G32_UINT },
    { eTextureFormat::Format_R32G32_SINT, DXGI_FORMAT_R32G32_SINT },
    { eTextureFormat::Format_R32G8X24_TYPELESS, DXGI_FORMAT_R32G8X24_TYPELESS },
    { eTextureFormat::Format_D32_FLOAT_S8X24_UINT, DXGI_FORMAT_D32_FLOAT_S8X24_UINT },
    { eTextureFormat::Format_R32_FLOAT_X8X24_TYPELESS, DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS },
    { eTextureFormat::Format_X32_TYPELESS_G8X24_UINT, DXGI_FORMAT_X32_TYPELESS_G8X24_UINT },
    { eTextureFormat::Format_R10G10B10A2_TYPELESS, DXGI_FORMAT_R10G10B10A2_TYPELESS },
    { eTextureFormat::Format_R10G10B10A2_UNORM, DXGI_FORMAT_R10G10B10A2_UNORM },
    { eTextureFormat::Format_R10G10B10A2_UINT, DXGI_FORMAT_R10G10B10A2_UINT },
    { eTextureFormat::Format_R11G11B10_FLOAT, DXGI_FORMAT_R11G11B10_FLOAT },
    { eTextureFormat::Format_R8G8B8A8_TYPELESS, DXGI_FORMAT_R8G8B8A8_TYPELESS },
    { eTextureFormat::Format_R8G8B8A8_UNORM, DXGI_FORMAT_R8G8B8A8_UNORM },
    { eTextureFormat::Format_R8G8B8A8_UNORM_SRGB, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB },
    { eTextureFormat::Format_R8G8B8A8_UINT, DXGI_FORMAT_R8G8B8A8_UINT },
    { eTextureFormat::Format_R8G8B8A8_SNORM, DXGI_FORMAT_R8G8B8A8_SNORM },
    { eTextureFormat::Format_R8G8B8A8_SINT, DXGI_FORMAT_R8G8B8A8_SINT },
    { eTextureFormat::Format_R16G16_TYPELESS, DXGI_FORMAT_R16G16_TYPELESS },
    { eTextureFormat::Format_R16G16_FLOAT, DXGI_FORMAT_R16G16_FLOAT },
    { eTextureFormat::Format_R16G16_UNORM, DXGI_FORMAT_R16G16_UNORM },
    { eTextureFormat::Format_R16G16_UINT, DXGI_FORMAT_R16G16_UINT },
    { eTextureFormat::Format_R16G16_SNORM, DXGI_FORMAT_R16G16_SNORM },
    { eTextureFormat::Format_R16G16_SINT, DXGI_FORMAT_R16G16_SINT },
    { eTextureFormat::Format_R32_TYPELESS, DXGI_FORMAT_R32_TYPELESS },
    { eTextureFormat::Format_D32_FLOAT, DXGI_FORMAT_D32_FLOAT },
    { eTextureFormat::Format_R32_FLOAT, DXGI_FORMAT_R32_FLOAT },
    { eTextureFormat::Format_R32_UINT, DXGI_FORMAT_R32_UINT },
    { eTextureFormat::Format_R32_SINT, DXGI_FORMAT_R32_SINT },
    { eTextureFormat::Format_R24G8_TYPELESS, DXGI_FORMAT_R24G8_TYPELESS },
    { eTextureFormat::Format_D24_UNORM_S8_UINT, DXGI_FORMAT_D24_UNORM_S8_UINT },
    { eTextureFormat::Format_R24_UNORM_X8_TYPELESS, DXGI_FORMAT_R24_UNORM_X8_TYPELESS },
    { eTextureFormat::Format_X24_TYPELESS_G8_UINT, DXGI_FORMAT_X24_TYPELESS_G8_UINT },
    { eTextureFormat::Format_R8G8_TYPELESS, DXGI_FORMAT_R8G8_TYPELESS },
    { eTextureFormat::Format_R8G8_UNORM, DXGI_FORMAT_R8G8_UNORM },
    { eTextureFormat::Format_R8G8_UINT, DXGI_FORMAT_R8G8_UINT },
    { eTextureFormat::Format_R8G8_SNORM, DXGI_FORMAT_R8G8_SNORM },
    { eTextureFormat::Format_R8G8_SINT, DXGI_FORMAT_R8G8_SINT },
    { eTextureFormat::Format_R16_TYPELESS, DXGI_FORMAT_R16_TYPELESS },
    { eTextureFormat::Format_R16_FLOAT, DXGI_FORMAT_R16_FLOAT },
    { eTextureFormat::Format_D16_UNORM, DXGI_FORMAT_D16_UNORM },
    { eTextureFormat::Format_R16_UNORM, DXGI_FORMAT_R16_UNORM },
    { eTextureFormat::Format_R16_UINT, DXGI_FORMAT_R16_UINT },
    { eTextureFormat::Format_R16_SNORM,DXGI_FORMAT_R16_SNORM },
    { eTextureFormat::Format_R16_SINT, DXGI_FORMAT_R16_SINT },
    { eTextureFormat::Format_R8_TYPELESS, DXGI_FORMAT_R8_TYPELESS },
    { eTextureFormat::Format_R8_UNORM, DXGI_FORMAT_R8_UNORM },
    { eTextureFormat::Format_R8_UINT, DXGI_FORMAT_R8_UINT },
    { eTextureFormat::Format_R8_SNORM, DXGI_FORMAT_R8_SNORM },
    { eTextureFormat::Format_R8_SINT, DXGI_FORMAT_R8_SINT },
    { eTextureFormat::Format_A8_UNORM, DXGI_FORMAT_A8_UNORM },
    { eTextureFormat::Format_R1_UNORM, DXGI_FORMAT_R1_UNORM },
    { eTextureFormat::Format_R9G9B9E5_SHAREDEXP, DXGI_FORMAT_R9G9B9E5_SHAREDEXP },
    { eTextureFormat::Format_R8G8_B8G8_UNORM, DXGI_FORMAT_R8G8_B8G8_UNORM },
    { eTextureFormat::Format_G8R8_G8B8_UNORM, DXGI_FORMAT_G8R8_G8B8_UNORM },
    { eTextureFormat::Format_BC1_TYPELESS, DXGI_FORMAT_BC1_TYPELESS },
    { eTextureFormat::Format_BC1_UNORM, DXGI_FORMAT_BC1_UNORM },
    { eTextureFormat::Format_BC1_UNORM_SRGB, DXGI_FORMAT_BC1_UNORM_SRGB },
    { eTextureFormat::Format_BC2_TYPELESS, DXGI_FORMAT_BC2_TYPELESS },
    { eTextureFormat::Format_BC2_UNORM, DXGI_FORMAT_BC2_UNORM },
    { eTextureFormat::Format_BC2_UNORM_SRGB, DXGI_FORMAT_BC2_UNORM_SRGB },
    { eTextureFormat::Format_BC3_TYPELESS, DXGI_FORMAT_BC3_TYPELESS },
    { eTextureFormat::Format_BC3_UNORM, DXGI_FORMAT_BC3_UNORM },
    { eTextureFormat::Format_BC3_UNORM_SRGB, DXGI_FORMAT_BC3_UNORM_SRGB },
    { eTextureFormat::Format_BC4_TYPELESS, DXGI_FORMAT_BC4_TYPELESS },
    { eTextureFormat::Format_BC4_UNORM, DXGI_FORMAT_BC4_UNORM },
    { eTextureFormat::Format_BC4_SNORM, DXGI_FORMAT_BC4_SNORM },
    { eTextureFormat::Format_BC5_TYPELESS, DXGI_FORMAT_BC5_TYPELESS },
    { eTextureFormat::Format_BC5_UNORM, DXGI_FORMAT_BC5_UNORM },
    { eTextureFormat::Format_BC5_SNORM, DXGI_FORMAT_BC5_SNORM },
    { eTextureFormat::Format_B5G6R5_UNORM, DXGI_FORMAT_B5G6R5_UNORM },
    { eTextureFormat::Format_B5G5R5A1_UNORM, DXGI_FORMAT_B5G5R5A1_UNORM },
    { eTextureFormat::Format_B8G8R8A8_UNORM, DXGI_FORMAT_B8G8R8A8_UNORM },
    { eTextureFormat::Format_B8G8R8X8_UNORM, DXGI_FORMAT_B8G8R8X8_UNORM },
    { eTextureFormat::Format_R10G10B10_XR_BIAS_A2_UNORM, DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM },
    { eTextureFormat::Format_B8G8R8A8_TYPELESS, DXGI_FORMAT_B8G8R8A8_TYPELESS },
    { eTextureFormat::Format_B8G8R8A8_UNORM_SRGB, DXGI_FORMAT_B8G8R8A8_UNORM_SRGB },
    { eTextureFormat::Format_B8G8R8X8_TYPELESS, DXGI_FORMAT_B8G8R8X8_TYPELESS },
    { eTextureFormat::Format_B8G8R8X8_UNORM_SRGB, DXGI_FORMAT_B8G8R8X8_UNORM_SRGB },
    { eTextureFormat::Format_BC6H_TYPELESS, DXGI_FORMAT_BC6H_TYPELESS },
    { eTextureFormat::Format_BC6H_UF16, DXGI_FORMAT_BC6H_UF16 },
    { eTextureFormat::Format_BC6H_SF16, DXGI_FORMAT_BC6H_SF16 },
    { eTextureFormat::Format_BC7_TYPELESS, DXGI_FORMAT_BC7_TYPELESS },
    { eTextureFormat::Format_BC7_UNORM, DXGI_FORMAT_BC7_UNORM },
    { eTextureFormat::Format_BC7_UNORM_SRGB, DXGI_FORMAT_BC7_UNORM_SRGB },
    { eTextureFormat::Format_AYUV, DXGI_FORMAT_AYUV },
    { eTextureFormat::Format_Y410, DXGI_FORMAT_Y410 },
    { eTextureFormat::Format_Y416, DXGI_FORMAT_Y416 },
    { eTextureFormat::Format_NV12, DXGI_FORMAT_NV12 },
    { eTextureFormat::Format_P010, DXGI_FORMAT_P010 },
    { eTextureFormat::Format_P016, DXGI_FORMAT_P016 },
    { eTextureFormat::Format_420_OPAQUE, DXGI_FORMAT_420_OPAQUE },
    { eTextureFormat::Format_YUY2, DXGI_FORMAT_YUY2 },
    { eTextureFormat::Format_Y210, DXGI_FORMAT_Y210 },
    { eTextureFormat::Format_Y216, DXGI_FORMAT_Y216 },
    { eTextureFormat::Format_NV11, DXGI_FORMAT_NV11 },
    { eTextureFormat::Format_AI44, DXGI_FORMAT_AI44 },
    { eTextureFormat::Format_IA44, DXGI_FORMAT_IA44 },
    { eTextureFormat::Format_P8, DXGI_FORMAT_P8 },
    { eTextureFormat::Format_A8P8,DXGI_FORMAT_A8P8 },
    { eTextureFormat::Format_B4G4R4A4_UNORM, DXGI_FORMAT_B4G4R4A4_UNORM },
    { eTextureFormat::Format_P208, DXGI_FORMAT_P208 },
    { eTextureFormat::Format_V208, DXGI_FORMAT_V208 },
    { eTextureFormat::Format_V408, DXGI_FORMAT_V408 },
    { eTextureFormat::Format_FORCE_UINT, DXGI_FORMAT_FORCE_UINT }
};
static std::map<eTextureDim, D3D12_RESOURCE_DIMENSION> ResourceDimensions
{
    { eTextureDim::Texture2D, D3D12_RESOURCE_DIMENSION_TEXTURE2D }
};
}