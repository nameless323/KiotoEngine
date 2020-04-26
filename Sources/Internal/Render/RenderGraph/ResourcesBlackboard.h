#pragma once

#include <unordered_map>
#include <string>
#include "Render/Texture/Texture.h"

namespace Kioto::Renderer
{
class ResourcesBlackboard
{
public:
    ResourcesBlackboard();
    ~ResourcesBlackboard();

    void NewTexture(const std::string& name, TextureDescriptor& desc);
    Texture* GetRenderTarget(const std::string& name);
    Texture* GetShaderResource(const std::string& name);

private:
    std::unordered_map<std::string, Texture*> m_resources;
};
}