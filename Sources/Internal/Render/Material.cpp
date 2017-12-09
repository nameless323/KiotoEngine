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
        m_layerType = FromString<RenderLayerType>(config["renderLayer"].as<std::string>());
    if (config["fill"] != nullptr)
        m_fill = FromString<FillMode>(config["fill"].as<std::string>());
    if (config["cull"] != nullptr)
        m_cull = FromString<CullMode>(config["cull"].as<std::string>());
    if (config["windingCCW"] != nullptr)
        m_windingCCW = config["windingCCW"].as<bool>();
    if (config["depthStencil"] != nullptr)
    {
        YAML::Node dsNode = config["depthStencil"];
        if (dsNode["enableStencil"] != nullptr)
            m_enableStencill = dsNode["enableStencil"].as<bool>();
        if (dsNode["ZTest"] != nullptr)
            m_ztest = FromString<ZTest>(dsNode["ZTest"].as<std::string>());
        if (dsNode["ZWrite"] != nullptr)
            m_zwrite = dsNode["ZWrite"].as<bool>();
    }
    if (config["blending"] != nullptr)
    {
        YAML::Node bNode = config["blending"];
        if (bNode["blendOp"] != nullptr)
            m_blendOp = FromString<BlendOps>(bNode["blendOp"].as<std::string>());
        if (bNode["srcBlend"] != nullptr)
            m_srcBlend = FromString<BlendModes>(bNode["srcBlend"].as<std::string>());
        if (bNode["dstBlend"] != nullptr)
            m_srcBlend = FromString<BlendModes>(bNode["dstBlend"].as<std::string>());
    }
    if (config["colorMask"] != nullptr)
        m_colorMask = FromString<ColorMask>(config["colorMask"].as<std::string>());
}
}