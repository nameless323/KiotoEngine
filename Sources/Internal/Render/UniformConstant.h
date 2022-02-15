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
    std::string mName;
    uint32 mValue;
    uint16 mIndex;
    uint16 mSpace;
};

inline UniformConstant::UniformConstant(const std::string& name, uint16 index, uint16 space)
    : mName(name)
    , mIndex(index)
    , mSpace(space)
    , mValue(0)
{
}

template <typename T>
inline void Renderer::UniformConstant::SetValue(T val)
{
    assert(sizeof(T) == 4);
    mValue = *reinterpret_cast<uint32*>(&val);
}

inline uint32 UniformConstant::GetValue() const
{
    return mValue;
}

inline uint16 UniformConstant::GetIndex() const
{
    return mIndex;
}

inline uint16 UniformConstant::GetSpace() const
{
    return mSpace;
}

inline const std::string& UniformConstant::GetName() const
{
    return mName;
}

}