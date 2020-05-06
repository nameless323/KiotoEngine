#include "stdafx.h"

#include "Render/DX12/Texture/TextureManagerDX12.h"

#include "Render/DX12/StateDX.h"
#include "Render/DX12/Texture/TextureDX12.h"
#include "Render/Texture/Texture.h"
#include "Render/Texture/TextureSet.h"

namespace Kioto::Renderer
{
namespace
{
constexpr uint16 MaxRenderTargetViews = 256;
}

TextureManagerDX12::TextureManagerDX12()
{
    m_textureQueue.reserve(64);
    m_textureSetUpdateQueue.reserve(256);
}

void TextureManagerDX12::RegisterTexture(Texture* texture)
{
    auto it = m_textures.find(texture->GetHandle());
    if (it != m_textures.end())
    {
        throw "Texture Already Registered";
        return;
    }

    TextureDX12* tex = new TextureDX12();
    tex->Path = StrToWstr(texture->GetAssetPath());
    tex->SetHandle(GetNewHandle());
    tex->SetDescriptor(texture->GetDescriptor());
    tex->SetIsFromMemoryAsset(texture->IsMemoryAsset());
    m_textureQueue.push_back(tex);
    texture->SetHandle(tex->GetHandle());

    m_textures[tex->GetHandle()] = tex;
}

TextureManagerDX12::~TextureManagerDX12()
{
    for (auto& tex : m_textures)
        delete tex.second;
    m_textures.clear();
}

void TextureManagerDX12::InitRtvHeap(const StateDX& state)
{
    D3D12_DESCRIPTOR_HEAP_DESC heapDescr = {};
    heapDescr.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    heapDescr.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    heapDescr.NumDescriptors = MaxRenderTargetViews;
    ThrowIfFailed(state.Device->CreateDescriptorHeap(&heapDescr, IID_PPV_ARGS(&m_rtvHeap)));
}

void TextureManagerDX12::UpdateTextureSetHeap(const StateDX& state, const TextureSet& texSet)
{
    if (m_textureHeaps.find(texSet.GetHandle()) != m_textureHeaps.end())
        m_textureHeaps[texSet.GetHandle()].Reset();

    D3D12_DESCRIPTOR_HEAP_DESC heapDescr = {};
    heapDescr.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    heapDescr.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    heapDescr.NumDescriptors = texSet.GetTexturesCount();

    // [a_vorontcov] TODO: maybe reuse the same heap and overwrite descriptors?
    ThrowIfFailed(state.Device->CreateDescriptorHeap(&heapDescr, IID_PPV_ARGS(&m_textureHeaps[texSet.GetHandle()])));

    CD3DX12_CPU_DESCRIPTOR_HANDLE handle(m_textureHeaps[texSet.GetHandle()]->GetCPUDescriptorHandleForHeapStart());
    for (uint32 i = 0; i < texSet.GetTexturesCount(); ++i)
    {
        const Texture* kiotoTex = texSet.GetTexture(i);
        auto it = m_textures.find(kiotoTex->GetHandle());
        if (it == m_textures.end())
            throw "ololo";
        TextureDX12* dxTex = (*it).second;

        D3D12_SHADER_RESOURCE_VIEW_DESC texDescr = {};
        texDescr.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        texDescr.Format = dxTex->Resource->GetDesc().Format;
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
    auto it = m_textureHeaps.find(handle);
    if (it != m_textureHeaps.cend())
        return it->second.Get();
    return nullptr;
}

TextureDX12* TextureManagerDX12::FindTexture(TextureHandle handle)
{
    auto it = m_textures.find(handle);
    if (it != m_textures.cend())
        return it->second;
    it = m_notOwningTextures.find(handle);
    if (it != m_notOwningTextures.cend())
        return it->second;
    return nullptr;
}

void TextureManagerDX12::RegisterTextureWithoutOwnership(TextureDX12* texture)
{
    auto it = m_notOwningTextures.find(texture->GetHandle());
    if (it != m_notOwningTextures.end())
    {
        throw "Texture Already Registered";
        return;
    }
    m_notOwningTextures[texture->GetHandle()] = texture;
}

void TextureManagerDX12::ProcessRegistationQueue(const StateDX& state)
{
    for (auto& tex : m_textureQueue)
    {
        tex->Create(state.Device.Get(), state.CommandList.Get());
        if (tex->GetIsFromMemoryAsset() && ((tex->GetDx12TextureFlags() & D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET) != 0))
        {
            assert(m_rtvHeapOffsets.count(tex->GetHandle()) == 0);
            m_rtvHeapOffsets[tex->GetHandle()] = m_currentRtvOffset;

            CD3DX12_CPU_DESCRIPTOR_HANDLE handle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart());
            handle.Offset(m_currentRtvOffset);

            m_currentRtvOffset += state.RtvDescriptorSize;

            D3D12_RENDER_TARGET_VIEW_DESC texDescr = {};
            texDescr.Format = tex->Resource->GetDesc().Format;
            texDescr.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
            texDescr.Texture2D = { 0, 0};

            state.Device->CreateRenderTargetView(tex->Resource.Get(), &texDescr, handle);
        }
    }
    m_textureQueue.clear();
}

void TextureManagerDX12::QueueTextureSetForUpdate(const TextureSet& texSet)
{
    if (texSet.GetHandle() == InvalidHandle)
        throw "Invalid handle";

    auto it = std::find(m_textureSetUpdateQueue.cbegin(), m_textureSetUpdateQueue.cend(), &texSet);
    if (it != m_textureSetUpdateQueue.cend())
        return;

    m_textureSetUpdateQueue.push_back(&texSet);
}

void TextureManagerDX12::ProcessTextureSetUpdates(const StateDX& state)
{
    for (auto& texSet : m_textureSetUpdateQueue)
        UpdateTextureSetHeap(state, *texSet);
    m_textureSetUpdateQueue.clear();
}

D3D12_CPU_DESCRIPTOR_HANDLE TextureManagerDX12::GetRtvHandle(TextureHandle handle) const
{
    assert(m_rtvHeapOffsets.count(handle) == 1 && "The texture is not registered as a rtv");
    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart());
    rtvHandle.Offset(m_rtvHeapOffsets.at(handle));
    return rtvHandle;
}

}