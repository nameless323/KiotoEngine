#pragma once

#include "Render/ShaderData.h"

namespace Kioto::Renderer::SInp
{
class ShaderInputBase
{
public:
    virtual ShaderBufferLayoutTemplate GetLayoutTemplate() const abstract;
    virtual ShaderData GetShaderData() const abstract;
};
}
