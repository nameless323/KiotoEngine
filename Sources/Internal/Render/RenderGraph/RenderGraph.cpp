#include "stdafx.h"

#include "Render/RenderGraph/RenderGraph.h"

#include "Render/RenderOptions.h"
#include "Render/RenderPass/RenderPass.h"

namespace Kioto::Renderer
{

RenderGraph::RenderGraph()
{
    m_registredPasses.resize(RenderOptions::MaxRenderPassesCount);
    m_activePasses.resize(RenderOptions::MaxRenderPassesCount);
}

void RenderGraph::AddPass(RenderPass& renderPass)
{
    PassInfo info;
    info.ConfigureInputsOutputs = [&renderPass] { return renderPass.ConfigureInputsAndOutputs(); };
    info.SetRenderObjects = [&renderPass](std::vector<RenderObject*> renderObjects) { renderPass.SetRenderObjects(renderObjects); };
    info.Setup = [&renderPass] { renderPass.Setup(); };
    info.BuildRenderPackets = [&renderPass] { renderPass.BuildRenderPackets(); };
    info.Submit = [&renderPass] { renderPass.Submit(); };
    info.Cleanup = [&renderPass] { renderPass.Cleanup(); };
    m_registredPasses.push_back(std::move(info));
}

RenderGraph::~RenderGraph()
{
    m_registredPasses.clear();
    m_activePasses.clear();
}

void RenderGraph::SheduleGraph()
{
    for (PassInfo& pass : m_registredPasses)
    {
        if (pass.ConfigureInputsOutputs())
            m_activePasses.push_back(&pass);
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
    m_registredPasses.clear();
    m_activePasses.clear();
}

}