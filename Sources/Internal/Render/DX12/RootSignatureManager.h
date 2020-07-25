#pragma once

#include <d3d12.h>
#include <map>
#include <wrl/client.h>

#include "Render/RendererPublic.h"
#include "Render/ShaderData.h"

namespace Kioto::Renderer
{
struct StateDX;

class RootSignatureManager
{
public:
    void CreateRootSignature(const StateDX& state, const ShaderData& shaderData, const RenderObjectBufferLayout& bufferLayoutTemplate, ShaderHandle handle);  // [a_vorontcov] Root sig and shader 1 to 1 connection.
    ID3D12RootSignature* GetRootSignature(ShaderHandle handle) const;

private:
    std::map<ShaderHandle, Microsoft::WRL::ComPtr<ID3D12RootSignature>> m_rootSignatures;
};
}