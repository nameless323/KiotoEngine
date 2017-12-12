//
// Copyright (C) Alexandr Vorontsov. 2017
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#include "stdafx.h"

#include "Render/VertexLayout.h"

#include <map>

namespace Kioto::Renderer
{
static const std::map<const eVertexDataFormat, uint16> formats
{
    { eVertexDataFormat::R32_G32_B32_A32, 16},
    { eVertexDataFormat::R32_G32_B32, 12 },
    { eVertexDataFormat::R32_G32, 8 },
    { eVertexDataFormat::R32, 4 }
};

void VertexLayout::AddElement(eVertexSemantic semantic, uint8 semanticIndex, eVertexDataFormat format)
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