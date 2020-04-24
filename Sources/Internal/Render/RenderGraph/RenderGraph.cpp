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
        if (pass->ConfigureInputsAndOutputs())
            m_activePasses.push_back(pass);
    }
}

void RenderGraph::Execute(std::vector<RenderObject*>& renderObjects)
{
    for (auto pass : m_activePasses)
    {
        pass->SetRenderObjects(renderObjects);
        pass->Setup();
    }

    for (auto pass : m_activePasses)
    {
        pass->BuildRenderPackets();
        pass->Cleanup();
    }
}

void RenderGraph::Submit()
{
    for (auto pass : m_activePasses)
    {
        pass->Submit();
    }
    Clear();
}

void RenderGraph::Clear()
{
    m_registredPasses.clear();
    m_activePasses.clear();
}

}