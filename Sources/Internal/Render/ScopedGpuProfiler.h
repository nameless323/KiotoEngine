#pragma once
#include <string>

namespace Kioto::Renderer
{
class CommandList;

class ScopedGpuProfiler
{
public:
    ScopedGpuProfiler(CommandList* cmdList, std::string name);

    ~ScopedGpuProfiler();

private:
    CommandList* mCmdList = nullptr;
};
}
