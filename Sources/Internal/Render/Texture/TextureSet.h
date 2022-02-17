#pragma once

#include <string>
#include <vector>

#include "Core/CoreTypes.h"
#include "Render/Texture/Texture.h"
#include "Render/RendererPublic.h"

namespace Kioto::Renderer
{
class TextureSet
{
public:
    enum class eReturnCode
    {
        Ok,
        AlreadyAdded,
        NotFound,
        TextureIsEmpty
    };

    TextureSet() = default;
    TextureSet(const TextureSet& other);
    TextureSet(TextureSet&& other);
    ~TextureSet() = default;
 
    TextureSet& operator= (TextureSet other);

    bool operator==(const TextureSet& other) const;
    bool operator!=(const TextureSet& other) const;

    eReturnCode AddTexture(const std::string& name, uint16 offset, Texture* texture);
    eReturnCode SetTexture(const std::string& name, Texture* texture);

    uint16 GetTexturesCount() const;
    uint16 GetMaxOffset() const;

    bool GetRebuildState() const;
    void SetRebuildState(bool newState);

    void SetHandle(TextureSetHandle handle);
    TextureSetHandle GetHandle() const;

    const Texture* GetTexture(uint32 index) const;
    const std::string* GetTextureName(uint32 index) const;
    uint16 GetTextureOffset(const std::string& name) const;
    uint16 GetTextureOffset(uint32 index) const;

    friend void swap(TextureSet& l, TextureSet& r)
    {
        l.mData.swap(r.mData);
        std::swap(l.mMaxOffset, r.mMaxOffset);
        std::swap(l.mNeedRebuild, r.mNeedRebuild);
        std::swap(l.mHandle, r.mHandle);
    }

private:
    struct TextureSetData
    {
        std::string Name;
        Texture* Texture;
        uint16 Offset;
    };

    bool Find(const std::string& name, TextureSetData*& data);

    std::vector<TextureSetData> mData;
    uint16 mMaxOffset = 0;
    bool mNeedRebuild = true;

    TextureSetHandle mHandle;
};

inline uint16 TextureSet::GetTexturesCount() const
{
    return static_cast<uint16>(mData.size());
}

inline uint16 TextureSet::GetMaxOffset() const
{
    return mMaxOffset;
}

inline bool TextureSet::GetRebuildState() const
{
    return mNeedRebuild;
}

inline void TextureSet::SetRebuildState(bool newState)
{
    mNeedRebuild = newState;
}

inline void TextureSet::SetHandle(TextureSetHandle handle)
{
    mHandle = handle;
}

inline TextureSetHandle TextureSet::GetHandle() const
{
    return mHandle;
}

inline const Texture* TextureSet::GetTexture(uint32 index) const
{
    return mData[index].Texture;
}

inline bool TextureSet::operator==(const TextureSet& other) const
{
    return mHandle == other.GetHandle();
}

inline bool TextureSet::operator!=(const TextureSet& other) const
{
    return !(*this == other);
}
}