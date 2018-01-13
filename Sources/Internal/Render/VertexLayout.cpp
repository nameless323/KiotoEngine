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
    using namespace std;
    swap(m_verticesDesc, other.m_verticesDesc);
    swap(m_totalOffset, other.m_totalOffset);
}

VertexLayout& VertexLayout::operator=(VertexLayout other)
{
    using namespace std;
    swap(m_verticesDesc, other.m_verticesDesc);
    swap(m_totalOffset, other.m_totalOffset);
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

}