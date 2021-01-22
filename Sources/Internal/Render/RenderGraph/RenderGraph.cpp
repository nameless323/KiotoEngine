#include "stdafx.h"

#include "Render/RenderGraph/RenderGraph.h"

#include "Render/Renderer.h"
#include "Render/RenderSettings.h"
#include "Render/RenderPass/RenderPass.h"
#include "Render/RenderPass/DrawData.h"

namespace Kioto::Renderer
{

RenderGraph::RenderGraph()
{
    m_registredPasses.reserve(RenderSettings::MaxRenderPassesCount);
    m_activePasses.reserve(RenderSettings::MaxRenderPassesCount);

    m_commandListPool.resize(RenderSettings::MaxRenderCommandsCount);
}

void RenderGraph::AddPass(RenderPass* renderPass)
{
    m_registredPasses.push_back(renderPass);
}

RenderGraph::~RenderGraph()
{
    Clear();
}

void RenderGraph::SheduleGraph()
{
    for (auto pass : m_registredPasses)
    {
        PassBlackboard* passBlackboard = m_resourceTable.GetNextBlackboard();
        passBlackboard->first = pass;
        if (pass->ConfigureInputsAndOutputs(passBlackboard->second))
            m_activePasses.push_back({ pass, &m_commandListPool[m_currentCommandListIndex++] });
    }
}

void RenderGraph::Execute(DrawData& drawData)
{
    for (auto& submInfo : m_activePasses)
    {
        submInfo.Pass->SetDrawData(&drawData);
        submInfo.Pass->Setup();
    }

    for (auto& submInfo : m_activePasses)
    {
        submInfo.CmdList->PushCommand(RenderCommandHelpers::CreateBeginGpuEventCommand(submInfo.Pass->GetName()));
        ResourcesBlackboard* blackboard = m_resourceTable.GetBalackboardForPass(submInfo.Pass);

        m_resourceTable.CreateResourcesForPass(submInfo.Pass);
        for (const auto& transitions : blackboard->GetTransitionRequests())
        {
            Texture* tex = m_resourceTable.GetResource(transitions.ResourceName);
            submInfo.CmdList->PushCommand(RenderCommandHelpers::CreateResourceTransitonCommand(tex->GetHandle(), transitions.TransitionTo, submInfo.Pass));
        }

        submInfo.Pass->BuildRenderPackets(submInfo.CmdList, m_resourceTable);
        submInfo.Pass->Cleanup();
        submInfo.CmdList->PushCommand(RenderCommandHelpers::CreateEndGpuEventCommand());
    }
    m_resourceTable.ClearBlackboards();
}

void RenderGraph::Submit()
{
    for (auto& submInfo : m_activePasses)
    {
        Renderer::SubmitRenderCommands(submInfo.CmdList->GetCommands());
        submInfo.Pass->Cleanup();
    }
    Clear();
}

void RenderGraph::Clear()
{
    m_registredPasses.clear();
    m_activePasses.clear();
    for (uint32 i = 0; i < m_currentCommandListIndex; ++i)
        m_commandListPool[i].ClearCommands();
    m_currentCommandListIndex = 0;
}

}