//
// Copyright (C) Alexandr Vorontsov. 2017
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#pragma once

#include <string>

#include "Core/CoreTypes.h"
#include "Render/PipelineStateParams.h"
#include "Render/RenderLayer.h"

namespace Kioto
{
class Material
{
public:
    Material() = default;
    Material(const std::wstring& path);
    ~Material() = default;

private:
    std::wstring m_shaderPath;
    RenderLayerType m_layerType = RenderLayerType::Opaque;
    FillMode m_fill = FillMode::Solid;
    CullMode m_cull = CullMode::Back;
    bool m_windingCCW = true;
    bool m_enableStencill = false;
    ZTest m_ztest = ZTest::LEqual;
    bool m_zwrite = true;
    StencilDesc m_frontFaceStencilDesc;
    StencilDesc m_backFaceStencilDesc;
    uint8 m_stencilWriteMask = 0xFF;
    uint8 m_stencilReadMask = 0xFF;
};
}