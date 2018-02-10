//
// Copyright (C) Alexandr Vorontsov. 2017
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#pragma once

#include <map>

#include "AssetsSystem/AssetsSystem.h"
#include "Render/Texture/Texture.h"
#include "Render/Renderer.h"

namespace Kioto::Renderer
{

template <typename T>
class RenderAssetsManager
{
public:
    void Init();
    void Shutdown();
    T* GetOrLoadAsset(const std::string path);

private:
    std::map<std::string, T*> m_assets;
};

template <typename T>
void RenderAssetsManager<T>::Init()
{
}

template <typename T>
void RenderAssetsManager<T>::Shutdown()
{
}

template <typename T>
T* RenderAssetsManager<T>::GetOrLoadAsset(const std::string path)
{
    auto it = m_assets.find(path);
    if (it != m_assets.end())
        return it->second;
    T* asset = AssetsSystem::LoadAsset<T>(path);
    Renderer::RegisterRenderAsset<T>(asset);
    m_assets[path] = asset;
    return asset;
}
}