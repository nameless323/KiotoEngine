#include "Render/RenderGraph/ResourceTable.h"

#include "Core/CoreHelpers.h"
#include "Render/Texture/Texture.h"
#include "Render/Renderer.h"

namespace Kioto::Renderer
{

ResourceTable::ResourceTable()
{
    mBlackboardsPool.resize(512);
}

ResourceTable::~ResourceTable()
{
    for (auto& it : mResources)
        SafeDelete(it.second);
}

PassBlackboard* ResourceTable::GetNextBlackboard()
{
    return &mBlackboardsPool[mCurrIndex++];
}

void ResourceTable::ClearBlackboards()
{
    for (uint32 i = 0; i < mCurrIndex; ++i)
    {
        mBlackboardsPool[i].first = nullptr;
        mBlackboardsPool[i].second.Clear();
    }
    mCurrIndex = 0;
    mPassesSharedData = {};
}

ResourcesBlackboard* ResourceTable::GetBalackboardForPass(const RenderPass* pass)
{
    auto it = std::find_if(mBlackboardsPool.begin(), mBlackboardsPool.end(),
        [&pass](const PassBlackboard& bb) { return bb.first == pass; });

    assert(it != mBlackboardsPool.end() && "Pass is unregistered for current frame");
    return &it->second;
}

void ResourceTable::ProcessCreationRequest(const ResourceCreationRequest& request)
{
    if (mResources.count(request.ResourceName))
    {
        const TextureDescriptor& desc = mResources[request.ResourceName]->GetDescriptor();
        if (desc != request.Desc)
            assert(false && "To be implemented...");
        else
            return;
    }
    Texture* tex = new Texture(request.Desc);
    Renderer::RegisterRenderAsset(tex);
    mResources[request.ResourceName] = tex;
}

void ResourceTable::CreateResourcesForPass(const RenderPass* pass)
{
    ResourcesBlackboard* blackboard = GetBalackboardForPass(pass);
    for (const auto& request : blackboard->GetCreationRequest())
    {
        ProcessCreationRequest(request);
    }
}

}