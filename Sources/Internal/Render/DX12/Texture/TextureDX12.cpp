#include "Render/DX12/Texture/TextureDX12.h"

#include <variant>

#include "AssetsSystem/FilesystemHelpers.h"
#include "Core/CoreHelpers.h"
#include "Math/Vector2.h"
#include "Render/DX12/KiotoDx12Mapping.h"
#include "Sources/External/Dx12Helpers/DDSTextureLoader.h"
#include "Render/Color.h"

#include "Render/DX12/DXHelpers.h"

namespace Kioto::Renderer
{
void TextureDX12::Create(ID3D12Device* device, ID3D12GraphicsCommandList* commandList)
{
    if (mFromMemoryAsset)
    {
        CreateFromDescriptor(device, commandList);
        SetName(Resource.Get(), Kioto::StrToWstr(mDescriptor.Name).c_str());
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
    mCurrentState = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
}

void TextureDX12::CreateFromDescriptor(ID3D12Device* device, ID3D12GraphicsCommandList* commandList)
{
#ifdef _DEBUG
    if (!mIsDescriptorInitialized)
        assert("Initialize texture descriptor before creating the texture with the descriptor" && false);
#endif
    D3D12_RESOURCE_DESC textureDesc = {};
    textureDesc.MipLevels = 1;
    textureDesc.Format = KiotoDx12Mapping::ResourceFormats[mDescriptor.Format];
    textureDesc.Width = mDescriptor.Width;
    textureDesc.Height = mDescriptor.Height;

    for (const auto& flag : FlagsArray)
    {
        if (((uint16)mDescriptor.Flags & (uint16)flag) != 0)
            mTextureFlags |= KiotoDx12Mapping::ResourceFlags[mDescriptor.Flags];
    }
    textureDesc.Flags = mTextureFlags;
    textureDesc.DepthOrArraySize = 1;
    textureDesc.SampleDesc.Count = 1;
    textureDesc.SampleDesc.Quality = 0;
    textureDesc.Dimension = KiotoDx12Mapping::ResourceDimensions[mDescriptor.Dimension];

    D3D12_CLEAR_VALUE clearValue;
    if (mDescriptor.FastClear)
    {
        clearValue.Format = textureDesc.Format;
        if ((textureDesc.Flags & D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL) != 0)
        {
            clearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; // todo: [a_vorontcov] Again kinda hackish
            Vector2 depthStencilClear = std::get<Vector2>(mDescriptor.FastClearValue);
            clearValue.DepthStencil.Depth = depthStencilClear.x;
            clearValue.DepthStencil.Stencil = static_cast<uint8>(depthStencilClear.y);
        }
        else
            memcpy(clearValue.Color, std::get<Color>(mDescriptor.FastClearValue).data, sizeof(float32) * 4);
    }

    auto defaultHeapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
    ThrowIfFailed(device->CreateCommittedResource(
        &defaultHeapProps,
        D3D12_HEAP_FLAG_NONE,
        &textureDesc,
        D3D12_RESOURCE_STATE_COPY_DEST,
        mDescriptor.FastClear ? &clearValue : nullptr,
        IID_PPV_ARGS(&Resource)));
    mCurrentState = D3D12_RESOURCE_STATE_COPY_DEST;
}

}
