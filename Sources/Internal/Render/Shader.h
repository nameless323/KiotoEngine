//
// Copyright (C) Alexandr Vorontsov. 2017
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#pragma once

#include <string>

#include "Render/RendererPublic.h"

namespace Kioto::Renderer
{
class Shader
{
private:
    std::string m_path;
    std::string m_shader;
    Handle m_vsHandle;
    Handle m_psHandle;

    friend class Material;
};
}