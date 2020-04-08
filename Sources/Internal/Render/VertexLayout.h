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

struct SemanticDesc
{
    SemanticDesc(eVertexSemantic semantic, uint8 semanticIndex, eDataFormat format, uint16 offset)
        : Offset(offset), Semantic(semantic), SemanticIndex(semanticIndex), Format(format)
    {}

    bool operator==(const SemanticDesc& other) const;
    bool operator!=(const SemanticDesc& other) const;

    uint16 Offset = 0;
    eVertexSemantic Semantic = eVertexSemantic::Position;
    uint8 SemanticIndex = 0;
    eDataFormat Format = eDataFormat::R8_G8_B8_A8;
};

class VertexLayout
{
public:
    VertexLayout() = default;
    VertexLayout(std::vector<SemanticDesc> desc);
    VertexLayout(const VertexLayout& other);
    VertexLayout(VertexLayout&& other);
    VertexLayout& operator=(VertexLayout other);
    bool operator==(const VertexLayout& other) const;
    bool operator!=(const VertexLayout& other) const;

    void AddElement(eVertexSemantic semantic, uint8 semanticIndex, eDataFormat format);
    void Clear();
    std::vector<SemanticDesc> GetElements() const;
    const SemanticDesc* FindElement(eVertexSemantic semantic, uint8 semanticIndex) const;
    uint32 GetVertexStride() const;
    const SemanticDesc& GetElement(uint32 i) const;
    uint32 GetElementsCount() const;

    friend void swap(VertexLayout& l, VertexLayout& r)
    {
        l.m_semanticsDesc.swap(r.m_semanticsDesc);
        std::swap(l.m_totalOffset, r.m_totalOffset);
    }

    static const VertexLayout LayoutPos3Norm3Uv2;
    static const VertexLayout LayoutPos3Norm3;
    static const VertexLayout LayoutPos3Norm3Tan3Bit3Uv2;
    static const VertexLayout LayoutPos3Norm3Tan3Bit3Uv2Col4;
    static const VertexLayout LayoutPos3Norm3Uv2Col4;
    static const VertexLayout LayoutPos3Uv2;

private:
    std::vector<SemanticDesc> m_semanticsDesc;
    uint32 m_totalOffset = 0;
};

inline std::vector<SemanticDesc> VertexLayout::GetElements() const
{
    return m_semanticsDesc;
}

inline uint32 VertexLayout::GetVertexStride() const
{
    return m_totalOffset;
}

inline const SemanticDesc& VertexLayout::GetElement(uint32 i) const
{
    assert(i < m_semanticsDesc.size());
    return m_semanticsDesc[i];
}

inline uint32 VertexLayout::GetElementsCount() const
{
    return static_cast<uint32>(m_semanticsDesc.size());
}
}