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
    R32_G32_B32_A32,
    R32_G32_B32,
    R32_G32,
    R32,
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
    eDataFormat Format = eDataFormat::R32_G32_B32_A32;
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

    friend void swap(VertexLayout& l, VertexLayout& r)
    {
        l.m_verticesDesc.swap(r.m_verticesDesc);
        std::swap(l.m_totalOffset, r.m_totalOffset);
    }

    static const VertexLayout LayoutPos3Uv2Norm3;
    static const VertexLayout LayoutPos3Norm3;
    static const VertexLayout LayoutPos3Uv2Norm3Tan3Bit3;
    static const VertexLayout LayoutPos3Uv2Norm3Tan3Bit3Col4;
    static const VertexLayout LayoutPos3Uv2Norm3Col4;
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

}