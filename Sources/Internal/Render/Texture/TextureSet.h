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
        l.m_data.swap(r.m_data);
        std::swap(l.m_maxOffset, r.m_maxOffset);
        std::swap(l.m_needRebuild, r.m_needRebuild);
        std::swap(l.m_handle, r.m_handle);
    }

private:
    struct TextureSetData
    {
        std::string Name;
        Texture* Texture;
        uint16 Offset;
    };

    bool Find(const std::string& name, TextureSetData*& data);

    std::vector<TextureSetData> m_data;
    uint16 m_maxOffset = 0;
    bool m_needRebuild = true;

    TextureSetHandle m_handle;
};

inline uint16 TextureSet::GetTexturesCount() const
{
    return static_cast<uint16>(m_data.size());
}

inline uint16 TextureSet::GetMaxOffset() const
{
    return m_maxOffset;
}

inline bool TextureSet::GetRebuildState() const
{
    return m_needRebuild;
}

inline void TextureSet::SetRebuildState(bool newState)
{
    m_needRebuild = newState;
}

inline void TextureSet::SetHandle(TextureSetHandle handle)
{
    m_handle = handle;
}

inline TextureSetHandle TextureSet::GetHandle() const
{
    return m_handle;
}

inline const Texture* TextureSet::GetTexture(uint32 index) const
{
    return m_data[index].Texture;
}

inline bool TextureSet::operator==(const TextureSet& other) const
{
    return m_handle == other.GetHandle();
}

inline bool TextureSet::operator!=(const TextureSet& other) const
{
    return !(*this == other);
}
}