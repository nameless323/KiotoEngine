#pragma once

#include "Render/ShaderData.h"

namespace Kioto::Renderer::Sinp
{
class ShaderInputBase
{
public:
    virtual ShaderBufferLayoutTemplate GetLayoutTemplate() const abstract;
    virtual ShaderData GetShaderData() const abstract;
};
}
