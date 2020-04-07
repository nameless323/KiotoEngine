#pragma once

#include <vector>

#include "Render/RendererPublic.h"

namespace Kioto::Renderer
{
class RenderPacket
{
public:
    MaterialHandle Material;
    RenderPassHandle Pass;
    ShaderHandle Shader;
    VertexLayoutHandle VertexLayout;
    TextureSetHandle TextureSet;
    MeshHandle Mesh;
    ConstantBufferSetHandle CBSet;
};

using RenderPacketList = std::vector<RenderPacket>;
}