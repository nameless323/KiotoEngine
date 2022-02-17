#include "Render/RenderObject.h"

#include "AssetsSystem/AssetsSystem.h"
#include "Render/Material.h"
#include "Render/Shader.h"

#include "Render/Shaders/autogen/sInp/Fallback.h"

namespace Kioto::Renderer
{
    void RenderObject::ComposeAllConstantBuffers()
    {
        for (auto& pipelines : mMaterial->GetPipelineStates())
        {
            Shader* shader = pipelines.second.Shader;
            RenderObjectBufferLayout bufferLayout = shader->CreateLayoutTemplateShalowCopy();

            for (auto& cb : bufferLayout)
            {
                if (cb.IsPerObjectBuffer())
                    cb.Reallocate();
            }

            assert(mRenderObjectBuffers.count(pipelines.first) == 0);
            mRenderObjectBuffers[pipelines.first] = std::move(bufferLayout);
            mRenderObjectConstants[pipelines.first] = std::move(shader->GetRenderObjectConstants());
        }
    }

    void RenderObject::RegisterAllTextureSets()
    {
        for (auto& textureAssetDescriptionsForPasses : mMaterial->GetTextureAssetDescriptions())
        {
            const PassName& passName = textureAssetDescriptionsForPasses.first;
            PipelineState& state = mMaterial->GetPipelineState(passName);
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
            assert(mTextureSets.count(passName) == 0);
            mTextureSets[passName] = std::move(set);
            if (mTextureSets[passName].GetTexturesCount() > 0)
            {
                // [a_vorontcov] TODO: easy to mess up. rethink
                Renderer::RegisterTextureSet(mTextureSets[passName]);
                Renderer::QueueTextureSetForUpdate(mTextureSets[passName]);
            }
        }
    }

    void RenderObject::SetTexture(const std::string& name, Texture* texture, const std::string& passName)
    {
        assert(mTextureSets.count(passName) && "Texture is missing in texture set");
        mTextureSets[passName].SetTexture(name, texture);
    }

    void RenderObject::PrepareConstantBuffers(const std::string& passName)
    {
        SInp::Fallback_sinp::CbRenderObject roBuffer;
        roBuffer.ToModel = GetToModel()->GetForGPU();
        roBuffer.ToWorld = GetToWorld()->GetForGPU();
        SetBuffer("cbRenderObject", roBuffer, passName);
    }

    void RenderObject::SetExternalCB(const std::string& passName, const std::string& cbName, ConstantBufferHandle newHandle)
    {
        if (!mRenderObjectBuffers.count(passName))
        {
            assert(false);
            return;
        }
        RenderObjectBufferLayout& layout = mRenderObjectBuffers[passName];
        auto cb = std::find_if(layout.begin(), layout.end(), [&cbName](const ConstantBuffer& b) { return b.GetName() == cbName; });
        if (cb == layout.end())
        {
            assert(false);
            return;
        }
        assert(!cb->IsPerObjectBuffer());
        cb->SetHandle(newHandle);
    }

    std::vector<Renderer::ConstantBufferHandle> RenderObject::GetCBHandles(const std::string& passName) const
    {
        if (!mRenderObjectBuffers.count(passName))
            return {};
        const RenderObjectBufferLayout& layout = mRenderObjectBuffers.at(passName);

        std::vector<Renderer::ConstantBufferHandle> handles;
        handles.reserve(layout.size());
        for (const auto& cb : layout)
            handles.push_back(cb.GetHandle());
        return handles;
    }

    std::vector<uint32> RenderObject::GetConstants(const std::string& passName) const
    {
        if (!mRenderObjectConstants.count(passName))
            return {};
        const RenderObjectConstants& constants = mRenderObjectConstants.at(passName);

        std::vector<uint32> values;
        values.reserve(constants.size());
        for (const auto& c : constants)
            values.push_back(c.GetValue());
        return values;
    }

}