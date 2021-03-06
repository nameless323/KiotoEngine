#pragma once

#include <vector>
#include <map>
#include <string>

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
    std::vector<PassBlackboard> m_blackboardsPool;
    uint32 m_currIndex = 0;

    std::map<std::string, Texture*> m_resources;

    TransientSharedData m_passesSharedData{};
};

inline Texture* ResourceTable::GetResource(const std::string& name)
{
    assert(m_resources.count(name) && "Resource wasn't added");
    return m_resources[name];
}

inline TransientSharedData& ResourceTable::GetPassesSharedData()
{
    return m_passesSharedData;
}

}
