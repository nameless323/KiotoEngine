#include "Render/RenderGraph/RenderGraph.h"

#include "Render/Renderer.h"
#include "Render/RenderSettings.h"
#include "Render/RenderPass/RenderPass.h"
#include "Render/RenderPass/DrawData.h"

namespace Kioto::Renderer
{

RenderGraph::RenderGraph()
{
    mRegistredPasses.reserve(RenderSettings::MaxRenderPassesCount);
    mActivePasses.reserve(RenderSettings::MaxRenderPassesCount);

    mCommandListPool.resize(RenderSettings::MaxRenderCommandsCount);
}

void RenderGraph::AddPass(RenderPass* renderPass)
{
    mRegistredPasses.push_back(renderPass);
}

RenderGraph::~RenderGraph()
{
    Clear();
}

void RenderGraph::SheduleGraph()
{
    for (auto pass : mRegistredPasses)
    {
        PassBlackboard* passBlackboard = mResourceTable.GetNextBlackboard();
        passBlackboard->first = pass;
        if (pass->ConfigureInputsAndOutputs(passBlackboard->second))
            mActivePasses.push_back({ pass, &mCommandListPool[mCurrentCommandListIndex++] });
    }
}

void RenderGraph::Execute(DrawData& drawData)
{
    for (auto& submInfo : mActivePasses)
    {
        submInfo.Pass->SetDrawData(&drawData);
        submInfo.Pass->Setup();
    }

    for (auto& submInfo : mActivePasses)
    {
        submInfo.CmdList->PushCommand(RenderCommandHelpers::CreateBeginGpuEventCommand(submInfo.Pass->GetName()));
        ResourcesBlackboard* blackboard = mResourceTable.GetBalackboardForPass(submInfo.Pass);

        mResourceTable.CreateResourcesForPass(submInfo.Pass);
        for (const auto& transitions : blackboard->GetTransitionRequests())
        {
            Texture* tex = mResourceTable.GetResource(transitions.ResourceName);
            submInfo.CmdList->PushCommand(RenderCommandHelpers::CreateResourceTransitonCommand(tex->GetHandle(), transitions.TransitionTo, submInfo.Pass));
        }

        submInfo.Pass->BuildRenderPackets(submInfo.CmdList, mResourceTable);
        submInfo.Pass->Cleanup();
        submInfo.CmdList->PushCommand(RenderCommandHelpers::CreateEndGpuEventCommand());
    }
    mResourceTable.ClearBlackboards();
}

void RenderGraph::Submit()
{
    for (auto& submInfo : mActivePasses)
    {
        Renderer::SubmitRenderCommands(submInfo.CmdList->GetCommands());
        submInfo.Pass->Cleanup();
    }
    Clear();
}

void RenderGraph::Clear()
{
    mRegistredPasses.clear();
    mActivePasses.clear();
    for (uint32 i = 0; i < mCurrentCommandListIndex; ++i)
        mCommandListPool[i].ClearCommands();
    mCurrentCommandListIndex = 0;
}

}