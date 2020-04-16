#include "stdafx.h"

#include "Render/Material.h"
#include "Render/RenderObject.h"
#include "Render/Shader.h"

namespace Kioto::Renderer
{
    void RenderObject::ComposeAllConstantBuffers()
    {
        for (auto& pipelines : m_material->GetPipelineStates())
        {
            Shader* shader = pipelines.second.Shader;
            RenderObjectBufferLayout bufferLayout;

            bufferLayout.constantBuffers = shader->CreateLayoutTemplateShalowCopy();

            for (auto& cb : bufferLayout.constantBuffers)
                cb.ComposeBufferData();

            assert(m_renderObjectBuffers.count(pipelines.first) == 0);
            m_renderObjectBuffers[pipelines.first] = std::move(bufferLayout);
        }
    }

    void RenderObject::RegisterAllTextureSets()
    {
        for (auto& textureAssetDescriptionsForPasses : m_material->GetTextureAssetDescriptions())
        {
            const PassName& passName = textureAssetDescriptionsForPasses.first;
            const PipelineState& state = m_material->GetPipelineState(passName);
            TextureSet set;
            for (auto& texDescr : textureAssetDescriptionsForPasses.second)
            {
                std::string fullPath = AssetsSystem::GetAssetFullPath(texDescr.Path);
                Texture* tex = AssetsSystem::GetRenderAssetsManager()->GetOrLoadAsset<Texture>(fullPath);
                set.SetTexture(texDescr.Name, tex);
            }
            if (set.GetTexturesCount() > 0)
                Renderer::RegisterTextureSet(set);
            assert(m_textureSets.count(passName) == 0);
            m_textureSets[passName] = std::move(set);
        }
    }
}