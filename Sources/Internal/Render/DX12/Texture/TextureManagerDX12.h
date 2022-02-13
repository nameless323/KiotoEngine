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
    void InitRtvHeap(const StateDX& state);
    void InitDsvHeap(const StateDX& state);
    void UpdateTextureSetHeap(const StateDX& state, const TextureSet& texSet);
    void QueueTextureSetForUpdate(const TextureSet& texSet); // TODO: need material handles.
    void ProcessTextureSetUpdates(const StateDX& state);

    ID3D12DescriptorHeap* GetTextureHeap(TextureSetHandle handle) const;
    TextureDX12* FindTexture(TextureHandle handle);

    D3D12_CPU_DESCRIPTOR_HANDLE GetRtvHandle(TextureHandle handle) const;
    D3D12_CPU_DESCRIPTOR_HANDLE GetDsvHandle(TextureHandle handle) const;

private:
    std::map<TextureSetHandle, Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>> mTextureHeaps; // [a_vorontcov] TODO: One tex heap for all textures

    std::map<TextureHandle, uint16> mRtvHeapOffsets;
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mRtvHeap;
    uint16 mCurrentRtvOffset = 0;

    std::map<TextureHandle, uint16> mDsvHeapOffsets;
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mDsvHeap;
    uint16 mCurrentDsvOffset = 0;

    std::vector<TextureDX12*> mTextureQueue;
    std::vector<const TextureSet*> mTextureSetUpdateQueue;

    std::map<TextureHandle, TextureDX12*> mTextures;
    std::map<TextureHandle, TextureDX12*> mNotOwningTextures;
};
}