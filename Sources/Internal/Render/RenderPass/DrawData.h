#pragma once

#include <vector>

namespace Kioto::Renderer
{
class RenderObject;
struct Light;

struct DrawData
{
    std::vector<RenderObject*> RenderObjects;
    std::vector<Light*> Lights;
};
}
