//
// Copyright (C) Alexandr Vorontsov. 2018
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#pragma once

#include <string>
#include <vector>

#include "Render/ShaderData.h"

namespace Kioto::Renderer
{
namespace ShaderParser
{
Renderer::ShaderData ParseShader(const std::string& path, const std::vector<ShaderDefine>* const defines);
Renderer::ShaderData ParseShaderFromString(std::string source, const std::vector<ShaderDefine>* const defines);
}
}