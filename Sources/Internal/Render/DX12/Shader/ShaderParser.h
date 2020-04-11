#pragma once

#include <string>
#include <vector>

#include "Render/ShaderData.h"

namespace Kioto::Renderer::ShaderParser
{
ShaderDataAndBufferLayout ParseShader(const std::string& path, const std::vector<ShaderDefine>* const defines);
ShaderDataAndBufferLayout ParseShaderFromString(std::string source, const std::vector<ShaderDefine>* const defines);
}