//
// Copyright (C) Alexandr Vorontsov. 2017
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#include "stdafx.h"

#include "Render/Material.h"

#include "yaml-cpp/yaml.h"

#include "AssetsSystem/AssetsSystem.h"
#include "AssetsSystem/RenderStateParamsConverter.h"

namespace Kioto
{
static const float32 CurrentVersion = 0.01f;

Material::Material(const std::wstring& path)
{
    using namespace RenderParamsConverter;
    std::string pathStr = WstrToStr(path);
    if (!AssetsSystem::CheckIfFileExist(path))
        throw "Material not exist";

    YAML::Node config = YAML::LoadFile(pathStr);
    float32 version = -1.0f;
    if (config["version"] != nullptr)
        version = config["version"].as<float32>();
    if (config["shader"] != nullptr)
        m_shaderPath = config["shader"].as<std::string>();
    if (config["renderLayer"] != nullptr)
        m_layerType = FromString<eRenderLayerType>(config["renderLayer"].as<std::string>());
    if (config["fill"] != nullptr)
        m_fill = FromString<eFillMode>(config["fill"].as<std::string>());
    if (config["cull"] != nullptr)
        m_cull = FromString<eCullMode>(config["cull"].as<std::string>());
    if (config["windingCCW"] != nullptr)
        m_windingCCW = config["windingCCW"].as<bool>();
    if (config["depthStencil"] != nullptr)
    {
        YAML::Node dsNode = config["depthStencil"];
        if (dsNode["enableStencil"] != nullptr)
            m_enableStencill = dsNode["enableStencil"].as<bool>();
        if (dsNode["ZTest"] != nullptr)
            m_ztest = FromString<eZTest>(dsNode["ZTest"].as<std::string>());
        if (dsNode["ZWrite"] != nullptr)
            m_zwrite = dsNode["ZWrite"].as<bool>();
        if (dsNode["writeMask"] != nullptr)
            m_stencilWriteMask = static_cast<uint8>(std::stoul(dsNode["writeMask"].as<std::string>(), 0, 0));
        if (dsNode["readMask"] != nullptr)
            m_stencilWriteMask = static_cast<uint8>(std::stoul(dsNode["readMask"].as<std::string>(), 0, 0));

        if (dsNode["stencilFront"] != nullptr)
        {
            YAML::Node frontNode = dsNode["stencilFront"];
            if (frontNode["fail"] != nullptr)
                m_frontFaceStencilDesc.StencilDepthFailOp = FromString<eStencilOp>(frontNode["fail"].as<std::string>());
            if (frontNode["ZFfail"] != nullptr)
                m_frontFaceStencilDesc.StencilDepthFailOp = FromString<eStencilOp>(frontNode["ZFfail"].as<std::string>());
            if (frontNode["pass"] != nullptr)
                m_frontFaceStencilDesc.StencilPassOp = FromString<eStencilOp>(frontNode["pass"].as<std::string>());
            if (frontNode["func"] != nullptr)
                m_frontFaceStencilDesc.StencilFunc = FromString<eStencilTest>(frontNode["func"].as<std::string>());
        }
        if (dsNode["stencilBack"] != nullptr)
        {
            YAML::Node backNode = dsNode["stencilBack"];
            if (backNode["fail"] != nullptr)
                m_backFaceStencilDesc.StencilDepthFailOp = FromString<eStencilOp>(backNode["fail"].as<std::string>());
            if (backNode["ZFfail"] != nullptr)
                m_backFaceStencilDesc.StencilDepthFailOp = FromString<eStencilOp>(backNode["ZFfail"].as<std::string>());
            if (backNode["pass"] != nullptr)
                m_backFaceStencilDesc.StencilPassOp = FromString<eStencilOp>(backNode["pass"].as<std::string>());
            if (backNode["func"] != nullptr)
                m_backFaceStencilDesc.StencilFunc = FromString<eStencilTest>(backNode["func"].as<std::string>());
        }
    }
    if (config["blending"] != nullptr)
    {
        YAML::Node bNode = config["blending"];
        if (bNode["blendOp"] != nullptr)
            m_blendOp = FromString<eBlendOps>(bNode["blendOp"].as<std::string>());
        if (bNode["srcBlend"] != nullptr)
            m_srcBlend = FromString<eBlendModes>(bNode["srcBlend"].as<std::string>());
        if (bNode["dstBlend"] != nullptr)
            m_srcBlend = FromString<eBlendModes>(bNode["dstBlend"].as<std::string>());
    }
    if (config["colorMask"] != nullptr)
        m_colorMask = FromString<eColorMask>(config["colorMask"].as<std::string>());
}
}