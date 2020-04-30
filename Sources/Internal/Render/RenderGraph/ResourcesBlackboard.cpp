#include "stdafx.h"

#include "Render/RenderGraph/ResourcesBlackboard.h"

#include "Render/Renderer.h"

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
    if (m_resources.count(name) != 0)
    {
        if (desc != m_resources[name]->GetDescriptor())
            assert(false);
        else
            return;
    }
    Texture* tex = new Texture(desc);
    Renderer::RegisterRenderAsset(tex);
    m_resources[name] = tex;
}

Texture* ResourcesBlackboard::GetRenderTarget(const std::string& name)
{
    if (m_resources.count(name) != 1)
        assert(false && "Requested resource is not in the table (you forgot to call ResourcesBlackboard::New*)?");
    return m_resources[name];
}

Texture* ResourcesBlackboard::GetShaderResource(const std::string& name)
{
    if (m_resources.count(name) != 1)
        assert(false && "Requested resource is not in the table (you forgot to call ResourcesBlackboard::New*)?");
    return m_resources[name];
}


}