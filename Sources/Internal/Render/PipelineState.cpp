#include "stdafx.h"

#include "yaml-cpp/yaml.h"

#include "AssetsSystem/RenderStateParamsConverter.h"
#include "Core/CoreTypes.h"
#include "Render/PipelineState.h"

namespace Kioto::Renderer
{
void PipelineState::FromYaml(const YAML::Node& config, PipelineState& dstConfig)
{
    if (config["parent"])
        FromYaml(YAML::Load(config["parent"].as<std::string>()), dstConfig);
    Append(config, dstConfig);
}

PipelineState PipelineState::FromYaml(const std::string& config)
{
    PipelineState pipelineState;
    FromYaml(YAML::Load(config), pipelineState);
    return pipelineState;
}

void PipelineState::Append(const YAML::Node& node, PipelineState& srcCfg)
{
    assert(srcCfg.Shader == nullptr);

    using namespace RenderParamsConverter;
    if (node["renderLayer"])
        srcCfg.LayerType = FromString<eRenderLayerType>(node["renderLayer"].as<std::string>());
    if (node["fill"])
        srcCfg.Fill = FromString<eFillMode>(node["fill"].as<std::string>());
    if (node["cull"])
        srcCfg.Cull = FromString<eCullMode>(node["cull"].as<std::string>());
    if (node["windingCCW"])
        srcCfg.WindingCCW = node["windingCCW"].as<bool>();
    if (node["depthStencil"])
    {
        YAML::Node dsNode = node["depthStencil"];
        if (dsNode["enableDepth"])
            srcCfg.EnableDepth = dsNode["enableDepth"].as<bool>();
        if (dsNode["enableStencil"])
            srcCfg.EnableStencill = dsNode["enableStencil"].as<bool>();
        if (dsNode["ZTest"])
            srcCfg.Ztest = FromString<eZTest>(dsNode["ZTest"].as<std::string>());
        if (dsNode["ZWrite"])
            srcCfg.Zwrite = dsNode["ZWrite"].as<bool>();
        if (dsNode["writeMask"])
            srcCfg.StencilWriteMask = static_cast<uint8>(std::stoul(dsNode["writeMask"].as<std::string>(), 0, 0));
        if (dsNode["readMask"])
            srcCfg.StencilReadMask = static_cast<uint8>(std::stoul(dsNode["readMask"].as<std::string>(), 0, 0));

        if (dsNode["stencilFront"])
        {
            YAML::Node frontNode = dsNode["stencilFront"];
            if (frontNode["fail"])
                srcCfg.FrontFaceStencilDesc.StencilFailOp = FromString<eStencilOp>(frontNode["fail"].as<std::string>());
            if (frontNode["ZFfail"])
                srcCfg.FrontFaceStencilDesc.StencilDepthFailOp = FromString<eStencilOp>(frontNode["ZFfail"].as<std::string>());
            if (frontNode["pass"])
                srcCfg.FrontFaceStencilDesc.StencilPassOp = FromString<eStencilOp>(frontNode["pass"].as<std::string>());
            if (frontNode["func"])
                srcCfg.FrontFaceStencilDesc.StencilFunc = FromString<eStencilTest>(frontNode["func"].as<std::string>());
        }
        if (dsNode["stencilBack"])
        {
            YAML::Node backNode = dsNode["stencilBack"];
            if (backNode["fail"])
                srcCfg.BackFaceStencilDesc.StencilFailOp = FromString<eStencilOp>(backNode["fail"].as<std::string>());
            if (backNode["ZFfail"])
                srcCfg.BackFaceStencilDesc.StencilDepthFailOp = FromString<eStencilOp>(backNode["ZFfail"].as<std::string>());
            if (backNode["pass"])
                srcCfg.BackFaceStencilDesc.StencilPassOp = FromString<eStencilOp>(backNode["pass"].as<std::string>());
            if (backNode["func"])
                srcCfg.BackFaceStencilDesc.StencilFunc = FromString<eStencilTest>(backNode["func"].as<std::string>());
        }
    }
    if (node["blending"])
    {
        YAML::Node bNode = node["blending"];
        if (bNode["blendOp"])
            srcCfg.BlendOp = FromString<eBlendOps>(bNode["blendOp"].as<std::string>());
        if (bNode["srcBlend"])
            srcCfg.SrcBlend = FromString<eBlendModes>(bNode["srcBlend"].as<std::string>());
        if (bNode["dstBlend"])
            srcCfg.DstBlend = FromString<eBlendModes>(bNode["dstBlend"].as<std::string>());
    }
    if (node["colorMask"])
        srcCfg.ColorMask = FromString<eColorMask>(node["colorMask"].as<std::string>());
}
}