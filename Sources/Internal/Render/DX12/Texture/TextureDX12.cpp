#include "stdafx.h"

#include "Render/DX12/Texture/TextureDX12.h"

#include "AssetsSystem/FilesystemHelpers.h"
#include "Core/CoreHelpers.h"
#include "Render/DX12/KiotoDx12Mapping.h"
#include "Sources/External/Dx12Helpers/DDSTextureLoader.h"
#include "Render/Color.h"

#include "Render/DX12/DXHelpers.h"

namespace Kioto::Renderer
{
void TextureDX12::Create(ID3D12Device* device, ID3D12GraphicsCommandList* commandList)
{
    if (m_fromMemoryAsset)
    {
        CreateFromDescriptor(device, commandList);
        SetName(Resource.Get(), Kioto::StrToWstr(m_descriptor.Name).c_str());
    }
    else
    {
        CreateFromFile(device, commandList);
        SetName(Resource.Get(), FilesystemHelpers::GetFilenameFromPath(Path).c_str());
    }
}

void TextureDX12::CreateFromFile(ID3D12Device* device, ID3D12GraphicsCommandList* commandList)
{
    HRESULT texRes = DirectX::CreateDDSTextureFromFile12(device, commandList, Path.c_str(), Resource, UploadResource);
    ThrowIfFailed(texRes);
    m_currentState = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
}

void TextureDX12::CreateFromDescriptor(ID3D12Device* device, ID3D12GraphicsCommandList* commandList)
{
#ifdef _DEBUG
    if (!m_isDescriptorInitialized)
        assert("Initialize texture descriptor before creating the texture with the descriptor" && false);
#endif
    D3D12_RESOURCE_DESC textureDesc = {};
    textureDesc.MipLevels = 1;
    textureDesc.Format = KiotoDx12Mapping::ResourceFormats[m_descriptor.Format];
    textureDesc.Width = m_descriptor.Width;
    textureDesc.Height = m_descriptor.Height;

    for (const auto& flag : FlagsArray)
    {
        if (((uint16)m_descriptor.Flags & (uint16)flag) != 0)
            m_textureFlags |= KiotoDx12Mapping::ResourceFlags[m_descriptor.Flags];
    }
    textureDesc.Flags = m_textureFlags;
    textureDesc.DepthOrArraySize = 1;
    textureDesc.SampleDesc.Count = 1;
    textureDesc.SampleDesc.Quality = 0;
    textureDesc.Dimension = KiotoDx12Mapping::ResourceDimensions[m_descriptor.Dimension];

    D3D12_CLEAR_VALUE clearValue;
    if (m_descriptor.FastClear)
    {
        clearValue.Format = textureDesc.Format;
        memcpy(clearValue.Color, std::get<Color>(m_descriptor.FastClearValue).data, sizeof(float32) * 4);
    }

    ThrowIfFailed(device->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
        D3D12_HEAP_FLAG_NONE,
        &textureDesc,
        D3D12_RESOURCE_STATE_COPY_DEST,
        m_descriptor.FastClear ? &clearValue : nullptr,
        IID_PPV_ARGS(&Resource)));
    m_currentState = D3D12_RESOURCE_STATE_COPY_DEST;
}

}
