#pragma once

#include <vector>

namespace Kioto
{
namespace Renderer
{
struct Light;
}

struct SystemSharedData
{
    std::vector<Renderer::Light*> Lights;
};

};