//
// Copyright (C) Alexandr Vorontsov. 2017
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#include "Core/CoreTypes.h"

#include <string>

#pragma once

namespace Kioto::Renderer
{
class Shader;
class VertexLayout;
class Texture;
class TextureSet;

struct ShaderDefine
{
    std::string Name;
    uint32 Value;
};

constexpr uint32 InvalidHandle = -1;

template <typename T>
struct SafeHandle
{
public:
    SafeHandle() = default;
    SafeHandle(uint32 handle) : m_handle(handle)
    {
    }
    SafeHandle(const SafeHandle& other)
    {
        this->m_handle = other.m_handle;
    }
    SafeHandle& operator=(const SafeHandle& other)
    {
        this->m_handle = other.m_handle;
        return *this;
    }
    bool operator== (const SafeHandle& other) const
    {
        return m_handle == other.m_handle;
    }
    bool operator!= (const SafeHandle& other) const
    {
        return !(*this == other);
    }
    bool operator> (const SafeHandle& other) const
    {
        return m_handle > other.m_handle;
    }
    bool operator< (const SafeHandle& other) const
    {
        return m_handle < other.m_handle;
    }
    bool operator<= (const SafeHandle& other) const
    {
        return !(m_handle > other.m_handle);
    }
    bool operator>= (const SafeHandle& other) const
    {
        return !(m_handle < other.m_handle);
    }

    uint32 GetHandle() const
    {
        return m_handle;
    }

    void SetHandle(uint32 handle)
    {
        m_handle = handle;
    }

private:
    uint32 m_handle = InvalidHandle;
};

inline uint32 GetNewHandle()
{
    static uint32 handle;
    return handle++;
}

using VertexLayoutHandle = SafeHandle<VertexLayout>;
using ShaderHandle = SafeHandle<Shader>;
using TextureHandle = SafeHandle<Texture>;
using TextureSetHandle = SafeHandle<TextureSet>;
}
