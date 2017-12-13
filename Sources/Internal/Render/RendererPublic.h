//
// Copyright (C) Alexandr Vorontsov. 2017
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#include "Core/CoreTypes.h"

#include "Render/VertexLayout.h"

#pragma once

namespace Kioto::Renderer
{
using Handle = uint32; // [a_vorontsov] Maybe some template class to some type safe.

template <typename T>
struct SafeHandle
{
public:
    SafeHandle() = default;
    SafeHandle(const SafeHandle& other)
    {
        this->Handle = other.Handle;
    }
    SafeHandle& operator=(const SafeHandle& other)
    {
        this->Handle = other.Handle;
        return this;
    }

    Handle Handle = 0;
};

using VertexLayoutHandle = SafeHandle<VertexLayout>;
const Handle InvalidHandle = static_cast<Handle>(-1);
}
