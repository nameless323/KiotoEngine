//
// Copyright (C) Alexandr Vorontsov. 2017
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#include "Render/PipelineStateParams.h"
#include "Render/RenderLayer.h"

#pragma once

namespace Kioto::RenderParamsConverter
{

template <typename T>
T FromString(const std::string& s)
{}

template <>
BlendModes FromString<BlendModes>(const std::string& s);

template <>
RenderLayerType FromString<RenderLayerType>(const std::string& s);

template <>
BlendOps FromString<BlendOps>(const std::string& s);

template <>
FillMode FromString<FillMode>(const std::string& s);

template <>
ColorMask FromString<ColorMask>(const std::string& s);

template <>
CullMode FromString<CullMode>(const std::string& s);

template <>
ZTest FromString<ZTest>(const std::string& s);

template <>
StencilOp FromString<StencilOp>(const std::string& s);

template <>
StencilTest FromString<StencilTest>(const std::string& s);

}