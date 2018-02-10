//
// Copyright (C) Alexandr Vorontsov. 2017
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#pragma once

#include <string>

#include "AssetsSystem/Asset.h"
#include "AssetsSystem/AssetsSystem.h"
#include "Render/RendererPublic.h"
#include "Render/DX12/Shader/ShaderParser.h"

namespace Kioto::Renderer
{
class Shader : public Asset
{
public:
    Shader() : Asset("") {}
    Shader(const std::string& path) : Asset(path) 
    {
        m_shader = AssetsSystem::ReadFileAsString(path);
        m_data = ShaderParser::ParseShaderFromString(m_shader, nullptr);
    }

private:
    std::string m_shader;
    ShaderHandle m_vsHandle;
    ShaderHandle m_psHandle;
    VertexLayoutHandle m_vertexLayout;
    ShaderData m_data;

    friend class Material;
};
}