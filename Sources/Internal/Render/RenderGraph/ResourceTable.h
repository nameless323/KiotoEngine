#pragma once

#include <vector>
#include <map>
#include <string>

#include "Math/Matrix4.h"
#include "Render/RenderGraph/ResourcesBlackboard.h"

namespace Kioto::Renderer
{
using PassBlackboard = std::pair<RenderPass*, ResourcesBlackboard>;

struct TransientSharedData // [a_vorontcov] todo: Bad idea.
{
    Matrix4 ShadowTransform;
};

class ResourceTable
{
public:
    ResourceTable();
    ~ResourceTable();

    PassBlackboard* GetNextBlackboard();
    ResourcesBlackboard* GetBalackboardForPass(const RenderPass* pass);
    void ProcessCreationRequest(const ResourceCreationRequest& request);
    void ClearBlackboards();

    void CreateResourcesForPass(const RenderPass* pass);

    Texture* GetResource(const std::string& name);

    TransientSharedData& GetPassesSharedData();

private:
    std::vector<PassBlackboard> mBlackboardsPool;
    uint32 mCurrIndex = 0;

    std::map<std::string, Texture*> mResources;

    TransientSharedData mPassesSharedData{};
};

inline Texture* ResourceTable::GetResource(const std::string& name)
{
    assert(mResources.count(name) && "Resource wasn't added");
    return mResources[name];
}

inline TransientSharedData& ResourceTable::GetPassesSharedData()
{
    return mPassesSharedData;
}

}
