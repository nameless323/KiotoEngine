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
    if (config["renderLayer"] != nullptr)
        m_layerType = FromString<RenderLayerType>(config["renderLayer"].as<std::string>());
}
}