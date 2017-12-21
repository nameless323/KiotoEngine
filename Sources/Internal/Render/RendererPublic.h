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

    uint32 Handle = -1;
};

using VertexLayoutHandle = SafeHandle<VertexLayout>;
using ShaderHandle = SafeHandle<Shader>;
}
