//
// Copyright (C) Alexandr Vorontsov. 2017
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#pragma once

#include <string>

#include "AssetsSystem/Asset.h"
#include "Core/CoreTypes.h"
#include "Render/PipelineState.h"
#include "Render/Shader.h"

namespace Kioto
{
class Material : public Asset
{
public:
    Material() = default;
    Material(const std::string& path);
    ~Material() = default;

private:
    std::string m_shaderPath;
    PipelineState m_pipelineState;
    Renderer::Shader* m_shader = nullptr;
};
}