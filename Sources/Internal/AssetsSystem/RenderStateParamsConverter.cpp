//
// Copyright (C) Alexandr Vorontsov. 2017
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#include "stdafx.h"

#include <map>
#include <string>

#include "AssetsSystem/RenderStateParamsConverter.h"

namespace Kioto::RenderParamsConverter
{
const std::map<const std::string, BlendModes> blendModes = // [a_vorontsov] To lower maybe.
{
    { "Zero", BlendModes::Zero },
    { "One", BlendModes::One },
    { "SrcColor", BlendModes::SrcColor },
    { "OneMinusSrcColor", BlendModes::OneMinusSrcColor },
    { "SrcAlpha", BlendModes::SrcAlpha },
    { "OneMinusSrcAlpha", BlendModes::OneMinusSrcAlpha },
    { "DstAlpha", BlendModes::DstAlpha },
    { "OneMinusDstAlpha", BlendModes::OneMinusDstAlpha },
    { "DstColor", BlendModes::DstColor },
    { "OneMinusDstColor", BlendModes::OneMinusDstColor },
    { "SrcAlphaSat", BlendModes::SrcAlphaSat }
};

const std::map<const std::string, RenderLayerType> layerTypes =
{
    { "Opaque", RenderLayerType::Opaque },
    { "Transparent", RenderLayerType::Transparent }
};

const std::map<const std::string, BlendOps> blendOps =
{
    { "Add", BlendOps::Add },
    { "Substract", BlendOps::Substract },
    { "ReverseSubstract", BlendOps::ReverseSubstract },
    { "Min", BlendOps::Min },
    { "Max", BlendOps::Max }
};

const std::map<const std::string, FillMode> fillMode =
{
    { "Solid", FillMode::Solid },
    { "Wireframe", FillMode::Wireframe }
};

const std::map<const std::string, ColorMask> colorMask =
{
    { "Red", ColorMask::Red },
    { "Green", ColorMask::Green },
    { "Blue", ColorMask::Blue },
    { "Alpha", ColorMask::Alpha },
    { "All", ColorMask::All }
};

const std::map<const std::string, CullMode> cullMode =
{
    { "None", CullMode::None },
    { "Front", CullMode::Front },
    { "Back", CullMode::Back }
};

const std::map<const std::string, ZTest> ZTestMode =
{
    { "Never", ZTest::Never },
    { "Less", ZTest::Less },
    { "Equal", ZTest::Equal },
    { "LEqual", ZTest::LEqual },
    { "Greater", ZTest::Greater },
    { "NotEqual", ZTest::NotEqual },
    { "GEqual", ZTest::GEqual },
    { "Always", ZTest::Always }
};

const std::map<const std::string, StencilOp> stencilOp =
{
    { "Keep", StencilOp::Keep },
    { "Zero", StencilOp::Zero },
    { "Replace", StencilOp::Replace },
    { "IncrSat", StencilOp::IncrSat },
    { "DectSat", StencilOp::DectSat },
    { "Invert", StencilOp::Invert },
    { "Incr", StencilOp::Incr },
    { "Decr", StencilOp::Decr }
};

const std::map<const std::string, StencilTest> stencilTest =
{
    { "Never", StencilTest::Never },
    { "Less", StencilTest::Less },
    { "Equal", StencilTest::Equal },
    { "LEqual", StencilTest::LEqual },
    { "Greater", StencilTest::Greater },
    { "NotEqual", StencilTest::NotEqual },
    { "GEqual", StencilTest::GEqual },
    { "Always", StencilTest::Always }
};

template <>
BlendModes FromString<BlendModes>(const std::string& s)
{
    auto it = blendModes.find(s);
    if (it != blendModes.end())
        return it->second;
    throw "Wrong mode";
    return BlendModes::COUNT;
}

template <>
RenderLayerType FromString<RenderLayerType>(const std::string& s)
{
    auto it = layerTypes.find(s);
    if (it != layerTypes.end())
        return it->second;
    throw "Wrong type";
    return RenderLayerType::COUNT;
}

template <>
BlendOps FromString<BlendOps>(const std::string& s)
{
    auto it = blendOps.find(s);
    if (it != blendOps.end())
        return it->second;
    throw "Wrong operation";
    return BlendOps::COUNT;
}

template <>
FillMode FromString<FillMode>(const std::string& s)
{
    auto it = fillMode.find(s);
    if (it != fillMode.end())
        return it->second;
    throw "Wrong fill mode";
    return FillMode::COUNT;
}

template <>
ColorMask FromString<ColorMask>(const std::string& s)
{
    auto it = colorMask.find(s);
    if (it != colorMask.end())
        return it->second;
    throw "Wrong color mask";
    return ColorMask::COUNT;
}

template <>
CullMode FromString<CullMode>(const std::string& s)
{
    auto it = cullMode.find(s);
    if (it != cullMode.end())
        return it->second;
    throw "Wrong cull mode";
    return CullMode::COUNT;
}

template <>
ZTest FromString<ZTest>(const std::string& s)
{
    auto it = ZTestMode.find(s);
    if (it != ZTestMode.end())
        return it->second;
    throw "Wrong ZTest mode";
    return ZTest::COUNT;
}

template <>
StencilOp FromString<StencilOp>(const std::string& s)
{
    auto it = stencilOp.find(s);
    if (it != stencilOp.end())
        return it->second;
    throw "Wrong Stencil operation";
    return StencilOp::COUNT;
}

template <>
StencilTest FromString<StencilTest>(const std::string& s)
{
    auto it = stencilTest.find(s);
    if (it != stencilTest.end())
        return it->second;
    throw "Wrong Stencil test";
    return StencilTest::COUNT;
}
}