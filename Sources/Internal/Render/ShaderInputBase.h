#pragma once

#include "Render/ShaderData.h"

namespace Kioto::Renderer::SInp
{
class ShaderInputBase
{
public:
    virtual RenderObjectBufferLayout GetLayoutTemplate() const abstract;
    virtual ShaderData GetShaderData() const abstract;
    virtual std::string GetProgramName(ShaderProgramType type) const abstract;
    virtual RenderObjectConstants GetConstants() const abstract;
};
}
