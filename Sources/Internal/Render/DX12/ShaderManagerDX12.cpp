#include "stdafx.h"

#include "Render/DX12/ShaderManagerDX12.h"

#include "Render/Shader.h"

namespace Kioto::Renderer
{
void ShaderManagerDX12::RegisterShader(Shader* shader)
{
    auto it = m_shaders.find(shader->GetHandle());
    if (it != m_shaders.cend())
        return;
    shader->SetHandle(GetNewHandle());
    shader->SetShaderData(ShaderParser::ParseShaderFromString(shader->GetShaderString(), nullptr));
    std::vector<ShaderDX12> shadersDX;
    if (shader->GetShaderData().shaderPrograms & uint8(ShaderProgramType::Vertex))
        shadersDX.push_back(CompileDXShader(*shader, "vs", "vs_5_1"));

    if (shader->GetShaderData().shaderPrograms & uint8(ShaderProgramType::Fragment))
        shadersDX.push_back(CompileDXShader(*shader, "ps", "ps_5_1"));

    m_shaders[shader->GetHandle()] = std::move(shadersDX);
}

const CD3DX12_SHADER_BYTECODE* ShaderManagerDX12::GetShaderBytecode(ShaderHandle handle, ShaderProgramType type) const
{
    auto it = m_shaders.find(handle);
    if (it == m_shaders.cend())
        return nullptr;
    auto& shaders = it->second;
    for (auto& dxShader : shaders)
    {
        if (dxShader.GetType() == type)
            return &dxShader.GetBytecode();
    }
    return nullptr;
}

const std::vector<ShaderDX12>* ShaderManagerDX12::GetDxShaders(ShaderHandle handle) const
{
    auto it = m_shaders.find(handle);
    if (it == m_shaders.cend())
        return nullptr;
    return &it->second;
}

ShaderDX12 ShaderManagerDX12::CompileDXShader(const Shader& shader, const std::string& entryName, const std::string& shaderModel)
{
    ShaderDX12 res;
    res.SetHandle(GetNewHandle());
    HRESULT hr = res.Compile(shader.GetShaderData().output.c_str(), shader.GetShaderData().output.length() * sizeof(char), entryName.c_str(), shaderModel.c_str(), shaderFlags);
    if (!res.GetIsCompiled())
        OutputDebugStringA(res.GetErrorMsg());
    ThrowIfFailed(hr);
    return res;
}
}