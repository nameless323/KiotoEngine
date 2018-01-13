//
// Copyright (C) Alexandr Vorontsov. 2018
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#pragma once

#include <string>
#include <vector>

#include "Render/ConstantBuffer.h"
#include "Render/VertexLayout.h"

namespace Kioto::Renderer
{
namespace ShaderPreprocessorDX12
{
struct ParseResult
{
    std::string output;
    VertexLayout vertexLayout;
    std::vector<ConstantBuffer> constantBuffer;
};

ParseResult ParseShader(const std::string& path);
}
}