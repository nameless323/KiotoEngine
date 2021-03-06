#pragma once

#include "Render/ConstantBuffer.h"
#include "Render/VertexLayout.h"
#include "Render/PipelineState.h"
#include "Render/RendererPublic.h"
#include "Render/UniformConstant.h"
#include "Render/Texture/TextureSet.h"

namespace Kioto::Renderer
{
enum class ShaderProgramType
{
    Vertex = 1 << 0,
    Fragment = 1 << 1
};

using RenderObjectBufferLayout = std::vector<ConstantBuffer>;
using RenderObjectConstants = std::vector<UniformConstant>;

struct ShaderData
{
    std::string output;
    std::string shaderPath;
    VertexLayout vertexLayout;
    TextureSet textureSet;
    uint8 shaderPrograms = 0;
};

using ShaderDataAndBufferLayout = std::pair<ShaderData, RenderObjectBufferLayout>;
}