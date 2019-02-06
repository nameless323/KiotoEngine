//
// Copyright (C) Aleksandr Vorontcov 2017.
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

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