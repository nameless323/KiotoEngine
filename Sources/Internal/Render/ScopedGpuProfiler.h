//
// Copyright (C) Aleksandr Vorontcov 2019.
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

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