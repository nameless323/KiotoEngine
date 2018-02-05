//
// Copyright (C) Alexandr Vorontsov. 2017
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#pragma once

#include <string>
#include <d3d12.h>
#include <wrl.h>

#include "Render/Texture/Texture.h"

namespace Kioto::Renderer
{
class TextureDX12
{
public:
    std::wstring Path;
    Microsoft::WRL::ComPtr<ID3D12Resource> Resource = nullptr; // [a_vorontsov] Change to ResourceDX12.
    Microsoft::WRL::ComPtr<ID3D12Resource> UploadResource = nullptr;

    void Create(ID3D12Device* device, ID3D12GraphicsCommandList* commandList);

    void SetTextureHandle(TextureHandle handle);
    TextureHandle GetHandle() const;

    static DXGI_FORMAT ToDXGIFormat(eTextureFormat format);

private:
    TextureHandle m_handle;
};

inline DXGI_FORMAT TextureDX12::ToDXGIFormat(eTextureFormat format)
{
    return static_cast<DXGI_FORMAT>(format);
}

inline void TextureDX12::SetTextureHandle(TextureHandle handle)
{
    m_handle = handle;
}

inline TextureHandle TextureDX12::GetHandle() const
{
    return m_handle;
}
}