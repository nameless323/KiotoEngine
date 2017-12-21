//
// Copyright (C) Alexandr Vorontsov. 2017
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#include "Core/CoreTypes.h"

#pragma once

namespace Kioto::Renderer
{
class Shader;
class VertexLayout;
class Texture;

constexpr uint32 InvalidHandle = -1;

template <typename T>
struct SafeHandle
{
public:
    SafeHandle() = default;
    SafeHandle(uint32 handle) : Handle(handle)
    {
    }
    SafeHandle(const SafeHandle& other)
    {
        this->Handle = other.Handle;
    }
    SafeHandle& operator=(const SafeHandle& other)
    {
        this->Handle = other.Handle;
        return *this;
    }

    uint32 Handle = InvalidHandle;
};

using VertexLayoutHandle = SafeHandle<VertexLayout>;
using ShaderHandle = SafeHandle<Shader>;
using TextureHandle = SafeHandle<Texture>;
}
