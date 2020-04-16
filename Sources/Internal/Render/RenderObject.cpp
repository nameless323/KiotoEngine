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
            PipelineState& state = m_material->GetPipelineState(passName);
            Shader* shader = state.Shader;
            TextureSet set;
            for (auto& texDescr : textureAssetDescriptionsForPasses.second)
            {
                std::string fullPath = AssetsSystem::GetAssetFullPath(texDescr.Path);
                Texture* tex = AssetsSystem::GetRenderAssetsManager()->GetOrLoadAsset<Texture>(fullPath);
                const TextureSet& shaderTextureSet = shader->GetShaderData().textureSet; // [a_vorontcov] TODO: move this shit with shader to material
                uint16 texOffset = shaderTextureSet.GetTextureOffset(texDescr.Name);
                set.AddTexture(texDescr.Name, texOffset, tex);
            }
            assert(m_textureSets.count(passName) == 0);
            m_textureSets[passName] = std::move(set);
            if (m_textureSets[passName].GetTexturesCount() > 0)
            {
                Renderer::RegisterTextureSet(m_textureSets[passName]);
                Renderer::QueueTextureSetForUpdate(m_textureSets[passName]);
            }
        }
    }
}