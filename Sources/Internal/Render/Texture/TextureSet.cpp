//
// Copyright (C) Aleksandr Vorontcov. 2018
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#include "stdafx.h"

#include "Render/Texture/TextureSet.h"
#include "Render/Renderer.h"

namespace Kioto::Renderer
{
TextureSet::TextureSet(const TextureSet& other)
    : m_data(other.m_data)
    , m_maxOffset(other.m_maxOffset)
    , m_needRebuild(other.m_needRebuild)
    , m_handle(other.m_handle)
{
}

TextureSet::TextureSet(TextureSet&& other)
{
    swap(*this, other);
}

TextureSet& TextureSet::operator=(TextureSet other)
{
    swap(*this, other);
    return *this;
}

TextureSet::eReturnCode TextureSet::AddTexture(const std::string& name, uint16 offset, Texture* texture)
{
    TextureSetData* data = nullptr;
    if (Find(name, data))
        return eReturnCode::AlreadyAdded;
    TextureSetData newElem;
    newElem.Name = name;
    newElem.Texture = texture;
    newElem.Offset = offset;
    m_data.push_back(std::move(newElem));
    return eReturnCode::Ok;
}

TextureSet::eReturnCode TextureSet::SetTexture(const std::string& name, Texture* texture)
{
    TextureSetData* data = nullptr;
    if (!Find(name, data))
        return eReturnCode::NotFound;
    data->Texture = texture;
    Renderer::QueueTextureSetForUpdate(*this);
    return eReturnCode::Ok;
}

bool TextureSet::Find(const std::string& name, TextureSetData*& data)
{
    auto it = std::find_if(m_data.begin(), m_data.end(), [&name](const TextureSetData& d) { return d.Name == name; });
    if (it == m_data.end())
        return false;
    data = &(*it);
    return true;
}
}