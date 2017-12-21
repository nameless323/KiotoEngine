//
// Copyright (C) Alexandr Vorontsov. 2017
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#pragma once

#include <string>

#include "AssetsSystem/Asset.h"
#include "Render/RendererPublic.h"

namespace Kioto::Renderer
{
class Shader : public Asset
{
public:
    Shader() : Asset("") {}
    Shader(const std::string& path) : Asset(path) {}

private:
    std::string m_path;
    std::string m_shader;
    ShaderHandle m_vsHandle;
    ShaderHandle m_psHandle;

    friend class Material;
};
}