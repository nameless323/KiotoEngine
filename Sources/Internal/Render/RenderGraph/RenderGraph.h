#pragma once

#include "Render/RenderGraph/ResourcesBlackboard.h"
#include "Render/RenderCommand.h"

namespace Kioto::Renderer
{
class RenderPass;
class RenderObject;

class RenderGraph
{
public:
    RenderGraph();
    ~RenderGraph();

    // TODO: [a_vorontcov] Maybe move ownership of passes here?
    void AddPass(RenderPass* renderPass);
    void SheduleGraph();
    void Execute(std::vector<RenderObject*>& renderObjects); // [a_vorontcov] TODO: for now, normally call culling system for object for pass.
    void Submit();
    void Clear();

private:
    using PassSubmitionInfo = std::pair<RenderPass*, CommandList*>;

    std::vector<RenderPass*> m_registredPasses;
    std::vector<PassSubmitionInfo> m_activePasses;

    std::vector<CommandList> m_commandListPool;
    uint32 m_currentCommandListIndex = 0;

    ResourcesBlackboard m_blackboard;
};
}
