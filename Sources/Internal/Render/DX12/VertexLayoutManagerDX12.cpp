#include "Render/DX12/VertexLayoutManagerDX12.h"

#include <d3d12.h>

#include "Render/Shader.h"

namespace Kioto::Renderer
{
void VertexLayoutManagerDX12::GenerateVertexLayout(const Shader* shader)
{
    for (auto& l : m_inputLayouts)
    {
        if (shader->GetShaderData().vertexLayout == l.second.LayoutKioto)
            m_inputLayouts[shader->GetHandle()] = l.second; // [a_vorontcov] TODO: remove copy.
    }
    VertexLayout layout = shader->GetShaderData().vertexLayout;
    VertexLayoutDX12 res;
    res.Handle = GetNewHandle();
    res.LayoutDX.reserve(layout.GetElements().size());
    for (const auto& e : layout.GetElements())
    {
        res.LayoutDX.push_back(D3D12_INPUT_ELEMENT_DESC{ SemanticNames[e.Semantic].c_str(), e.SemanticIndex, VertexDataFormats[e.Format], 0, e.Offset, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
    }
    res.LayoutKioto = std::move(layout);
    m_inputLayouts[shader->GetHandle()] = res;
}

const std::vector<D3D12_INPUT_ELEMENT_DESC>* VertexLayoutManagerDX12::FindVertexLayout(ShaderHandle handle) const
{
    auto it = m_inputLayouts.find(handle);
    if (it == m_inputLayouts.cend())
        return nullptr;
    return &it->second.LayoutDX;
}
}