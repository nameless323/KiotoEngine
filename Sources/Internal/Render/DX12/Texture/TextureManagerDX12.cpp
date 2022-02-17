#include "Render/DX12/Texture/TextureManagerDX12.h"

#include "Core/CoreHelpers.h"
#include "Dx12Helpers/d3dx12.h"
#include "Render/DX12/DXHelpers.h"
#include "Render/DX12/StateDX.h"
#include "Render/DX12/Texture/TextureDX12.h"
#include "Render/Texture/Texture.h"
#include "Render/Texture/TextureSet.h"

namespace Kioto::Renderer
{
namespace
{
constexpr uint16 MaxRenderTargetViews = 256;
constexpr uint16 MaxDepthStencilViews = 32;
}

TextureManagerDX12::TextureManagerDX12()
{
    mTextureQueue.reserve(64);
    mTextureSetUpdateQueue.reserve(256);
}

void TextureManagerDX12::RegisterTexture(Texture* texture)
{
    auto it = mTextures.find(texture->GetHandle());
    if (it != mTextures.end())
    {
        throw "Texture Already Registered";
        return;
    }

    TextureDX12* tex = new TextureDX12();
    tex->Path = StrToWstr(texture->GetAssetPath());
    tex->SetHandle(GetNewHandle());
    tex->SetDescriptor(texture->GetDescriptor());
    tex->SetIsFromMemoryAsset(texture->IsMemoryAsset());
    mTextureQueue.push_back(tex);
    texture->SetHandle(tex->GetHandle());

    mTextures[tex->GetHandle()] = tex;
}

TextureManagerDX12::~TextureManagerDX12()
{
    for (auto& tex : mTextures)
        delete tex.second;
    mTextures.clear();
}

void TextureManagerDX12::InitRtvHeap(const StateDX& state)
{
    D3D12_DESCRIPTOR_HEAP_DESC heapDescr = {};
    heapDescr.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    heapDescr.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    heapDescr.NumDescriptors = MaxRenderTargetViews;
    ThrowIfFailed(state.Device->CreateDescriptorHeap(&heapDescr, IID_PPV_ARGS(&mRtvHeap)));
}

void TextureManagerDX12::InitDsvHeap(const StateDX& state)
{
    D3D12_DESCRIPTOR_HEAP_DESC heapDescr = {};
    heapDescr.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    heapDescr.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
    heapDescr.NumDescriptors = MaxDepthStencilViews;
    ThrowIfFailed(state.Device->CreateDescriptorHeap(&heapDescr, IID_PPV_ARGS(&mDsvHeap)));
}

void TextureManagerDX12::UpdateTextureSetHeap(const StateDX& state, const TextureSet& texSet)
{
    if (mTextureHeaps.find(texSet.GetHandle()) != mTextureHeaps.end())
        mTextureHeaps[texSet.GetHandle()].Reset();

    D3D12_DESCRIPTOR_HEAP_DESC heapDescr = {};
    heapDescr.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    heapDescr.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    heapDescr.NumDescriptors = texSet.GetTexturesCount();

    // [a_vorontcov] TODO: maybe reuse the same heap and overwrite descriptors?
    ThrowIfFailed(state.Device->CreateDescriptorHeap(&heapDescr, IID_PPV_ARGS(&mTextureHeaps[texSet.GetHandle()])));


    CD3DX12_CPU_DESCRIPTOR_HANDLE handle(mTextureHeaps[texSet.GetHandle()]->GetCPUDescriptorHandleForHeapStart());
    for (uint32 i = 0; i < texSet.GetTexturesCount(); ++i)
    {
        const Texture* kiotoTex = texSet.GetTexture(i);
        auto it = mTextures.find(kiotoTex->GetHandle());
        if (it == mTextures.end())
            throw "ololo";
        TextureDX12* dxTex = (*it).second;

        D3D12_SHADER_RESOURCE_VIEW_DESC texDescr = {};
        texDescr.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        bool isDepth = dxTex->Resource->GetDesc().Format == DXGI_FORMAT_R24G8_TYPELESS;
        texDescr.Format = isDepth ? DXGI_FORMAT_R24_UNORM_X8_TYPELESS : dxTex->Resource->GetDesc().Format;
        texDescr.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
        texDescr.Texture2D.MipLevels = dxTex->Resource->GetDesc().MipLevels;
        texDescr.Texture2D.MostDetailedMip = 0;
        texDescr.Texture2D.ResourceMinLODClamp = 0.0f;

        state.Device->CreateShaderResourceView(dxTex->Resource.Get(), &texDescr, handle);
        handle.Offset(state.CbvSrvUavDescriptorSize);
    }
}

ID3D12DescriptorHeap* TextureManagerDX12::GetTextureHeap(TextureSetHandle handle) const
{
    auto it = mTextureHeaps.find(handle);
    if (it != mTextureHeaps.cend())
        return it->second.Get();
    return nullptr;
}

TextureDX12* TextureManagerDX12::FindTexture(TextureHandle handle)
{
    auto it = mTextures.find(handle);
    if (it != mTextures.cend())
        return it->second;
    it = mNotOwningTextures.find(handle);
    if (it != mNotOwningTextures.cend())
        return it->second;
    return nullptr;
}

void TextureManagerDX12::RegisterTextureWithoutOwnership(TextureDX12* texture)
{
    auto it = mNotOwningTextures.find(texture->GetHandle());
    if (it != mNotOwningTextures.end())
    {
        throw "Texture Already Registered";
        return;
    }
    mNotOwningTextures[texture->GetHandle()] = texture;
}

void TextureManagerDX12::ProcessRegistationQueue(const StateDX& state)
{
    for (auto& tex : mTextureQueue)
    {
        tex->Create(state.Device.Get(), state.CommandList.Get());
        if (tex->GetIsFromMemoryAsset())
        {
            if ((tex->GetDx12TextureFlags() & D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET) != 0)
            {
                assert(mRtvHeapOffsets.count(tex->GetHandle()) == 0);
                mRtvHeapOffsets[tex->GetHandle()] = mCurrentRtvOffset;

                CD3DX12_CPU_DESCRIPTOR_HANDLE handle(mRtvHeap->GetCPUDescriptorHandleForHeapStart());
                handle.Offset(mCurrentRtvOffset);

                mCurrentRtvOffset += state.RtvDescriptorSize;

                D3D12_RENDER_TARGET_VIEW_DESC texDescr = {};
                texDescr.Format = tex->Resource->GetDesc().Format;
                texDescr.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
                texDescr.Texture2D = { 0, 0 };

                state.Device->CreateRenderTargetView(tex->Resource.Get(), &texDescr, handle);
            }
            else if ((tex->GetDx12TextureFlags() & D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL) != 0)
            {
                assert(mDsvHeapOffsets.count(tex->GetHandle()) == 0);
                mDsvHeapOffsets[tex->GetHandle()] = mCurrentDsvOffset;

                CD3DX12_CPU_DESCRIPTOR_HANDLE handle(mDsvHeap->GetCPUDescriptorHandleForHeapStart());
                handle.Offset(mCurrentDsvOffset);

                mCurrentDsvOffset += state.DsvDescriptorSize;

                D3D12_DEPTH_STENCIL_VIEW_DESC texDescr = {};
                texDescr.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; //tex->Resource->GetDesc().Format; todo: [a_vorontcov] fix it
                texDescr.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
                texDescr.Texture2D = { 0 };

                state.Device->CreateDepthStencilView(tex->Resource.Get(), &texDescr, handle);
            }
        }
    }
    mTextureQueue.clear();
}

void TextureManagerDX12::QueueTextureSetForUpdate(const TextureSet& texSet)
{
    if (texSet.GetHandle() == InvalidHandle)
        throw "Invalid handle";

    auto it = std::find(mTextureSetUpdateQueue.cbegin(), mTextureSetUpdateQueue.cend(), &texSet);
    if (it != mTextureSetUpdateQueue.cend())
        return;

    mTextureSetUpdateQueue.push_back(&texSet);
}

void TextureManagerDX12::ProcessTextureSetUpdates(const StateDX& state)
{
    for (auto& texSet : mTextureSetUpdateQueue)
        UpdateTextureSetHeap(state, *texSet);
    mTextureSetUpdateQueue.clear();
}

D3D12_CPU_DESCRIPTOR_HANDLE TextureManagerDX12::GetRtvHandle(TextureHandle handle) const
{
    assert(mRtvHeapOffsets.count(handle) == 1 && "The texture is not registered as a rtv");
    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(mRtvHeap->GetCPUDescriptorHandleForHeapStart());
    rtvHandle.Offset(mRtvHeapOffsets.at(handle));
    return rtvHandle;
}

D3D12_CPU_DESCRIPTOR_HANDLE TextureManagerDX12::GetDsvHandle(TextureHandle handle) const
{
    assert(mDsvHeapOffsets.count(handle) == 1 && "The texture is not registred as a dsv");
    CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle(mDsvHeap->GetCPUDescriptorHandleForHeapStart());
    dsvHandle.Offset(mDsvHeapOffsets.at(handle));
    return dsvHandle;
}

}