//
// Copyright (C) Alexandr Vorontsov. 2017
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#pragma once

#include <dxgiformat.h>
#include <map>

#include "Core/CoreTypes.h"
#include "Render/VertexLayout.h"

namespace Kioto::Renderer
{
std::map<eVertexSemantic, std::string> SemanticNames =
{
    { eVertexSemantic::Position, "POSIITON" },
    { eVertexSemantic::Normal, "NORMAL" },
    { eVertexSemantic::Texcoord, "TEXCOORD" },
    { eVertexSemantic::Color, "COLOR" },
    { eVertexSemantic::Tangent, "TANGENT" },
    { eVertexSemantic::Bitangent, "BITANGENT" }
};

std::map<eVertexDataFormat, DXGI_FORMAT> VertexDataFormats =
{
    { eVertexDataFormat::R32_G32_B32_A32, DXGI_FORMAT_R32G32B32A32_FLOAT },
    { eVertexDataFormat::R32_G32_B32, DXGI_FORMAT_R32G32B32_FLOAT },
    { eVertexDataFormat::R32_G32, DXGI_FORMAT_R32G32_FLOAT },
    { eVertexDataFormat::R32, DXGI_FORMAT_R32_FLOAT}
};
}