//
// Copyright (C) Alexandr Vorontsov. 2018
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#pragma once

#include "Render/ConstantBuffer.h"
#include "Render/VertexLayout.h"
#include "Render/PipelineState.h"
#include "Render/RendererPublic.h"
#include "Render/Texture/TextureSet.h"

namespace Kioto::Renderer
{
enum class ShaderProgramType
{
    Vertex = 1 << 0,
    Fragment = 1 << 1
};

struct ShaderData
{
    std::string output;
    VertexLayout vertexLayout;
    PipelineState pipelineState;
    TextureSet textureSet;
    std::vector<ConstantBuffer> constantBuffers;
    ConstantBufferSet bufferSet;
    uint8 shaderPrograms = 0;
};
}