#include "stdafx.h"

#include "Render/RenderGraph/RenderGraph.h"
#include "Render/RenderPass/RenderPass.h"

namespace Kioto::Renderer
{

RenderGraph::RenderGraph()
{
    m_registredPasses.resize(128);
}

void RenderGraph::SchedulePass(RenderPass& renderPass)
{
    PassInfo info;
    info.ConfigureInputsOutputs = [&renderPass] { return renderPass.ConfigureInputsAndOutputs(); };
    info.SetRenderObjects = [&renderPass](std::vector<RenderObject*> renderObjects) { renderPass.SetRenderObjects(renderObjects); };
    info.Setup = [&renderPass] { renderPass.Setup(); };
    info.BuildRenderPackets = [&renderPass] { renderPass.BuildRenderPackets(); };
    info.Submit = [&renderPass] { renderPass.Submit(); };
}

RenderGraph::~RenderGraph()
{
    m_registredPasses.clear();
    m_activePasses.clear();
}

}