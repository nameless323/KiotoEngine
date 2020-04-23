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
    void SchedulePass(RenderPass& renderPass);

private:
    struct PassInfo
    {
        std::function<bool(void)> ConfigureInputsOutputs;
        std::function<void(std::vector<RenderObject*>)> SetRenderObjects;
        std::function<void(void)> Setup;
        std::function<void(void)> BuildRenderPackets;
        std::function<void(void)> Submit;
    };

    std::vector<PassInfo> m_registredPasses;
    std::vector<PassInfo*> m_activePasses;
};
}
