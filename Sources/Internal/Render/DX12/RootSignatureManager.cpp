#include "Render/Buffers/EngineBuffers.h"

#include "Render/DX12/RootSignatureManager.h"

#include "DXHelpers.h"
#include "Dx12Helpers/d3dx12.h"
#include "Render/DX12/StateDX.h"

namespace Kioto::Renderer
{
void RootSignatureManager::CreateRootSignature(const StateDX& state, const ShaderData& shaderData, const RenderObjectBufferLayout& bufferLayoutTemplate, const RenderObjectConstants& constants, ShaderHandle handle)
{
    using Microsoft::WRL::ComPtr;

    std::vector<CD3DX12_ROOT_PARAMETER1> rootParams;
    std::vector<D3D12_DESCRIPTOR_RANGE1> cbRange;
    cbRange.reserve(bufferLayoutTemplate.size());

    for (size_t i = 0; i < bufferLayoutTemplate.size(); ++i)
    {
        if (bufferLayoutTemplate[i].GetElemCount() == 1)
        {
            CD3DX12_ROOT_PARAMETER1 param;
            param.InitAsConstantBufferView(bufferLayoutTemplate[i].GetIndex(), bufferLayoutTemplate[i].GetSpace());
            rootParams.push_back(std::move(param));
        }
        else
        {
            cbRange.emplace_back();
            D3D12_DESCRIPTOR_RANGE1* bRange = &cbRange.back();
            bRange->NumDescriptors = bufferLayoutTemplate[i].GetElemCount();
            bRange->BaseShaderRegister = bufferLayoutTemplate[i].GetIndex();
            bRange->OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
            bRange->Flags = D3D12_DESCRIPTOR_RANGE_FLAG_NONE;
            bRange->RegisterSpace = bufferLayoutTemplate[i].GetSpace();
            bRange->RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;

            CD3DX12_ROOT_PARAMETER1 param;
            param.InitAsDescriptorTable(1, bRange, D3D12_SHADER_VISIBILITY_ALL);
            rootParams.push_back(std::move(param));
        }
    }

    for (size_t i = 0; i < constants.size(); ++i)
    {
        CD3DX12_ROOT_PARAMETER1 param;
        param.InitAsConstants(1, constants[i].GetIndex(), constants[i].GetSpace());
        rootParams.push_back(std::move(param));
    }

    D3D12_DESCRIPTOR_RANGE1 texRange;
    if (shaderData.textureSet.GetTexturesCount() > 0)
    {
        texRange.NumDescriptors = shaderData.textureSet.GetTexturesCount();
        texRange.BaseShaderRegister = 0;
        texRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
        texRange.Flags = D3D12_DESCRIPTOR_RANGE_FLAG_NONE;
        texRange.RegisterSpace = 0;
        texRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;

        CD3DX12_ROOT_PARAMETER1 table;
        table.InitAsDescriptorTable(1, &texRange, D3D12_SHADER_VISIBILITY_PIXEL); // [a_vorontcov] Could be anything. Need to add an annotation for it.
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