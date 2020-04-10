#pragma once

namespace Kioto::Renderer
{
class RenderPass;

class ScopedGpuProfiler
{
public:
    ScopedGpuProfiler(RenderPass* pass, std::string name);

    ~ScopedGpuProfiler();

private:
    RenderPass* m_pass = nullptr;
};
}