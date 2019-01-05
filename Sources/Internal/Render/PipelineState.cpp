//
// Copyright (C) Aleksandr Vorontcov. 2018
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#include "stdafx.h"

#include "yaml-cpp/yaml.h"

#include "AssetsSystem/RenderStateParamsConverter.h"
#include "Core/CoreTypes.h"
#include "Render/PipelineState.h"

namespace Kioto::Renderer
{

PipelineState PipelineState::FromYaml(const YAML::Node& config)
{
    PipelineState pipelineState;
    Append(config, pipelineState);
    return pipelineState;
}

PipelineState PipelineState::FromYaml(const std::string& config)
{
    return FromYaml(YAML::Load(config));
}

void PipelineState::Append(const YAML::Node& node, PipelineState& srcCfg)
{
    using namespace RenderParamsConverter;
    if (node["renderLayer"] != nullptr)
        srcCfg.LayerType = FromString<eRenderLayerType>(node["renderLayer"].as<std::string>());
    if (node["fill"] != nullptr)
        srcCfg.Fill = FromString<eFillMode>(node["fill"].as<std::string>());
    if (node["cull"] != nullptr)
        srcCfg.Cull = FromString<eCullMode>(node["cull"].as<std::string>());
    if (node["windingCCW"] != nullptr)
        srcCfg.WindingCCW = node["windingCCW"].as<bool>();
    if (node["depthStencil"] != nullptr)
    {
        YAML::Node dsNode = node["depthStencil"];
        if (dsNode["enableDepth"] != nullptr)
            srcCfg.EnableDepth = dsNode["enableDepth"].as<bool>();
        if (dsNode["enableStencil"] != nullptr)
            srcCfg.EnableStencill = dsNode["enableStencil"].as<bool>();
        if (dsNode["ZTest"] != nullptr)
            srcCfg.Ztest = FromString<eZTest>(dsNode["ZTest"].as<std::string>());
        if (dsNode["ZWrite"] != nullptr)
            srcCfg.Zwrite = dsNode["ZWrite"].as<bool>();
        if (dsNode["writeMask"] != nullptr)
            srcCfg.StencilWriteMask = static_cast<uint8>(std::stoul(dsNode["writeMask"].as<std::string>(), 0, 0));
        if (dsNode["readMask"] != nullptr)
            srcCfg.StencilReadMask = static_cast<uint8>(std::stoul(dsNode["readMask"].as<std::string>(), 0, 0));

        if (dsNode["stencilFront"] != nullptr)
        {
            YAML::Node frontNode = dsNode["stencilFront"];
            if (frontNode["fail"] != nullptr)
                srcCfg.FrontFaceStencilDesc.StencilFailOp = FromString<eStencilOp>(frontNode["fail"].as<std::string>());
            if (frontNode["ZFfail"] != nullptr)
                srcCfg.FrontFaceStencilDesc.StencilDepthFailOp = FromString<eStencilOp>(frontNode["ZFfail"].as<std::string>());
            if (frontNode["pass"] != nullptr)
                srcCfg.FrontFaceStencilDesc.StencilPassOp = FromString<eStencilOp>(frontNode["pass"].as<std::string>());
            if (frontNode["func"] != nullptr)
                srcCfg.FrontFaceStencilDesc.StencilFunc = FromString<eStencilTest>(frontNode["func"].as<std::string>());
        }
        if (dsNode["stencilBack"] != nullptr)
        {
            YAML::Node backNode = dsNode["stencilBack"];
            if (backNode["fail"] != nullptr)
                srcCfg.BackFaceStencilDesc.StencilFailOp = FromString<eStencilOp>(backNode["fail"].as<std::string>());
            if (backNode["ZFfail"] != nullptr)
                srcCfg.BackFaceStencilDesc.StencilDepthFailOp = FromString<eStencilOp>(backNode["ZFfail"].as<std::string>());
            if (backNode["pass"] != nullptr)
                srcCfg.BackFaceStencilDesc.StencilPassOp = FromString<eStencilOp>(backNode["pass"].as<std::string>());
            if (backNode["func"] != nullptr)
                srcCfg.BackFaceStencilDesc.StencilFunc = FromString<eStencilTest>(backNode["func"].as<std::string>());
        }
    }
    if (node["blending"] != nullptr)
    {
        YAML::Node bNode = node["blending"];
        if (bNode["blendOp"] != nullptr)
            srcCfg.BlendOp = FromString<eBlendOps>(bNode["blendOp"].as<std::string>());
        if (bNode["srcBlend"] != nullptr)
            srcCfg.SrcBlend = FromString<eBlendModes>(bNode["srcBlend"].as<std::string>());
        if (bNode["dstBlend"] != nullptr)
            srcCfg.DstBlend = FromString<eBlendModes>(bNode["dstBlend"].as<std::string>());
    }
    if (node["colorMask"] != nullptr)
        srcCfg.ColorMask = FromString<eColorMask>(node["colorMask"].as<std::string>());
}

}