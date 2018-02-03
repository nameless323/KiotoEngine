//
// Copyright (C) Alexandr Vorontsov. 2018
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#pragma once

#include "Core/CoreTypes.h"
#include "Render/Texture/Texture.h"
#include "Render/RendererPublic.h"

#include <string>

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
    ~TextureSet() = default;
 
    eReturnCode AddTexture(const std::string& name, uint16 offset, Texture* texture);
    eReturnCode SetTexture(const std::string& name, Texture* texture);

    uint16 GetTexturesCount() const;
    uint16 GetMaxOffset() const;

    bool GetRebuildState() const;
    void SetRebuildState(bool newState);

    void SetHandle(TextureSetHandle handle);
    TextureSetHandle GetHandle() const;

    const Texture* GetTexture(uint32 index) const;

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
}