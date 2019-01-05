//
// Copyright (C) Aleksandr Vorontcov. 2018
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#pragma once

#include <d3d12.h>
#include <map>
#include <vector>
#include <wrl/client.h>

#include "Render/RendererPublic.h"

namespace Kioto::Renderer
{
class Texture;
class TextureDX12;
struct StateDX;
namespace ShaderParser
{
struct ShaderData;
}

class TextureManagerDX12
{
public:
    TextureManagerDX12();
    ~TextureManagerDX12();
    void RegisterTexture(Texture* texture);
    void RegisterTextureWithoutOwnership(TextureDX12* texture);
    void ProcessRegistationQueue(const StateDX& state);
    void UpdateTextureSetHeap(const StateDX& state, const TextureSet& texSet);
    void QueueTextureSetForUpdate(const TextureSet& texSet); // TODO: need material handles.
    void ProcessTextureSetUpdates(const StateDX& state);

    ID3D12DescriptorHeap* GetTextureHeap(TextureSetHandle handle) const;
    TextureDX12* FindTexture(TextureHandle handle);

private:
    std::map<TextureSetHandle, Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>> m_textureHeaps; // [a_vorontcov] One tex heap for all textures?
    std::vector<TextureDX12*> m_textureQueue;
    std::vector<const TextureSet*> m_textureSetUpdateQueue;
    std::map<TextureHandle, TextureDX12*> m_textures;
    std::map<TextureHandle, TextureDX12*> m_notOwningTextures;
};
}