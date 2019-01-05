//
// Copyright (C) Aleksandr Vorontcov. 2018
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#pragma once

#include "Core/CoreTypes.h"
#include "Render/PipelineStateParams.h"
#include "Render/RenderLayer.h"

namespace YAML
{
class Node;
}

namespace Kioto::Renderer
{
struct PipelineState
{
    eRenderLayerType LayerType = eRenderLayerType::Opaque;
    eFillMode Fill = eFillMode::Solid;
    eCullMode Cull = eCullMode::Back;
    eZTest Ztest = eZTest::LEqual;
    StencilDesc FrontFaceStencilDesc;
    StencilDesc BackFaceStencilDesc;
    uint8 StencilWriteMask = 0xF;
    uint8 StencilReadMask = 0xF;
    eBlendModes SrcBlend = eBlendModes::Zero;
    eBlendModes DstBlend = eBlendModes::One;
    eBlendOps BlendOp = eBlendOps::Add;
    eColorMask ColorMask = eColorMask::All;
    bool Zwrite = true;
    bool EnableStencill = false;
    bool EnableDepth = false;
    bool WindingCCW = true;

    static PipelineState FromYaml(const YAML::Node& config);
    static PipelineState FromYaml(const std::string& config);
    static void Append(const YAML::Node& node, PipelineState& initConfig);
};
}