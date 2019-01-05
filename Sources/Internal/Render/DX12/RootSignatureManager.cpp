//
// Copyright (C) Aleksandr Vorontcov. 2018
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#include "stdafx.h"

#include "Render/DX12/RootSignatureManager.h"

#include "Render/DX12/Shader/ShaderParser.h"
#include "Render/DX12/StateDX.h"

namespace Kioto::Renderer
{

void RootSignatureManager::CreateRootSignature(const StateDX& state, const ShaderData& parseResult, ShaderHandle handle)
{
    using Microsoft::WRL::ComPtr;

    std::vector<CD3DX12_ROOT_PARAMETER1> rootParams;
    for (size_t i = 0; i < parseResult.constantBuffers.size(); ++i)
    {
        CD3DX12_ROOT_PARAMETER1 param;
        param.InitAsConstantBufferView(parseResult.constantBuffers[i].GetIndex(), parseResult.constantBuffers[i].GetSpace());
        rootParams.push_back(std::move(param));
    }
    std::vector<D3D12_DESCRIPTOR_RANGE1> ranges; // [a_vorontcov] Careful, table remembers pointer to range.
    ranges.reserve(parseResult.textureSet.GetTexturesCount());
    if (parseResult.textureSet.GetTexturesCount() > 0)
    {
        ranges.emplace_back();
        D3D12_DESCRIPTOR_RANGE1* texRange = &ranges.back();
        texRange->NumDescriptors = 1;
        texRange->BaseShaderRegister = 0;
        texRange->OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
        texRange->Flags = D3D12_DESCRIPTOR_RANGE_FLAG_NONE;
        texRange->RegisterSpace = 0;
        texRange->RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;

        CD3DX12_ROOT_PARAMETER1 table;
        table.InitAsDescriptorTable(1, texRange, D3D12_SHADER_VISIBILITY_PIXEL);
        rootParams.push_back(std::move(table));
    }

    D3D12_FEATURE_DATA_ROOT_SIGNATURE signatureData = {};
    signatureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;

    if (FAILED(state.Device->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &signatureData, sizeof(signatureData))))
        signatureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;

    D3D12_ROOT_SIGNATURE_FLAGS flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
    CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
    auto staticSamplers = GetStaticSamplers();
    rootSignatureDesc.Init_1_1(static_cast<UINT>(rootParams.size()), rootParams.data(), static_cast<UINT>(staticSamplers.size()), staticSamplers.data(), flags);

    ComPtr<ID3DBlob> signature;
    ComPtr<ID3DBlob> rootSignatureCreationError;
    HRESULT hr = D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, signatureData.HighestVersion, &signature, &rootSignatureCreationError);
    if (hr != S_OK)
        OutputDebugStringA(reinterpret_cast<char*>(rootSignatureCreationError->GetBufferPointer()));
    m_rootSignatures[handle] = Microsoft::WRL::ComPtr<ID3D12RootSignature>();
    hr = state.Device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&m_rootSignatures[handle]));
    if (hr != S_OK)
        OutputDebugStringA(reinterpret_cast<char*>(rootSignatureCreationError->GetBufferPointer()));
    ThrowIfFailed(hr);

    NAME_D3D12_OBJECT(m_rootSignatures[handle]);
}

ID3D12RootSignature* RootSignatureManager::GetRootSignature(ShaderHandle handle) const
{
    auto it = m_rootSignatures.find(handle);
    if (it != m_rootSignatures.cend())
        return it->second.Get();
    return nullptr;
}

}