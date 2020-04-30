#include "stdafx.h"

#include "Render/RenderGraph/RenderGraph.h"

#include "Render/RenderOptions.h"
#include "Render/RenderPass/RenderPass.h"

namespace Kioto::Renderer
{

RenderGraph::RenderGraph()
{
    m_registredPasses.reserve(RenderOptions::MaxRenderPassesCount);
    m_activePasses.reserve(RenderOptions::MaxRenderPassesCount);

    m_commandListPool.resize(RenderOptions::MaxRenderCommandsCount);
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
        if (pass->ConfigureInputsAndOutputs(m_blackboard))
            m_activePasses.push_back({ pass, &m_commandListPool[m_currentCommandListIndex++] });
    }
}

void RenderGraph::Execute(std::vector<RenderObject*>& renderObjects)
{
    for (auto submInfo : m_activePasses)
    {
        submInfo.first->SetRenderObjects(renderObjects);
        submInfo.first->Setup();
    }

    for (auto submInfo : m_activePasses)
    {
        submInfo.first->BuildRenderPackets();
        submInfo.first->Cleanup();
    }
}

void RenderGraph::Submit()
{
    for (auto submInfo : m_activePasses)
    {
        submInfo.first->Submit();
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