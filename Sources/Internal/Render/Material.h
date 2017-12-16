//
// Copyright (C) Alexandr Vorontsov. 2017
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#pragma once

#include <string>

#include "Core/CoreTypes.h"
#include "Render/PipelineStateParams.h"
#include "Render/RenderLayer.h"
#include "Render/Shader.h"

namespace Kioto
{
class Material
{
public:
    Material() = default;
    Material(const std::wstring& path);
    ~Material() = default;

private:
    std::string m_shaderPath;
    Renderer::Shader shader;
    eRenderLayerType m_layerType = eRenderLayerType::Opaque;
    eFillMode m_fill = eFillMode::Solid;
    eCullMode m_cull = eCullMode::Back;
    eZTest m_ztest = eZTest::LEqual;
    StencilDesc m_frontFaceStencilDesc;
    StencilDesc m_backFaceStencilDesc;
    uint8 m_stencilWriteMask = 0xF;
    uint8 m_stencilReadMask = 0xF;
    eBlendModes m_srcBlend = eBlendModes::Zero;
    eBlendModes m_dstBlend = eBlendModes::One;
    eBlendOps m_blendOp = eBlendOps::Add;
    eColorMask m_colorMask = eColorMask::All;
    bool m_zwrite = true;
    bool m_enableStencill = false;
    bool m_windingCCW = true;
};
}