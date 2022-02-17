#pragma once

#include <map>

#include "Render/DX12/VertexLayoutDX12.h"
#include "Render/RendererPublic.h"

namespace Kioto::Renderer
{
class VertexLayoutManagerDX12
{
public:
    void GenerateVertexLayout(const Shader* shader);
    const std::vector<D3D12_INPUT_ELEMENT_DESC>* FindVertexLayout(ShaderHandle handle) const;

private:
    std::map<ShaderHandle, VertexLayoutDX12> mInputLayouts;
};
}