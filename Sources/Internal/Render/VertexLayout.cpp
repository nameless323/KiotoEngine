//
// Copyright (C) Aleksandr Vorontcov. 2017
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#include "stdafx.h"

#include "Render/VertexLayout.h"

#include <map>

namespace Kioto::Renderer
{
static const std::map<const eDataFormat, uint16> formats
{
    { eDataFormat::UNKNOWN, 0 },
    { eDataFormat::R8_G8_B8_A8, 16 },
    { eDataFormat::R8_G8_B8, 12 },
    { eDataFormat::R8_G8, 8 },
    { eDataFormat::R8, 4 }
};


const VertexLayout VertexLayout::LayoutPos3Norm3Uv2
{
    std::vector<VertexDesc>
{
    { eVertexSemantic::Position, 0, eDataFormat::R8_G8_B8, 0 },
    { eVertexSemantic::Normal, 0, eDataFormat::R8_G8_B8, 12 },
    { eVertexSemantic::Texcoord, 0, eDataFormat::R8_G8, 24 }
}
};

const VertexLayout VertexLayout::LayoutPos3Norm3
{
    std::vector<VertexDesc>
{
    { eVertexSemantic::Position, 0, eDataFormat::R8_G8_B8, 0 },
    { eVertexSemantic::Normal, 0, eDataFormat::R8_G8_B8, 12 }
}
};

const VertexLayout VertexLayout::LayoutPos3Norm3Tan3Bit3Uv2
{
    std::vector<VertexDesc>
{
    { eVertexSemantic::Position, 0, eDataFormat::R8_G8_B8, 0 },
    { eVertexSemantic::Normal, 0, eDataFormat::R8_G8_B8, 12 },
    { eVertexSemantic::Tangent, 0, eDataFormat::R8_G8_B8, 24 },
    { eVertexSemantic::Bitangent, 0, eDataFormat::R8_G8_B8, 36 },
    { eVertexSemantic::Texcoord, 0, eDataFormat::R8_G8, 42 }
}
};

const VertexLayout VertexLayout::LayoutPos3Norm3Tan3Bit3Uv2Col4
{
    std::vector<VertexDesc>
{
    { eVertexSemantic::Position, 0, eDataFormat::R8_G8_B8, 0 },
    { eVertexSemantic::Normal, 0, eDataFormat::R8_G8_B8, 12 },
    { eVertexSemantic::Tangent, 0, eDataFormat::R8_G8_B8, 24 },
    { eVertexSemantic::Bitangent, 0, eDataFormat::R8_G8_B8, 36 },
    { eVertexSemantic::Texcoord, 0, eDataFormat::R8_G8, 48 },
    { eVertexSemantic::Color, 0, eDataFormat::R8_G8_B8_A8, 56 }
}
};

const VertexLayout VertexLayout::LayoutPos3Norm3Uv2Col4
{
    std::vector<VertexDesc>
{
    { eVertexSemantic::Position, 0, eDataFormat::R8_G8_B8, 0 },
    { eVertexSemantic::Normal, 0, eDataFormat::R8_G8_B8, 12 },
    { eVertexSemantic::Texcoord, 0, eDataFormat::R8_G8, 24 },
    { eVertexSemantic::Color, 0, eDataFormat::R8_G8_B8_A8, 32 }
}
};

const VertexLayout VertexLayout::LayoutPos3Uv2
{
    std::vector<VertexDesc>
{
    { eVertexSemantic::Position, 0, eDataFormat::R8_G8_B8, 0 },
    { eVertexSemantic::Texcoord, 0, eDataFormat::R8_G8, 12 }
}
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

VertexLayout::VertexLayout(std::vector<VertexDesc> desc)
{
    assert(desc.size() > 0);
    m_verticesDesc.swap(desc);
    for (const auto& descr : m_verticesDesc)
    {
        auto it = formats.find(descr.Format);
        assert(it != formats.end());
        m_totalOffset += it->second;
    }
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

void VertexLayout::Clear()
{
    m_verticesDesc.clear();
    m_totalOffset = 0;
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