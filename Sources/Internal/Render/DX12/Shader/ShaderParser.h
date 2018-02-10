//
// Copyright (C) Alexandr Vorontsov. 2018
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#pragma once

#include <string>
#include <vector>

#include "Render/MaterialData.h"

namespace Kioto::Renderer
{
namespace ShaderParser
{
Renderer::MaterialData ParseShader(const std::string& path, const std::vector<ShaderDefine>* const defines);
Renderer::MaterialData ParseShaderFromString(std::string source, const std::vector<ShaderDefine>* const defines);
}
}