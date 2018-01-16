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
    VertexLayout(const VertexLayout& other);
    VertexLayout(VertexLayout&& other);
    VertexLayout& operator=(VertexLayout other);
    bool operator==(const VertexLayout& other) const;
    bool operator!=(const VertexLayout& other) const;

    void AddElement(eVertexSemantic semantic, uint8 semanticIndex, eDataFormat format);
    void CleanElements();
    std::vector<VertexDesc> GetElements() const;

private:
    std::vector<VertexDesc> m_verticesDesc;
    uint16 m_totalOffset = 0;
};

inline std::vector<VertexDesc> VertexLayout::GetElements() const
{
    return m_verticesDesc;
}

}