#pragma once

#include <vector>
#include <map>
#include <string>

#include "Render/RenderGraph/ResourcesBlackboard.h"

namespace Kioto::Renderer
{
using PassBlackboard = std::pair<RenderPass*, ResourcesBlackboard>;

class ResourceTable
{
public:
    ResourceTable();
    ~ResourceTable();

    PassBlackboard* GetNextBlackboard();
    ResourcesBlackboard* GetBalackboardForPass(const RenderPass* pass);
    void ClearBlackboards();

private:
    std::vector<PassBlackboard> m_blackboardsPool;
    uint32 m_currIndex = 0;

    std::map<std::string, Texture*> m_resources;
};
}
