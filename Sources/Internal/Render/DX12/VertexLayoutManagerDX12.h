//
// Copyright (C) Aleksandr Vorontcov. 2018
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#pragma once

#include <map>

#include "Render/RendererPublic.h"

#include "Render/DX12/VertexLayoutDX12.h"

namespace Kioto::Renderer
{
class VertexLayoutManagerDX12
{
public:
    void GenerateVertexLayout(const Shader* shader);
    const std::vector<D3D12_INPUT_ELEMENT_DESC>* FindVertexLayout(ShaderHandle handle) const;

private:
    std::map<ShaderHandle, VertexLayoutDX12> m_inputLayouts;
};
}