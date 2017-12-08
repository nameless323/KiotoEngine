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
    SrcAlphaSat
};

enum class BlendOps
{
    Add,
    Substract,
    ReverseSubstract,
    Min,
    Max
};

enum class FillMode
{
    Solid,
    Wireframe
};

enum class ColorMask
{
    Red = 1 << 0,
    Green = 1 << 1,
    Blue = 1 << 2,
    Alpha = 1 << 3,
    All = 1 << 4
};

enum class CullMode
{
    None,
    Front,
    Back
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
    Always
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
    Decr
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
    Always
};

struct StencilDesc
{
    StencilOp StencilFailOp = StencilOp::Zero;
    StencilOp StencilDepthFailOp = StencilOp::Zero;
    StencilOp StencilPassOp = StencilOp::Zero;
    StencilTest StencilFunc = StencilTest::Never;
};
}