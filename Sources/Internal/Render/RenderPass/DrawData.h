#pragma once

#include <vector>

namespace Kioto
{
class LightComponent; // [a_vorontcov] TODO: for now
}

namespace Kioto::Renderer
{
class RenderObject;

struct DrawData
{
    std::vector<RenderObject*> RenderObjects;
    std::vector<LightComponent*> Lights;
};
}
