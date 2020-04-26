#include "stdafx.h"

#include "Render/RenderGraph/ResourcesBlackboard.h"

namespace Kioto::Renderer
{

ResourcesBlackboard::ResourcesBlackboard()
{

}

ResourcesBlackboard::~ResourcesBlackboard()
{
    for (auto& it : m_resources)
        SafeDelete(it.second);
    m_resources.clear();
}

void ResourcesBlackboard::NewTexture(const std::string& name, TextureDescriptor& desc)
{

}

Kioto::Renderer::Texture* ResourcesBlackboard::GetRenderTarget(const std::string& name)
{
    if (m_resources.count(name) != 1)
        assert(false && "Requested resource is not in the table (you forgot to call ResourcesBlackboard::New*)?");
    return m_resources[name];
}

Kioto::Renderer::Texture* ResourcesBlackboard::GetShaderResource(const std::string& name)
{
    if (m_resources.count(name) != 1)
        assert(false && "Requested resource is not in the table (you forgot to call ResourcesBlackboard::New*)?");
    return m_resources[name];
}


}