//
// Copyright (C) Alexandr Vorontsov. 2018
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#include "stdafx.h"

#include "Render/DX12/Shader/ShaderPreprocessorDX12.h"

#include "AssetsSystem/AssetsSystem.h"

namespace Kioto::Renderer
{
namespace ShaderPreprocessorDX12
{

std::vector<std::string> m_preprocessedHeaders;
static constexpr uint16 m_maxDepth = 128;

std::string UnfoldIncludes(std::string source, uint16 recursionDepth)
{
    if (recursionDepth > m_maxDepth)
        return "";
    size_t includePos = source.find("#include", 0);
    size_t includeFin = source.find("\n", includePos);
    while (includePos != std::string::npos)
    {
        if (includeFin != std::string::npos)
        {
            std::string s = source.substr(includePos, includeFin - includePos);
            source.erase(includePos, includeFin - includePos);

            std::string relativeIncludePath;
            bool startFound = false;
            for (int i = 8; i < s.size(); ++i)
            {
                if (s[i] == '\"' && !startFound)
                {
                    startFound = true;
                    continue;
                }
                else if (!startFound)
                    continue;
                else if (s[i] == '\"')
                    break;
                relativeIncludePath += s[i];
            }
            relativeIncludePath.insert(0, "Shaders\\");

            auto it = std::find(m_preprocessedHeaders.begin(), m_preprocessedHeaders.end(), relativeIncludePath);
            if (it == m_preprocessedHeaders.end())
            {
                std::wstring shaderPath = AssetsSystem::GetAssetFullPath(StrToWstr(relativeIncludePath));
                bool isExist = AssetsSystem::CheckIfFileExist(shaderPath);
                std::string incl = AssetsSystem::ReadFileAsString(std::string(shaderPath.begin(), shaderPath.end()));
                source.insert(0, UnfoldIncludes(incl, recursionDepth + 1));
                m_preprocessedHeaders.push_back(relativeIncludePath);
            }
        }

        includePos = source.find("#include", includePos);
        includeFin = source.find("\n", includePos);
    }

    return source;
}

ParseResult ParseShader(const std::string& path)
{
    ParseResult res;
    m_preprocessedHeaders.clear();
    std::string shaderStr = AssetsSystem::ReadFileAsString(path);
    shaderStr = ShaderPreprocessorDX12::UnfoldIncludes(shaderStr, 0);
    res.output = shaderStr;
    return res;
}

}
}