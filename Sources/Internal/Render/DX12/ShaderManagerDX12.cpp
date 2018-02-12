//
// Copyright (C) Alexandr Vorontsov. 2018
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#include "stdafx.h"

#include "Render/DX12/ShaderManagerDX12.h"

#include "Render/Shader.h"

namespace Kioto::Renderer
{

void ShaderManagerDX12::RegisterShader(Shader* shader)
{
    auto it = m_shaders.find(shader->GetHandle());
    if (it == m_shaders.cend())
        return;
    shader->SetHandle(GetNewHandle());
    shader->SetShaderData(ShaderParser::ParseShaderFromString(shader->GetShaderString(), nullptr));
    if (shader->GetShaderData().shaderPrograms & uint8(ShaderProgramType::Vertex))
    {
        ShaderDX12 vs;
    }
}

}