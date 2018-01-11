//
// Copyright (C) Alexandr Vorontsov. 2018
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#pragma once

#include <string>

namespace Kioto::Renderer
{
namespace ShaderPreprocessorDX12
{
struct ParseResult
{
    std::string output;
};

ParseResult ParseShader(const std::string& path);
}
}