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
class Texture;
class TextureDX12;
struct StateDX;
namespace ShaderParser
{
struct MaterialData;
}

class TextureManagerDX12
{
public:
    ~TextureManagerDX12();
    void RegisterTexture(const StateDX& state, Texture* texture);
    void RegisterTextureWithoutOwnership(TextureDX12* texture);
    void UpdateTextureSetHeap(const StateDX& state, const TextureSet& texSet);

    ID3D12DescriptorHeap* GetTextureHeap(TextureSetHandle handle) const;
    TextureDX12* FindTexture(TextureHandle handle);

private:
    std::map<TextureSetHandle, Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>> m_textureHeaps; // [a_vorontsov] One tex heap for all textures?
    std::map<TextureHandle, TextureDX12*> m_textures;
    std::map<TextureHandle, TextureDX12*> m_notOwningTextures;
};
}