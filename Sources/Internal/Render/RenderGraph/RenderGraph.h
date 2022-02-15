#pragma once

#include "Render/RenderGraph/ResourceTable.h"
#include "Render/RenderCommand.h"

namespace Kioto::Renderer
{
class RenderPass;
class RenderObject;
struct DrawData;

class RenderGraph
{
public:
    RenderGraph();
    ~RenderGraph();

    // TODO: [a_vorontcov] Maybe move ownership of passes here?
    void AddPass(RenderPass* renderPass);
    void SheduleGraph();
    void Execute(DrawData& drawData); // [a_vorontcov] TODO: for now, normally call culling system for object for pass.
    void Submit();
    void Clear();

private:
    struct PassSubmitionInfo
    {
        RenderPass* Pass;
        CommandList* CmdList;
    };

    std::vector<RenderPass*> mRegistredPasses;
    std::vector<PassSubmitionInfo> mActivePasses;

    std::vector<CommandList> mCommandListPool;
    uint32 mCurrentCommandListIndex = 0;

    ResourceTable mResourceTable;
};
}
