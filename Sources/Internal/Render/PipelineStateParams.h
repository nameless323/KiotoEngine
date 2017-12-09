//
// Copyright (C) Alexandr Vorontsov. 2017
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#pragma once

namespace Kioto
{
enum class BlendModes
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

enum class BlendOps
{
    Add,
    Substract,
    ReverseSubstract,
    Min,
    Max,
    COUNT
};

enum class FillMode
{
    Solid,
    Wireframe,
    COUNT
};

enum class ColorMask
{
    Red = 1 << 0,
    Green = 1 << 1,
    Blue = 1 << 2,
    Alpha = 1 << 3,
    All = 1 << 4,
    COUNT = 0xFF
};

enum class CullMode
{
    None,
    Front,
    Back,
    COUNT
};

enum class ZTest
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

enum class StencilOp
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

enum class StencilTest
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
    StencilOp StencilFailOp = StencilOp::Zero;
    StencilOp StencilDepthFailOp = StencilOp::Zero;
    StencilOp StencilPassOp = StencilOp::Zero;
    StencilTest StencilFunc = StencilTest::Never;
};
}