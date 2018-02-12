//
// Copyright (C) Alexandr Vorontsov. 2018
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#pragma once

#include <map>

#include "Render/RendererPublic.h"
#include "Render/DX12/ShaderDX12.h"

namespace Kioto::Renderer
{
class ShaderManagerDX12
{
public:
    void RegisterShader(Shader* shader);

private:
    std::map<ShaderHandle, std::vector<ShaderDX12>> m_shaders;
};
}