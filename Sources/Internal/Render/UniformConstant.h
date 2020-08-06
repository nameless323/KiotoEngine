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

    uint16 GetIndex() const;
    uint16 GetSpace() const;

    const std::string& GetName() const;

private:
    std::string m_name;
    uint32 m_value;
    uint16 m_index;
    uint16 m_space;
};

inline UniformConstant::UniformConstant(const std::string& name, uint16 index, uint16 space)
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
    m_value = *reinterpret_cast<uint32*>(&val);
}

inline uint32 UniformConstant::GetValue() const
{
    return m_value;
}

inline uint16 UniformConstant::GetIndex() const
{
    return m_index;
}

inline uint16 UniformConstant::GetSpace() const
{
    return m_space;
}

inline const std::string& UniformConstant::GetName() const
{
    return m_name;
}

}