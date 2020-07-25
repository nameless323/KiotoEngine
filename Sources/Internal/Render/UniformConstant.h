#pragma once

#include <string>
#include <variant>

#include "Core/CoreTypes.h"

namespace Kioto::Renderer
{
struct UniformConstant
{
public:
    UniformConstant(const std::string& name, uint16 index, uint16 space);

    template <typename T>
    void SetValue(T val);

    uint32 GetValue() const;

private:
    std::string m_name;
    uint32 m_value;
    uint16 m_index;
    uint16 m_space;
};

UniformConstant::UniformConstant(const std::string& name, uint16 index, uint16 space)
    : m_name(name)
    , m_index(index)
    , m_space(space)
    , m_value(0)
{
}

template <typename T>
inline void Renderer::UniformConstant::SetValue(T val)
{
    assert(sizeof(T) == 4);
    m_value = *reinterpret_cast(uint32*)(val);
}

uint32 UniformConstant::GetValue() const
{
    return m_value;
}

}