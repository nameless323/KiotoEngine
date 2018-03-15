//
// Copyright (C) Alexandr Vorontsov. 2017
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#pragma once

#include <vector>

#include "Core/CoreTypes.h"

namespace Kioto::Renderer
{

enum class eVertexSemantic
{
    Position,
    Normal,
    Color,
    Texcoord,
    Tangent,
    Bitangent
};

enum class eDataFormat
{
    UNKNOWN,
    R8_G8_B8_A8,
    R8_G8_B8,
    R8_G8,
    R8,
    MATRIX3x3,
    MATRIX4x4
};

struct VertexDesc
{
    VertexDesc(eVertexSemantic semantic, uint8 semanticIndex, eDataFormat format, uint16 offset)
        : Offset(offset), Semantic(semantic), SemanticIndex(semanticIndex), Format(format)
    {}

    bool operator==(const VertexDesc& other) const;
    bool operator!=(const VertexDesc& other) const;

    uint16 Offset = 0;
    eVertexSemantic Semantic = eVertexSemantic::Position;
    uint8 SemanticIndex = 0;
    eDataFormat Format = eDataFormat::R8_G8_B8_A8;
};

class VertexLayout
{
public:
    VertexLayout() = default;
    VertexLayout(std::vector<VertexDesc> desc);
    VertexLayout(const VertexLayout& other);
    VertexLayout(VertexLayout&& other);
    VertexLayout& operator=(VertexLayout other);
    bool operator==(const VertexLayout& other) const;
    bool operator!=(const VertexLayout& other) const;

    void AddElement(eVertexSemantic semantic, uint8 semanticIndex, eDataFormat format);
    void Clear();
    std::vector<VertexDesc> GetElements() const;
    const VertexDesc* FindElement(eVertexSemantic semantic, uint8 semanticIndex) const;
    uint32 GetVertexStride() const;
    const VertexDesc& GetElement(uint32 i) const;
    uint32 GetElementsCount() const;

    friend void swap(VertexLayout& l, VertexLayout& r)
    {
        l.m_verticesDesc.swap(r.m_verticesDesc);
        std::swap(l.m_totalOffset, r.m_totalOffset);
    }

    static const VertexLayout LayoutPos3Norm3Uv2;
    static const VertexLayout LayoutPos3Norm3;
    static const VertexLayout LayoutPos3Norm3Tan3Bit3Uv2;
    static const VertexLayout LayoutPos3Norm3Tan3Bit3Uv2Col4;
    static const VertexLayout LayoutPos3Norm3Uv2Col4;
    static const VertexLayout LayoutPos3Uv2;

private:
    std::vector<VertexDesc> m_verticesDesc;
    uint32 m_totalOffset = 0;
};

inline std::vector<VertexDesc> VertexLayout::GetElements() const
{
    return m_verticesDesc;
}

inline uint32 VertexLayout::GetVertexStride() const
{
    return m_totalOffset;
}

inline const VertexDesc& VertexLayout::GetElement(uint32 i) const
{
    assert(i < m_verticesDesc.size());
    return m_verticesDesc[i];
}

inline uint32 VertexLayout::GetElementsCount() const
{
    return static_cast<uint32>(m_verticesDesc.size());
}

}