#include "stdafx.h"

#include <map>
#include <string>

#include "AssetsSystem/RenderStateParamsConverter.h"

namespace Kioto::RenderParamsConverter
{
const std::map<const std::string, eBlendModes> blendModes = // [a_vorontcov] To lower maybe.
{
    { "Zero", eBlendModes::Zero },
    { "One", eBlendModes::One },
    { "SrcColor", eBlendModes::SrcColor },
    { "OneMinusSrcColor", eBlendModes::OneMinusSrcColor },
    { "SrcAlpha", eBlendModes::SrcAlpha },
    { "OneMinusSrcAlpha", eBlendModes::OneMinusSrcAlpha },
    { "DstAlpha", eBlendModes::DstAlpha },
    { "OneMinusDstAlpha", eBlendModes::OneMinusDstAlpha },
    { "DstColor", eBlendModes::DstColor },
    { "OneMinusDstColor", eBlendModes::OneMinusDstColor },
    { "SrcAlphaSat", eBlendModes::SrcAlphaSat }
};

const std::map<const std::string, eRenderLayerType> layerTypes =
{
    { "Opaque", eRenderLayerType::Opaque },
    { "Transparent", eRenderLayerType::Transparent }
};

const std::map<const std::string, eBlendOps> blendOps =
{
    { "Add", eBlendOps::Add },
    { "Substract", eBlendOps::Substract },
    { "ReverseSubstract", eBlendOps::ReverseSubstract },
    { "Min", eBlendOps::Min },
    { "Max", eBlendOps::Max }
};

const std::map<const std::string, eFillMode> fillMode =
{
    { "Solid", eFillMode::Solid },
    { "Wireframe", eFillMode::Wireframe }
};

const std::map<const std::string, eColorMask> colorMask =
{
    { "Red", eColorMask::Red },
    { "Green", eColorMask::Green },
    { "Blue", eColorMask::Blue },
    { "Alpha", eColorMask::Alpha },
    { "All", eColorMask::All }
};

const std::map<const std::string, eCullMode> cullMode =
{
    { "None", eCullMode::None },
    { "Front", eCullMode::Front },
    { "Back", eCullMode::Back }
};

const std::map<const std::string, eZTest> ZTestMode =
{
    { "Never", eZTest::Never },
    { "Less", eZTest::Less },
    { "Equal", eZTest::Equal },
    { "LEqual", eZTest::LEqual },
    { "Greater", eZTest::Greater },
    { "NotEqual", eZTest::NotEqual },
    { "GEqual", eZTest::GEqual },
    { "Always", eZTest::Always }
};

const std::map<const std::string, eStencilOp> stencilOp =
{
    { "Keep", eStencilOp::Keep },
    { "Zero", eStencilOp::Zero },
    { "Replace", eStencilOp::Replace },
    { "IncrSat", eStencilOp::IncrSat },
    { "DectSat", eStencilOp::DectSat },
    { "Invert", eStencilOp::Invert },
    { "Incr", eStencilOp::Incr },
    { "Decr", eStencilOp::Decr }
};

const std::map<const std::string, eStencilTest> stencilTest =
{
    { "Never", eStencilTest::Never },
    { "Less", eStencilTest::Less },
    { "Equal", eStencilTest::Equal },
    { "LEqual", eStencilTest::LEqual },
    { "Greater", eStencilTest::Greater },
    { "NotEqual", eStencilTest::NotEqual },
    { "GEqual", eStencilTest::GEqual },
    { "Always", eStencilTest::Always }
};

template <>
eBlendModes FromString<eBlendModes>(const std::string& s)
{
    auto it = blendModes.find(s);
    if (it != blendModes.end())
        return it->second;
    throw "Wrong mode";
    return eBlendModes::COUNT;
}

template <>
eRenderLayerType FromString<eRenderLayerType>(const std::string& s)
{
    auto it = layerTypes.find(s);
    if (it != layerTypes.end())
        return it->second;
    throw "Wrong type";
    return eRenderLayerType::COUNT;
}

template <>
eBlendOps FromString<eBlendOps>(const std::string& s)
{
    auto it = blendOps.find(s);
    if (it != blendOps.end())
        return it->second;
    throw "Wrong operation";
    return eBlendOps::COUNT;
}

template <>
eFillMode FromString<eFillMode>(const std::string& s)
{
    auto it = fillMode.find(s);
    if (it != fillMode.end())
        return it->second;
    throw "Wrong fill mode";
    return eFillMode::COUNT;
}

template <>
eColorMask FromString<eColorMask>(const std::string& s)
{
    auto it = colorMask.find(s);
    if (it != colorMask.end())
        return it->second;
    throw "Wrong color mask";
    return eColorMask::COUNT;
}

template <>
eCullMode FromString<eCullMode>(const std::string& s)
{
    auto it = cullMode.find(s);
    if (it != cullMode.end())
        return it->second;
    throw "Wrong cull mode";
    return eCullMode::COUNT;
}

template <>
eZTest FromString<eZTest>(const std::string& s)
{
    auto it = ZTestMode.find(s);
    if (it != ZTestMode.end())
        return it->second;
    throw "Wrong ZTest mode";
    return eZTest::COUNT;
}

template <>
eStencilOp FromString<eStencilOp>(const std::string& s)
{
    auto it = stencilOp.find(s);
    if (it != stencilOp.end())
        return it->second;
    throw "Wrong Stencil operation";
    return eStencilOp::COUNT;
}

template <>
eStencilTest FromString<eStencilTest>(const std::string& s)
{
    auto it = stencilTest.find(s);
    if (it != stencilTest.end())
        return it->second;
    throw "Wrong Stencil test";
    return eStencilTest::COUNT;
}
}