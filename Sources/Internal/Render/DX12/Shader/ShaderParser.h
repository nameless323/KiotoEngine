//
// Copyright (C) Alexandr Vorontsov. 2018
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#pragma once

#include <string>
#include <vector>

#include "Render/ConstantBuffer.h"
#include "Render/VertexLayout.h"
#include "Render/PipelineState.h"
#include "Render/RendererPublic.h"

namespace Kioto::Renderer
{
namespace ShaderParser
{
struct ParseResult
{
    std::string output;
    VertexLayout vertexLayout;
    PipelineState pipelineState;
    std::vector<ConstantBuffer> constantBuffers;
};

ParseResult ParseShader(const std::string& path, const std::vector<ShaderDefine>* const defines);
}
}