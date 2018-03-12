//
// Copyright (C) Alexandr Vorontsov. 2017
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#include "stdafx.h"

#include "Render/VertexLayout.h"

#include <map>

namespace Kioto::Renderer
{
static const std::map<const eDataFormat, uint16> formats
{
    { eDataFormat::R32_G32_B32_A32, 16},
    { eDataFormat::R32_G32_B32, 12 },
    { eDataFormat::R32_G32, 8 },
    { eDataFormat::R32, 4 }
};

VertexLayout::VertexLayout(const VertexLayout& other)
{
    m_verticesDesc = other.m_verticesDesc;
    m_totalOffset = other.m_totalOffset;
}

VertexLayout::VertexLayout(VertexLayout&& other)
{
    swap(*this, other);
}

VertexLayout& VertexLayout::operator=(VertexLayout other)
{
    swap(*this, other);
    return *this;
}

void VertexLayout::AddElement(eVertexSemantic semantic, uint8 semanticIndex, eDataFormat format)
{
    m_verticesDesc.emplace_back(semantic, semanticIndex, format, m_totalOffset);
    auto it = formats.find(format);
    if (it != formats.end())
        m_totalOffset += it->second;
}

void VertexLayout::CleanElements()
{
    m_verticesDesc.clear();
}

bool VertexLayout::operator==(const VertexLayout& other) const
{
    if (other.m_verticesDesc.size() != m_verticesDesc.size())
        return false;
    for (uint32 i = 0; i < m_verticesDesc.size(); ++i)
    {
        if (m_verticesDesc[i] != other.m_verticesDesc[i])
            return false;
    }
    return true;
}

bool VertexLayout::operator!=(const VertexLayout& other) const
{
    return !(*this == other);
}

const VertexDesc* VertexLayout::FindElement(eVertexSemantic semantic, uint8 semanticIndex) const
{
    for (const auto& desc : m_verticesDesc)
    {
        if (desc.Semantic == semantic && desc.SemanticIndex == semanticIndex)
            return &desc;
    }
    return nullptr;
}

bool VertexDesc::operator==(const VertexDesc& other) const
{
    return Semantic == other.Semantic && SemanticIndex == other.SemanticIndex && Format == other.Format;
}

bool VertexDesc::operator!=(const VertexDesc& other) const
{
    return !(*this == other);
}

}