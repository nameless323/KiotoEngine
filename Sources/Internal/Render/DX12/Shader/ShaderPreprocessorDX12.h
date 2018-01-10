//
// Copyright (C) Alexandr Vorontsov. 2017
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#pragma once

#include <string>

namespace Kioto::Renderer
{
class ShaderPreprocessorDX12
{
public:
    static std::string UnfoldIncludes(std::string source)
    {
        size_t includePos = source.find("#include", 0);
        size_t includeFin = source.find("\n", includePos);
        if (includePos != std::string::npos && includeFin != std::string::npos)
        {
            std::string s = source.substr(includePos, includeFin - includePos);
            source.erase(includePos, includeFin - includePos);

            std::string p;
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
                p += s[i];
            }
            p.insert(0, "Shaders\\");
            std::wstring shaderPath = AssetsSystem::GetAssetFullPath(StrToWstr(p));
            bool iisExist = AssetsSystem::CheckIfFileExist(shaderPath);
            std::string incl = AssetsSystem::ReadFileAsString(std::string(shaderPath.begin(), shaderPath.end()));

            source.insert(0, UnfoldIncludes(incl));
        }
        else
            return source;

        return source;
    }

private:
};
}