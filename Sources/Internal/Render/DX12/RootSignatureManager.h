//
// Copyright (C) Alexandr Vorontsov. 2018
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#pragma once

#include <d3d12.h>
#include <map>
#include <wrl/client.h>

#include "Render/RendererPublic.h"

namespace Kioto::Renderer
{
struct StateDX;
struct ShaderData;

class RootSignatureManager
{
public:
    void CreateRootSignature(const StateDX& state, const ShaderData& parseResult, ShaderHandle handle);  // [a_vorontsov] Root sig and shader 1 to 1 connection.
    ID3D12RootSignature* GetRootSignature(ShaderHandle handle) const;

private:
    std::map<ShaderHandle, Microsoft::WRL::ComPtr<ID3D12RootSignature>> m_rootSignatures;
};
}