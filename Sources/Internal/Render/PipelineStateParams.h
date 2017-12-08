//
// Copyright (C) Alexandr Vorontsov. 2017
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#pragma once

namespace Kioto::Renderer
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
    Red,
    Green,
    Blue,
    Alpha,
    All
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
}