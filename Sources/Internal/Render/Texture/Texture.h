#pragma once

#include "AssetsSystem/Asset.h"

#include "Render/RendererPublic.h"
#include "Render/ResourceStates.h"

namespace Kioto::Renderer
{
enum class eTextureFormat
{
    Format_UNKNOWN = 0,
    Format_R32G32B32A32_TYPELESS = 1,
    Format_R32G32B32A32_FLOAT = 2,
    Format_R32G32B32A32_UINT = 3,
    Format_R32G32B32A32_SINT = 4,
    Format_R32G32B32_TYPELESS = 5,
    Format_R32G32B32_FLOAT = 6,
    Format_R32G32B32_UINT = 7,
    Format_R32G32B32_SINT = 8,
    Format_R16G16B16A16_TYPELESS = 9,
    Format_R16G16B16A16_FLOAT = 10,
    Format_R16G16B16A16_UNORM = 11,
    Format_R16G16B16A16_UINT = 12,
    Format_R16G16B16A16_SNORM = 13,
    Format_R16G16B16A16_SINT = 14,
    Format_R32G32_TYPELESS = 15,
    Format_R32G32_FLOAT = 16,
    Format_R32G32_UINT = 17,
    Format_R32G32_SINT = 18,
    Format_R32G8X24_TYPELESS = 19,
    Format_D32_FLOAT_S8X24_UINT = 20,
    Format_R32_FLOAT_X8X24_TYPELESS = 21,
    Format_X32_TYPELESS_G8X24_UINT = 22,
    Format_R10G10B10A2_TYPELESS = 23,
    Format_R10G10B10A2_UNORM = 24,
    Format_R10G10B10A2_UINT = 25,
    Format_R11G11B10_FLOAT = 26,
    Format_R8G8B8A8_TYPELESS = 27,
    Format_R8G8B8A8_UNORM = 28,
    Format_R8G8B8A8_UNORM_SRGB = 29,
    Format_R8G8B8A8_UINT = 30,
    Format_R8G8B8A8_SNORM = 31,
    Format_R8G8B8A8_SINT = 32,
    Format_R16G16_TYPELESS = 33,
    Format_R16G16_FLOAT = 34,
    Format_R16G16_UNORM = 35,
    Format_R16G16_UINT = 36,
    Format_R16G16_SNORM = 37,
    Format_R16G16_SINT = 38,
    Format_R32_TYPELESS = 39,
    Format_D32_FLOAT = 40,
    Format_R32_FLOAT = 41,
    Format_R32_UINT = 42,
    Format_R32_SINT = 43,
    Format_R24G8_TYPELESS = 44,
    Format_D24_UNORM_S8_UINT = 45,
    Format_R24_UNORM_X8_TYPELESS = 46,
    Format_X24_TYPELESS_G8_UINT = 47,
    Format_R8G8_TYPELESS = 48,
    Format_R8G8_UNORM = 49,
    Format_R8G8_UINT = 50,
    Format_R8G8_SNORM = 51,
    Format_R8G8_SINT = 52,
    Format_R16_TYPELESS = 53,
    Format_R16_FLOAT = 54,
    Format_D16_UNORM = 55,
    Format_R16_UNORM = 56,
    Format_R16_UINT = 57,
    Format_R16_SNORM = 58,
    Format_R16_SINT = 59,
    Format_R8_TYPELESS = 60,
    Format_R8_UNORM = 61,
    Format_R8_UINT = 62,
    Format_R8_SNORM = 63,
    Format_R8_SINT = 64,
    Format_A8_UNORM = 65,
    Format_R1_UNORM = 66,
    Format_R9G9B9E5_SHAREDEXP = 67,
    Format_R8G8_B8G8_UNORM = 68,
    Format_G8R8_G8B8_UNORM = 69,
    Format_BC1_TYPELESS = 70,
    Format_BC1_UNORM = 71,
    Format_BC1_UNORM_SRGB = 72,
    Format_BC2_TYPELESS = 73,
    Format_BC2_UNORM = 74,
    Format_BC2_UNORM_SRGB = 75,
    Format_BC3_TYPELESS = 76,
    Format_BC3_UNORM = 77,
    Format_BC3_UNORM_SRGB = 78,
    Format_BC4_TYPELESS = 79,
    Format_BC4_UNORM = 80,
    Format_BC4_SNORM = 81,
    Format_BC5_TYPELESS = 82,
    Format_BC5_UNORM = 83,
    Format_BC5_SNORM = 84,
    Format_B5G6R5_UNORM = 85,
    Format_B5G5R5A1_UNORM = 86,
    Format_B8G8R8A8_UNORM = 87,
    Format_B8G8R8X8_UNORM = 88,
    Format_R10G10B10_XR_BIAS_A2_UNORM = 89,
    Format_B8G8R8A8_TYPELESS = 90,
    Format_B8G8R8A8_UNORM_SRGB = 91,
    Format_B8G8R8X8_TYPELESS = 92,
    Format_B8G8R8X8_UNORM_SRGB = 93,
    Format_BC6H_TYPELESS = 94,
    Format_BC6H_UF16 = 95,
    Format_BC6H_SF16 = 96,
    Format_BC7_TYPELESS = 97,
    Format_BC7_UNORM = 98,
    Format_BC7_UNORM_SRGB = 99,
    Format_AYUV = 100,
    Format_Y410 = 101,
    Format_Y416 = 102,
    Format_NV12 = 103,
    Format_P010 = 104,
    Format_P016 = 105,
    Format_420_OPAQUE = 106,
    Format_YUY2 = 107,
    Format_Y210 = 108,
    Format_Y216 = 109,
    Format_NV11 = 110,
    Format_AI44 = 111,
    Format_IA44 = 112,
    Format_P8 = 113,
    Format_A8P8 = 114,
    Format_B4G4R4A4_UNORM = 115,
    Format_P208 = 130,
    Format_V208 = 131,
    Format_V408 = 132,
    Format_FORCE_UINT = -1
};

enum class eTextureDim
{
    Texture2D
};

struct TextureDescriptor
{
    eTextureFormat Format = eTextureFormat::Format_UNKNOWN;
    eTextureDim Dimension = eTextureDim::Texture2D;
    eResourceState InitialState = eResourceState::Common;
    uint32 Width = 0;
    uint32 Height = 0;
};

class Texture : public Asset
{
public:
    Texture();
    Texture(TextureDescriptor descriptor);
    Texture(const std::string& path) : Asset(path) {}

    TextureHandle GetHandle() const;
    void SetHandle(TextureHandle handle);

private:
    TextureHandle m_handle;

    TextureDescriptor m_descriptor;
    eResourceState m_currentState = eResourceState::Common;
};

inline Texture::Texture(TextureDescriptor descriptor)
{
    std::swap(m_descriptor, descriptor);
    m_currentState = m_descriptor.InitialState;
}

inline TextureHandle Texture::GetHandle() const
{
    return m_handle;
}

inline void Texture::SetHandle(TextureHandle handle)
{
    m_handle = handle;
}

}