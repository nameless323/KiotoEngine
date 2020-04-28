#include "stdafx.h"

#include "Render/DX12/Texture/TextureDX12.h"

#include "Render/DX12/KiotoDx12Mapping.h"
#include "Sources/External/Dx12Helpers/DDSTextureLoader.h"

namespace Kioto::Renderer
{
void TextureDX12::Create(ID3D12Device* device, ID3D12GraphicsCommandList* commandList)
{
    if (Path.empty())
    {
        CreateFromDescriptor(device, commandList);
    }
    else
    {
        CreateFromFile(device, commandList);
    }
}

void TextureDX12::CreateFromFile(ID3D12Device* device, ID3D12GraphicsCommandList* commandList)
{
    HRESULT texRes = DirectX::CreateDDSTextureFromFile12(device, commandList, Path.c_str(), Resource, UploadResource);
    ThrowIfFailed(texRes);
}

void TextureDX12::CreateFromDescriptor(ID3D12Device* device, ID3D12GraphicsCommandList* commandList)
{
    D3D12_RESOURCE_DESC textureDesc = {};
    textureDesc.MipLevels = 1;
    textureDesc.Format = KiotoDx12Mapping::ResourceFormats[m_descriptor.Format];
    textureDesc.Width = m_descriptor.Width;
    textureDesc.Height = m_descriptor.Height;
    textureDesc.Flags = KiotoDx12Mapping::ResourceFlags[m_descriptor.Flags];
    textureDesc.DepthOrArraySize = 1;
    textureDesc.SampleDesc.Count = 1;
    textureDesc.SampleDesc.Quality = 0;
    textureDesc.Dimension = KiotoDx12Mapping::ResourceDimensions[m_descriptor.Dimension];

    ThrowIfFailed(device->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
        D3D12_HEAP_FLAG_NONE,
        &textureDesc,
        D3D12_RESOURCE_STATE_COPY_DEST,
        nullptr,
        IID_PPV_ARGS(&Resource)));
}

}
