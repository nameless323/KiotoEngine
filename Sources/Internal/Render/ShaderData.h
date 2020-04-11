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

using ShaderBufferLayoutTemplate = std::vector<ConstantBuffer>;

struct ShaderData
{
    std::string output;
    VertexLayout vertexLayout;
    PipelineState pipelineState;
    TextureSet textureSet;
    uint8 shaderPrograms = 0;
};

using ShaderDataAndBufferLayout = std::pair<ShaderData, ShaderBufferLayoutTemplate>;

struct RenderObjectBufferLayout
{
    std::vector<ConstantBuffer> constantBuffers;
    ConstantBufferSet bufferSet;
    ConstantBufferSetHandle bufferSetHandle;
};
}