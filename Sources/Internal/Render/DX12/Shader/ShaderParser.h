#pragma once

#include <string>
#include <vector>

#include "Render/ShaderData.h"

namespace Kioto::Renderer::ShaderParser
{
Renderer::ShaderData ParseShader(const std::string& path, const std::vector<ShaderDefine>* const defines);
Renderer::ShaderData ParseShaderFromString(std::string source, const std::vector<ShaderDefine>* const defines);
}