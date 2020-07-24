#pragma once

#include <vector>
#include <variant>

#include "Core/CoreTypes.h"
#include "Core/CoreHelpers.h"

#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"
#include "Math/Matrix3.h"
#include "Math/Matrix4.h"

#include "Render/RendererPublic.h"
#include "Render/Renderer.h"

namespace Kioto::Renderer
{
struct ConstantBufferSet
{
    ConstantBufferSetHandle Handle;
    std::vector<ConstantBufferHandle> BuffersSet;
};

class ConstantBuffer
{
public:
    ConstantBuffer() {}
    ConstantBuffer(std::string name, uint16 index, uint16 space, uint16 elemSize, uint16 elemCount = 1, bool allocate = false);
    ConstantBuffer(std::string name, uint16 index, uint16 space);
    template <typename T>
    void Set(const T& val, uint16 elemOffset = 0, bool updateHWinstance = true);
    template <typename T>
    T* Get(uint16 elemOffset = 0);
    void Reallocate();

    ConstantBuffer(const ConstantBuffer& other);
    ConstantBuffer(ConstantBuffer&& other);
    ~ConstantBuffer();

    ConstantBuffer& operator= (ConstantBuffer other);
    template <typename T>
    T* GetBufferData();
    byte* GetBufferData();
    uint32 GetDataSize() const;
    uint32 GetElemCount() const;
    const std::string& GetName() const;
    template <typename T>
    void SetElemCount(uint32 count, bool reallocate = false);
    bool IsAllocated() const;

    ConstantBufferHandle GetHandle() const;
    void SetHandle(ConstantBufferHandle handle);
    void ScheduleToUpdate();

    uint16 GetIndex() const;
    uint16 GetSpace() const;
    uint32 GetKey() const;

    void MakeShallowCopy(ConstantBuffer& target, bool queueForUpdate = true) const; // [a_vorontcov] Space and key. Doesn't copy memory itself.

private:
    uint16 m_index = 0;
    uint16 m_space = 0;
    uint32 m_key = 0;
    bool m_isAllocated = false;

    byte* m_memData = nullptr;
    uint32 m_dataSize = 0;
    uint32 m_elemCount = 0;
    uint32 m_elemSize = 0;
    std::string m_name;

    ConstantBufferHandle m_handle;

    friend void swap(ConstantBuffer& l, ConstantBuffer& r)
    {
        std::swap(l.m_index, r.m_index);
        std::swap(l.m_space, r.m_space);
        std::swap(l.m_key, r.m_key);
        std::swap(l.m_isAllocated, r.m_isAllocated);
        std::swap(l.m_memData, r.m_memData);
        std::swap(l.m_dataSize, r.m_dataSize);
        std::swap(l.m_handle, r.m_handle);
        std::swap(l.m_elemCount, r.m_elemCount);
        std::swap(l.m_elemSize, r.m_elemSize);
        std::swap(l.m_name, r.m_name);
    }
};

inline uint32 ConstantBuffer::GetElemCount() const
{
    return m_elemCount;
}

inline const std::string& ConstantBuffer::GetName() const
{
    return m_name;
}

template <typename T>
inline void ConstantBuffer::SetElemCount(uint32 count, bool reallocate)
{
    m_elemCount = count;
    SafeDeleteArray(m_memData);
    m_isAllocated = false;
    m_elemSize = sizeof(T);
    m_dataSize = m_elemCount * m_elemSize;
    if (reallocate)
        Reallocate();
}

inline ConstantBuffer::ConstantBuffer(std::string name, uint16 index, uint16 space, uint16 elemSize, uint16 elemCount, bool allocate)
    : m_name(std::move(name))
    , m_index(index)
    , m_space(space)
    , m_key(m_index | m_space << 16)
    , m_dataSize(elemSize * elemCount)
    , m_elemCount(elemCount)
    , m_elemSize(elemSize)
{
    if (allocate)
    {
        m_memData = new byte[m_dataSize];
        m_isAllocated = true;
    }
}

inline ConstantBuffer::ConstantBuffer(std::string name, uint16 index, uint16 space)
    : m_name(std::move(name))
    , m_index(index)
    , m_space(space)
    , m_key(m_index | m_space << 16)
{
}

template <typename T>
inline void ConstantBuffer::Set(const T& val, uint16 elemOffset, bool updateHWinstance)
{
    T* mem = reinterpret_cast<T*>(m_memData);
    *(mem + elemOffset) = val;

    if (updateHWinstance)
        ScheduleToUpdate();
}

template <typename T>
inline T* ConstantBuffer::Get(uint16 elemOffset)
{
    T* mem = reinterpret_cast<T*>(m_memData);
    return mem + elemOffset;
}

inline uint16 ConstantBuffer::GetIndex() const
{
    return m_index;
}

inline uint16 ConstantBuffer::GetSpace() const
{
    return m_space;
}

inline uint32 ConstantBuffer::GetKey() const
{
    return m_key;
}

template <typename T>
inline T* ConstantBuffer::GetBufferData()
{
    return reinterpret_cast<T*>(m_memData);
}

inline byte* ConstantBuffer::GetBufferData()
{
    return m_memData;
}

inline uint32 ConstantBuffer::GetDataSize() const
{
    return m_dataSize;
}

inline bool ConstantBuffer::IsAllocated() const
{
    return m_isAllocated;
}

inline ConstantBufferHandle ConstantBuffer::GetHandle() const
{
    return m_handle;
}

inline void ConstantBuffer::SetHandle(ConstantBufferHandle handle)
{
    m_handle = handle;
}

inline void ConstantBuffer::Reallocate()
{
    SafeDeleteArray(m_memData);
    m_memData = new byte[m_dataSize];
    m_isAllocated = true;
}

inline ConstantBuffer::~ConstantBuffer()
{
    SafeDeleteArray(m_memData);
}

inline ConstantBuffer::ConstantBuffer(const ConstantBuffer& other)
    : m_name(other.m_name)
    , m_index(other.m_index)
    , m_space(other.m_space)
    , m_key(other.m_key)
    , m_isAllocated(other.m_isAllocated)
    , m_dataSize(other.m_dataSize)
    , m_elemSize(other.m_elemSize)
    , m_elemCount(other.m_elemCount)
{
    if (other.IsAllocated())
        memcpy(m_memData, other.m_memData, other.m_dataSize);
}

inline ConstantBuffer::ConstantBuffer(ConstantBuffer&& other)
{
    swap(*this, other);
}

inline ConstantBuffer& ConstantBuffer::operator=(ConstantBuffer other)
{
    swap(*this, other);
    return *this;
}

inline void ConstantBuffer::MakeShallowCopy(ConstantBuffer& target, bool queueForUpdate) const
{
    target.m_index = m_index;
    target.m_space = m_space;
    target.m_key = m_key;
    target.m_elemCount = m_elemCount;
    target.m_elemSize = m_elemSize;
    target.m_dataSize = m_dataSize;
    target.m_name = m_name;
    target.m_isAllocated = false;
}

inline void ConstantBuffer::ScheduleToUpdate()
{
    Renderer::QueueConstantBufferForUpdate(*this);
}
}