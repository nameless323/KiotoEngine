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

enum class eVertexDataFormat
{
    R32_G32_B32_A32,
    R32_G32_B32,
    R32_G32,
    R32
};

struct VertexDesc
{
    VertexDesc(eVertexSemantic semantic, uint8 semanticIndex, eVertexDataFormat format, uint16 offset)
        : Offset(offset), Semantic(semantic), SemanticIndex(semanticIndex), Format(format)
    {}

    uint16 Offset = 0;
    eVertexSemantic Semantic = eVertexSemantic::Position;
    uint8 SemanticIndex = 0;
    eVertexDataFormat Format = eVertexDataFormat::R32_G32_B32_A32;
};

class VertexLayout
{
public:
    void AddElement(eVertexSemantic semantic, uint8 semanticIndex, eVertexDataFormat format);
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