#pragma once

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
    std::vector<RenderPass*> m_registredPasses;
    std::vector<RenderPass*> m_activePasses;
};
}
