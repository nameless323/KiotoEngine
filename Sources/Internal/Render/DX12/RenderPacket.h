//
// Copyright (C) Alexandr Vorontsov 2017.
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#pragma once

#include "Render/RendererPublic.h"

namespace Kioto::Renderer
{
class RenderPacket
{
public:
    ShaderProgramHandle VertexShader;
    ShaderProgramHandle FragmentShader;
    VertexLayoutHandle VertexLayout;
};
}