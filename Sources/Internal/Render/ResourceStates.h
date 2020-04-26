#pragma once

namespace Kioto::Renderer
{
enum class eResourceState
{
    Common,
    VertexAndConstantBuffer,
    IndexBuffer,
    RenderTarget,
    UnorderedAccess,
    DepthWrite,
    DepthRead,
    NonPixelShaderResource,
    PixelShaderResource,
    StreamOut,
    IndirectArgument,
    CopyDest,
    CopySource,
    ResolveDest,
    ResolveSource,
    RaytracingAccelerationStructure,
    ShadingRateSource,
    GenericRead
};
}