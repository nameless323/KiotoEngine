#pragma once

#include <map>

#include "Render/PipelineStateParams.h"
#include "Render/Texture/Texture.h"

#include "Sources/External/Dx12Helpers/d3dx12.h"

namespace Kioto::Renderer::KiotoDx12Mapping
{
inline std::map<eCullMode, D3D12_CULL_MODE> CullModes
{
    { eCullMode::Front, D3D12_CULL_MODE_FRONT },
    { eCullMode::Back, D3D12_CULL_MODE_BACK },
    { eCullMode::None, D3D12_CULL_MODE_NONE }
};
inline std::map<eFillMode, D3D12_FILL_MODE> FillModes
{
    { eFillMode::Solid, D3D12_FILL_MODE_SOLID},
    { eFillMode::Wireframe, D3D12_FILL_MODE_WIREFRAME}
};
inline std::map<eBlendModes, D3D12_BLEND> BlendModes
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
inline std::map<eBlendOps, D3D12_BLEND_OP> BlendOps
{
    { eBlendOps::Add, D3D12_BLEND_OP_ADD },
    { eBlendOps::Substract, D3D12_BLEND_OP_SUBTRACT },
    { eBlendOps::Max, D3D12_BLEND_OP_MAX },
    { eBlendOps::Min, D3D12_BLEND_OP_MIN },
    { eBlendOps::ReverseSubstract, D3D12_BLEND_OP_REV_SUBTRACT },
};
inline std::map<eColorMask, D3D12_COLOR_WRITE_ENABLE> ColorWrites
{
    { eColorMask::All, D3D12_COLOR_WRITE_ENABLE_ALL },
    { eColorMask::Red, D3D12_COLOR_WRITE_ENABLE_RED },
    { eColorMask::Green, D3D12_COLOR_WRITE_ENABLE_GREEN },
    { eColorMask::Blue, D3D12_COLOR_WRITE_ENABLE_BLUE },
    { eColorMask::Alpha, D3D12_COLOR_WRITE_ENABLE_ALPHA }
};
inline std::map<eStencilOp, D3D12_STENCIL_OP> StencilOps
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
inline std::map<eStencilTest, D3D12_COMPARISON_FUNC> StencilFunc
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
inline std::map<eZTest, D3D12_COMPARISON_FUNC> DepthFunc
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
inline std::map<eResourceFormat, DXGI_FORMAT> ResourceFormats
{
    { eResourceFormat::Format_UNKNOWN, DXGI_FORMAT_UNKNOWN },
    { eResourceFormat::Format_R32G32B32A32_TYPELESS, DXGI_FORMAT_R32G32B32A32_TYPELESS },
    { eResourceFormat::Format_R32G32B32A32_TYPELESS, DXGI_FORMAT_R32G32B32A32_TYPELESS },
    { eResourceFormat::Format_R32G32B32A32_UINT, DXGI_FORMAT_R32G32B32A32_UINT },
    { eResourceFormat::Format_R32G32B32A32_SINT, DXGI_FORMAT_R32G32B32A32_SINT },
    { eResourceFormat::Format_R32G32B32_TYPELESS, DXGI_FORMAT_R32G32B32_TYPELESS },
    { eResourceFormat::Format_R32G32B32_FLOAT, DXGI_FORMAT_R32G32B32_FLOAT },
    { eResourceFormat::Format_R32G32B32_UINT, DXGI_FORMAT_R32G32B32_UINT },
    { eResourceFormat::Format_R32G32B32_SINT, DXGI_FORMAT_R32G32B32_SINT },
    { eResourceFormat::Format_R16G16B16A16_TYPELESS, DXGI_FORMAT_R16G16B16A16_TYPELESS },
    { eResourceFormat::Format_R16G16B16A16_FLOAT, DXGI_FORMAT_R16G16B16A16_FLOAT },
    { eResourceFormat::Format_R16G16B16A16_UNORM, DXGI_FORMAT_R16G16B16A16_UNORM },
    { eResourceFormat::Format_R16G16B16A16_UINT, DXGI_FORMAT_R16G16B16A16_UINT },
    { eResourceFormat::Format_R16G16B16A16_SNORM, DXGI_FORMAT_R16G16B16A16_SNORM },
    { eResourceFormat::Format_R16G16B16A16_SINT, DXGI_FORMAT_R16G16B16A16_SINT },
    { eResourceFormat::Format_R32G32_TYPELESS, DXGI_FORMAT_R32G32_TYPELESS },
    { eResourceFormat::Format_R32G32_FLOAT, DXGI_FORMAT_R32G32_FLOAT },
    { eResourceFormat::Format_R32G32_UINT, DXGI_FORMAT_R32G32_UINT },
    { eResourceFormat::Format_R32G32_SINT, DXGI_FORMAT_R32G32_SINT },
    { eResourceFormat::Format_R32G8X24_TYPELESS, DXGI_FORMAT_R32G8X24_TYPELESS },
    { eResourceFormat::Format_D32_FLOAT_S8X24_UINT, DXGI_FORMAT_D32_FLOAT_S8X24_UINT },
    { eResourceFormat::Format_R32_FLOAT_X8X24_TYPELESS, DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS },
    { eResourceFormat::Format_X32_TYPELESS_G8X24_UINT, DXGI_FORMAT_X32_TYPELESS_G8X24_UINT },
    { eResourceFormat::Format_R10G10B10A2_TYPELESS, DXGI_FORMAT_R10G10B10A2_TYPELESS },
    { eResourceFormat::Format_R10G10B10A2_UNORM, DXGI_FORMAT_R10G10B10A2_UNORM },
    { eResourceFormat::Format_R10G10B10A2_UINT, DXGI_FORMAT_R10G10B10A2_UINT },
    { eResourceFormat::Format_R11G11B10_FLOAT, DXGI_FORMAT_R11G11B10_FLOAT },
    { eResourceFormat::Format_R8G8B8A8_TYPELESS, DXGI_FORMAT_R8G8B8A8_TYPELESS },
    { eResourceFormat::Format_R8G8B8A8_UNORM, DXGI_FORMAT_R8G8B8A8_UNORM },
    { eResourceFormat::Format_R8G8B8A8_UNORM_SRGB, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB },
    { eResourceFormat::Format_R8G8B8A8_UINT, DXGI_FORMAT_R8G8B8A8_UINT },
    { eResourceFormat::Format_R8G8B8A8_SNORM, DXGI_FORMAT_R8G8B8A8_SNORM },
    { eResourceFormat::Format_R8G8B8A8_SINT, DXGI_FORMAT_R8G8B8A8_SINT },
    { eResourceFormat::Format_R16G16_TYPELESS, DXGI_FORMAT_R16G16_TYPELESS },
    { eResourceFormat::Format_R16G16_FLOAT, DXGI_FORMAT_R16G16_FLOAT },
    { eResourceFormat::Format_R16G16_UNORM, DXGI_FORMAT_R16G16_UNORM },
    { eResourceFormat::Format_R16G16_UINT, DXGI_FORMAT_R16G16_UINT },
    { eResourceFormat::Format_R16G16_SNORM, DXGI_FORMAT_R16G16_SNORM },
    { eResourceFormat::Format_R16G16_SINT, DXGI_FORMAT_R16G16_SINT },
    { eResourceFormat::Format_R32_TYPELESS, DXGI_FORMAT_R32_TYPELESS },
    { eResourceFormat::Format_D32_FLOAT, DXGI_FORMAT_D32_FLOAT },
    { eResourceFormat::Format_R32_FLOAT, DXGI_FORMAT_R32_FLOAT },
    { eResourceFormat::Format_R32_UINT, DXGI_FORMAT_R32_UINT },
    { eResourceFormat::Format_R32_SINT, DXGI_FORMAT_R32_SINT },
    { eResourceFormat::Format_R24G8_TYPELESS, DXGI_FORMAT_R24G8_TYPELESS },
    { eResourceFormat::Format_D24_UNORM_S8_UINT, DXGI_FORMAT_D24_UNORM_S8_UINT },
    { eResourceFormat::Format_R24_UNORM_X8_TYPELESS, DXGI_FORMAT_R24_UNORM_X8_TYPELESS },
    { eResourceFormat::Format_X24_TYPELESS_G8_UINT, DXGI_FORMAT_X24_TYPELESS_G8_UINT },
    { eResourceFormat::Format_R8G8_TYPELESS, DXGI_FORMAT_R8G8_TYPELESS },
    { eResourceFormat::Format_R8G8_UNORM, DXGI_FORMAT_R8G8_UNORM },
    { eResourceFormat::Format_R8G8_UINT, DXGI_FORMAT_R8G8_UINT },
    { eResourceFormat::Format_R8G8_SNORM, DXGI_FORMAT_R8G8_SNORM },
    { eResourceFormat::Format_R8G8_SINT, DXGI_FORMAT_R8G8_SINT },
    { eResourceFormat::Format_R16_TYPELESS, DXGI_FORMAT_R16_TYPELESS },
    { eResourceFormat::Format_R16_FLOAT, DXGI_FORMAT_R16_FLOAT },
    { eResourceFormat::Format_D16_UNORM, DXGI_FORMAT_D16_UNORM },
    { eResourceFormat::Format_R16_UNORM, DXGI_FORMAT_R16_UNORM },
    { eResourceFormat::Format_R16_UINT, DXGI_FORMAT_R16_UINT },
    { eResourceFormat::Format_R16_SNORM,DXGI_FORMAT_R16_SNORM },
    { eResourceFormat::Format_R16_SINT, DXGI_FORMAT_R16_SINT },
    { eResourceFormat::Format_R8_TYPELESS, DXGI_FORMAT_R8_TYPELESS },
    { eResourceFormat::Format_R8_UNORM, DXGI_FORMAT_R8_UNORM },
    { eResourceFormat::Format_R8_UINT, DXGI_FORMAT_R8_UINT },
    { eResourceFormat::Format_R8_SNORM, DXGI_FORMAT_R8_SNORM },
    { eResourceFormat::Format_R8_SINT, DXGI_FORMAT_R8_SINT },
    { eResourceFormat::Format_A8_UNORM, DXGI_FORMAT_A8_UNORM },
    { eResourceFormat::Format_R1_UNORM, DXGI_FORMAT_R1_UNORM },
    { eResourceFormat::Format_R9G9B9E5_SHAREDEXP, DXGI_FORMAT_R9G9B9E5_SHAREDEXP },
    { eResourceFormat::Format_R8G8_B8G8_UNORM, DXGI_FORMAT_R8G8_B8G8_UNORM },
    { eResourceFormat::Format_G8R8_G8B8_UNORM, DXGI_FORMAT_G8R8_G8B8_UNORM },
    { eResourceFormat::Format_BC1_TYPELESS, DXGI_FORMAT_BC1_TYPELESS },
    { eResourceFormat::Format_BC1_UNORM, DXGI_FORMAT_BC1_UNORM },
    { eResourceFormat::Format_BC1_UNORM_SRGB, DXGI_FORMAT_BC1_UNORM_SRGB },
    { eResourceFormat::Format_BC2_TYPELESS, DXGI_FORMAT_BC2_TYPELESS },
    { eResourceFormat::Format_BC2_UNORM, DXGI_FORMAT_BC2_UNORM },
    { eResourceFormat::Format_BC2_UNORM_SRGB, DXGI_FORMAT_BC2_UNORM_SRGB },
    { eResourceFormat::Format_BC3_TYPELESS, DXGI_FORMAT_BC3_TYPELESS },
    { eResourceFormat::Format_BC3_UNORM, DXGI_FORMAT_BC3_UNORM },
    { eResourceFormat::Format_BC3_UNORM_SRGB, DXGI_FORMAT_BC3_UNORM_SRGB },
    { eResourceFormat::Format_BC4_TYPELESS, DXGI_FORMAT_BC4_TYPELESS },
    { eResourceFormat::Format_BC4_UNORM, DXGI_FORMAT_BC4_UNORM },
    { eResourceFormat::Format_BC4_SNORM, DXGI_FORMAT_BC4_SNORM },
    { eResourceFormat::Format_BC5_TYPELESS, DXGI_FORMAT_BC5_TYPELESS },
    { eResourceFormat::Format_BC5_UNORM, DXGI_FORMAT_BC5_UNORM },
    { eResourceFormat::Format_BC5_SNORM, DXGI_FORMAT_BC5_SNORM },
    { eResourceFormat::Format_B5G6R5_UNORM, DXGI_FORMAT_B5G6R5_UNORM },
    { eResourceFormat::Format_B5G5R5A1_UNORM, DXGI_FORMAT_B5G5R5A1_UNORM },
    { eResourceFormat::Format_B8G8R8A8_UNORM, DXGI_FORMAT_B8G8R8A8_UNORM },
    { eResourceFormat::Format_B8G8R8X8_UNORM, DXGI_FORMAT_B8G8R8X8_UNORM },
    { eResourceFormat::Format_R10G10B10_XR_BIAS_A2_UNORM, DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM },
    { eResourceFormat::Format_B8G8R8A8_TYPELESS, DXGI_FORMAT_B8G8R8A8_TYPELESS },
    { eResourceFormat::Format_B8G8R8A8_UNORM_SRGB, DXGI_FORMAT_B8G8R8A8_UNORM_SRGB },
    { eResourceFormat::Format_B8G8R8X8_TYPELESS, DXGI_FORMAT_B8G8R8X8_TYPELESS },
    { eResourceFormat::Format_B8G8R8X8_UNORM_SRGB, DXGI_FORMAT_B8G8R8X8_UNORM_SRGB },
    { eResourceFormat::Format_BC6H_TYPELESS, DXGI_FORMAT_BC6H_TYPELESS },
    { eResourceFormat::Format_BC6H_UF16, DXGI_FORMAT_BC6H_UF16 },
    { eResourceFormat::Format_BC6H_SF16, DXGI_FORMAT_BC6H_SF16 },
    { eResourceFormat::Format_BC7_TYPELESS, DXGI_FORMAT_BC7_TYPELESS },
    { eResourceFormat::Format_BC7_UNORM, DXGI_FORMAT_BC7_UNORM },
    { eResourceFormat::Format_BC7_UNORM_SRGB, DXGI_FORMAT_BC7_UNORM_SRGB },
    { eResourceFormat::Format_AYUV, DXGI_FORMAT_AYUV },
    { eResourceFormat::Format_Y410, DXGI_FORMAT_Y410 },
    { eResourceFormat::Format_Y416, DXGI_FORMAT_Y416 },
    { eResourceFormat::Format_NV12, DXGI_FORMAT_NV12 },
    { eResourceFormat::Format_P010, DXGI_FORMAT_P010 },
    { eResourceFormat::Format_P016, DXGI_FORMAT_P016 },
    { eResourceFormat::Format_420_OPAQUE, DXGI_FORMAT_420_OPAQUE },
    { eResourceFormat::Format_YUY2, DXGI_FORMAT_YUY2 },
    { eResourceFormat::Format_Y210, DXGI_FORMAT_Y210 },
    { eResourceFormat::Format_Y216, DXGI_FORMAT_Y216 },
    { eResourceFormat::Format_NV11, DXGI_FORMAT_NV11 },
    { eResourceFormat::Format_AI44, DXGI_FORMAT_AI44 },
    { eResourceFormat::Format_IA44, DXGI_FORMAT_IA44 },
    { eResourceFormat::Format_P8, DXGI_FORMAT_P8 },
    { eResourceFormat::Format_A8P8,DXGI_FORMAT_A8P8 },
    { eResourceFormat::Format_B4G4R4A4_UNORM, DXGI_FORMAT_B4G4R4A4_UNORM },
    { eResourceFormat::Format_P208, DXGI_FORMAT_P208 },
    { eResourceFormat::Format_V208, DXGI_FORMAT_V208 },
    { eResourceFormat::Format_V408, DXGI_FORMAT_V408 },
    { eResourceFormat::Format_FORCE_UINT, DXGI_FORMAT_FORCE_UINT }
};
inline std::map<eResourceDim, D3D12_RESOURCE_DIMENSION> ResourceDimensions
{
    { eResourceDim::Texture2D, D3D12_RESOURCE_DIMENSION_TEXTURE2D }
};
inline std::map<eResourceFlags, D3D12_RESOURCE_FLAGS> ResourceFlags
{
     { eResourceFlags::None, D3D12_RESOURCE_FLAG_NONE },
     { eResourceFlags::AllowRenderTarget,  D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET },
     { eResourceFlags::AllowDepthStencil, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL },
     { eResourceFlags::AllowUnorderedAccess, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS },
     { eResourceFlags::DenyShaderResource, D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE },
     { eResourceFlags::AllowCrossAdapter, D3D12_RESOURCE_FLAG_ALLOW_CROSS_ADAPTER },
     { eResourceFlags::AllowSimultaneousAccess, D3D12_RESOURCE_FLAG_ALLOW_SIMULTANEOUS_ACCESS }
};
inline std::map<eResourceState, D3D12_RESOURCE_STATES> ResourceStates
{
    { eResourceState::Common, D3D12_RESOURCE_STATE_COMMON },
    { eResourceState::VertexAndConstantBuffer, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER },
    { eResourceState::IndexBuffer, D3D12_RESOURCE_STATE_INDEX_BUFFER },
    { eResourceState::RenderTarget, D3D12_RESOURCE_STATE_RENDER_TARGET },
    { eResourceState::UnorderedAccess, D3D12_RESOURCE_STATE_UNORDERED_ACCESS },
    { eResourceState::DepthWrite, D3D12_RESOURCE_STATE_DEPTH_WRITE },
    { eResourceState::DepthRead, D3D12_RESOURCE_STATE_DEPTH_READ },
    { eResourceState::NonPixelShaderResource, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE },
    { eResourceState::PixelShaderResource, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE },
    { eResourceState::StreamOut, D3D12_RESOURCE_STATE_STREAM_OUT },
    { eResourceState::IndirectArgument, D3D12_RESOURCE_STATE_INDIRECT_ARGUMENT },
    { eResourceState::CopyDest, D3D12_RESOURCE_STATE_COPY_DEST },
    { eResourceState::CopySource, D3D12_RESOURCE_STATE_COPY_SOURCE },
    { eResourceState::ResolveDest, D3D12_RESOURCE_STATE_RESOLVE_DEST },
    { eResourceState::ResolveSource, D3D12_RESOURCE_STATE_RESOLVE_SOURCE },
    { eResourceState::RaytracingAccelerationStructure, D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE },
    { eResourceState::ShadingRateSource, D3D12_RESOURCE_STATE_SHADING_RATE_SOURCE },
    { eResourceState::GenericRead, D3D12_RESOURCE_STATE_GENERIC_READ },
    { eResourceState::Present, D3D12_RESOURCE_STATE_PRESENT },
    { eResourceState::Predication, D3D12_RESOURCE_STATE_PREDICATION }
};
}