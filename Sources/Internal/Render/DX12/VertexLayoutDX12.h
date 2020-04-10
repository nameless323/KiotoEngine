#pragma once

#include <dxgiformat.h>
#include <map>

#include "Core/CoreTypes.h"
#include "Render/VertexLayout.h"

namespace Kioto::Renderer
{
static inline std::map<eVertexSemantic, std::string> SemanticNames =
{
    { eVertexSemantic::Position, "POSITION" },
    { eVertexSemantic::Normal, "NORMAL" },
    { eVertexSemantic::Texcoord, "TEXCOORD" },
    { eVertexSemantic::Color, "COLOR" },
    { eVertexSemantic::Tangent, "TANGENT" },
    { eVertexSemantic::Bitangent, "BITANGENT" }
};

static inline std::map<eDataFormat, DXGI_FORMAT> VertexDataFormats =
{
    { eDataFormat::R8_G8_B8_A8, DXGI_FORMAT_R32G32B32A32_FLOAT },
    { eDataFormat::R8_G8_B8, DXGI_FORMAT_R32G32B32_FLOAT },
    { eDataFormat::R8_G8, DXGI_FORMAT_R32G32_FLOAT },
    { eDataFormat::R8, DXGI_FORMAT_R32_FLOAT}
};

struct VertexLayoutDX12
{
    VertexLayoutHandle Handle;
    VertexLayout LayoutKioto;
    std::vector<D3D12_INPUT_ELEMENT_DESC> LayoutDX;
};
}