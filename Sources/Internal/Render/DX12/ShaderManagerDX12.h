#pragma once

#include <map>

#include "Render/RendererPublic.h"
#include "Render/DX12/ShaderDX12.h"
#include "Dx12Helpers/d3dx12.h"

namespace Kioto::Renderer
{
class ShaderManagerDX12
{
public:
    void RegisterShader(Shader* shader);

    const CD3DX12_SHADER_BYTECODE* GetShaderBytecode(ShaderHandle handle, ShaderProgramType type) const;
    const std::vector<ShaderDX12>* GetDxShaders(ShaderHandle handle) const;

private:
    std::map<ShaderHandle, std::vector<ShaderDX12>> mShaders;

    ShaderDX12 CompileDXShader(const Shader& shader, const std::string& entryName, const std::string& shaderModel);

#ifdef _DEBUG
    inline static constexpr UINT shaderFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
    inline static constexpr UINT shaderFlags = 0;
#endif
};
}