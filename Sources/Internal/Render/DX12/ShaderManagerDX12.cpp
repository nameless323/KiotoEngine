#include "stdafx.h"

#include "Render/DX12/ShaderManagerDX12.h"

#include "AssetsSystem/AssetsSystem.h"
#include "Core/CoreHelpers.h"
#include "Render/Shader.h"

#include "Render/Shaders/autogen/KiotoShaders.h"

namespace Kioto::Renderer
{
void ShaderManagerDX12::RegisterShader(Shader* shader)
{
    auto it = m_shaders.find(shader->GetHandle());
    if (it != m_shaders.cend())
        return;
    shader->SetHandle(GetNewHandle());

    // [a_vorontcov] TODO: api independent, move to shader ctor
    std::string filename = FilesystemHelpers::GetFilenameFromPath(shader->GetAssetPath());
    SInp::ShaderInputBase& parsedShader = SInp::KiotoShaders::GetShader(filename);
    shader->SetShaderData(parsedShader.GetShaderData()); // [a_vorontcov] TODO:: Move it?
    shader->SetBufferLayoutTemplate(parsedShader.GetLayoutTemplate());

    std::vector<ShaderDX12> shadersDX;
    if (shader->GetShaderData().shaderPrograms & uint8(ShaderProgramType::Vertex))
        shadersDX.push_back(CompileDXShader(*shader, parsedShader.GetProgramName(ShaderProgramType::Vertex), "vs_5_1"));

    if (shader->GetShaderData().shaderPrograms & uint8(ShaderProgramType::Fragment))
        shadersDX.push_back(CompileDXShader(*shader, parsedShader.GetProgramName(ShaderProgramType::Fragment), "ps_5_1"));

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
    std::string shaderPath = AssetsSystem::GetAssetFullPath(shader.GetShaderData().shaderPath);
    HRESULT hr = res.CompileFromFile(StrToWstr(shaderPath).c_str(), entryName.c_str(), shaderModel.c_str(), shaderFlags);
    if (!res.GetIsCompiled())
        OutputDebugStringA(res.GetErrorMsg());
    ThrowIfFailed(hr);
    return res;
}
}