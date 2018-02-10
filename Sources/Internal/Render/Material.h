//
// Copyright (C) Alexandr Vorontsov. 2017
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#pragma once

#include <string>

#include "Render/Texture/TextureSet.h"
#include "Render/ConstantBuffer.h"
#include "AssetsSystem/Asset.h"
#include "Core/CoreTypes.h"
#include "Render/PipelineState.h"
#include "Render/Shader.h"
#include "Render/RendererPublic.h"

namespace Kioto::Renderer
{
class Material : public Asset
{
public:
    Material() = default;
    Material(const std::string& path);
    ~Material();

    void SetHandle(MaterialHandle handle);
    MaterialHandle GetHandle() const;

private:
    std::string m_shaderPath;
    PipelineState m_pipelineState;
    Shader* m_shader = nullptr;
    TextureSet m_textureSet;
    std::vector<ConstantBuffer> m_constantBuffers;
    MaterialHandle m_handle;
};

inline void Material::SetHandle(MaterialHandle handle)
{
    m_handle = handle;
}

inline MaterialHandle Material::GetHandle() const
{
    return m_handle;
}
}