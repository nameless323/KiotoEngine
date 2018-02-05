//
// Copyright (C) Alexandr Vorontsov. 2018
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#include "stdafx.h"

#include "Render/Texture/TextureManagerDX12.h"

#include "Render/Texture/Texture.h"
#include "Render/Texture/TextureDX12.h"
#include "Render/Texture/TextureSet.h"
#include "Render/DX12/StateDX.h"

namespace Kioto::Renderer
{

void TextureManagerDX12::RegisterTexture(const StateDX& state, Texture* texture)
{
    auto it = m_textures.find(texture->GetHandle());
    if (it != m_textures.end())
    {
        throw "Texture Already Registered";
        return;
    }

    TextureDX12* tex = new TextureDX12();
    tex->Path = StrToWstr(texture->GetAssetPath());
    tex->Create(state.Device.Get(), state.CommandList.Get());
    tex->SetTextureHandle(GetNewHandle());
    texture->SetHandle(tex->GetHandle());

    m_textures[tex->GetHandle()] = tex;
}

TextureManagerDX12::~TextureManagerDX12()
{
    for (auto& tex : m_textures)
        delete tex.second;
    m_textures.clear();
}

void TextureManagerDX12::UpdateTextureSetHeap(const StateDX& state, const TextureSet& texSet)
{
    if (m_textureHeaps.find(texSet.GetHandle()) != m_textureHeaps.end())
        m_textureHeaps[texSet.GetHandle()].Reset();

    D3D12_DESCRIPTOR_HEAP_DESC heapDescr = {};
    heapDescr.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    heapDescr.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    heapDescr.NumDescriptors = texSet.GetTexturesCount();
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

}