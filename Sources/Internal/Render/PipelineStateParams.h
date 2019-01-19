//
// Copyright (C) Aleksandr Vorontcov. 2017
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#pragma once

namespace Kioto
{
enum class eBlendModes
{
    Zero,
    One,
    SrcColor,
    OneMinusSrcColor,
    SrcAlpha,
    OneMinusSrcAlpha,
    DstAlpha,
    OneMinusDstAlpha,
    DstColor,
    OneMinusDstColor,
    SrcAlphaSat,
    COUNT
};

enum class eBlendOps
{
    Add,
    Substract,
    ReverseSubstract,
    Min,
    Max,
    COUNT
};

enum class eFillMode
{
    Solid,
    Wireframe,
    COUNT
};

enum class eColorMask
{
    Red = 1 << 0,
    Green = 1 << 1,
    Blue = 1 << 2,
    Alpha = 1 << 3,
    All = 1 << 4,
    COUNT = 0xFF
};

enum class eCullMode
{
    None,
    Front,
    Back,
    COUNT
};

enum class eZTest
{
    Never,
    Less,
    Equal,
    LEqual,
    Greater,
    NotEqual,
    GEqual,
    Always,
    COUNT
};

enum class eStencilOp
{
    Keep,
    Zero,
    Replace,
    IncrSat,
    DectSat,
    Invert,
    Incr,
    Decr,
    COUNT
};

enum class eStencilTest
{
    Never,
    Less,
    Equal,
    LEqual,
    Greater,
    NotEqual,
    GEqual,
    Always,
    COUNT
};

struct StencilDesc
{
    eStencilOp StencilFailOp = eStencilOp::Zero;
    eStencilOp StencilDepthFailOp = eStencilOp::Zero;
    eStencilOp StencilPassOp = eStencilOp::Zero;
    eStencilTest StencilFunc = eStencilTest::Never;
};
}