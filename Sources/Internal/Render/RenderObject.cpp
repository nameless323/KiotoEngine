#include "stdafx.h"

#include "Render/RenderObject.h"

#include "AssetsSystem/AssetsSystem.h"
#include "Render/Material.h"
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
            PipelineState& state = m_material->GetPipelineState(passName);
            Shader* shader = state.Shader;
            TextureSet set;
            for (auto& texDescr : textureAssetDescriptionsForPasses.second)
            {
                Texture* tex = nullptr;
                if (!texDescr.Path.empty())
                {
                    std::string fullPath = AssetsSystem::GetAssetFullPath(texDescr.Path);
                    tex = AssetsSystem::GetRenderAssetsManager()->GetOrLoadAsset<Texture>(fullPath);
                }
                set.AddTexture(texDescr.Name, texDescr.Offset, tex);
            }
            assert(m_textureSets.count(passName) == 0);
            m_textureSets[passName] = std::move(set);
            if (m_textureSets[passName].GetTexturesCount() > 0)
            {
                // [a_vorontcov] TODO: easy to mess up. rethink
                Renderer::RegisterTextureSet(m_textureSets[passName]);
                Renderer::QueueTextureSetForUpdate(m_textureSets[passName]);
            }
        }
    }

    void RenderObject::SetTexture(const std::string& name, Texture* texture, const std::string& passName)
    {
        assert(m_textureSets.contains(passName) && "Texture is missing in texture set");
        m_textureSets[passName].SetTexture(name, texture);
    }

}