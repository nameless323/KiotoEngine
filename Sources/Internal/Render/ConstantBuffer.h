#pragma once

#include <vector>
#include <variant>

#include "Core/CoreTypes.h"

#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"
#include "Math/Matrix3.h"
#include "Math/Matrix4.h"

#include "Render/RendererPublic.h"

namespace Kioto::Renderer
{
struct ConstantBufferSet
{
    ConstantBufferSetHandle Handle;
    std::vector<ConstantBufferHandle> BuffersSet;
};

template <typename T>
class ConstantBuffer
{
public:
    enum class eReturnCode
    {
        Ok,
        AlreadyAdded,
        NotFound,
        TypesDontMatch
    };

    ConstantBuffer() {}
    ConstantBuffer(uint16 index, uint16 space, uint16 elemCount = 1, bool allocate = false);
    void Set(const T& val, uint16 elemOffset = 0);
    T* Get(uint16 elemOffset = 0);
    void Reallocate();

    ConstantBuffer(const ConstantBuffer& other);
    ConstantBuffer(ConstantBuffer&& other);
    ~ConstantBuffer();

    ConstantBuffer& operator= (ConstantBuffer other);
    T* GetBufferData();
    uint32 GetDataSize() const;
    uint32 GetElemCount() const;
    void SetElemCount(uint32 count, bool reallocate = false);
    bool IsAllocated() const;

    ConstantBufferHandle GetHandle() const;
    void SetHandle(ConstantBufferHandle handle);
    void ScheduleToUpdate();

    uint16 GetIndex() const;
    uint16 GetSpace() const;
    uint32 GetKey() const;

    void MakeShallowCopy(ConstantBuffer& target, bool queueForUpdate = true) const; // [a_vorontcov] Copies params, space and key.

private:
    uint16 m_index = 0;
    uint16 m_space = 0;
    uint32 m_key = 0;
    bool m_isAllocated = false;

    T* m_memData = nullptr;
    uint32 m_dataSize = 0;
    uint32 m_elemCount = 0;

    ConstantBufferHandle m_handle;

    friend void swap(ConstantBuffer<T>& l, ConstantBuffer<T>& r)
    {
        std::swap(l.m_index, r.m_index);
        std::swap(l.m_space, r.m_space);
        std::swap(l.m_key, r.m_key);
        std::swap(l.m_isAllocated, r.m_isAllocated);
        std::swap(l.m_memData, r.m_memData);
        std::swap(l.m_dataSize, r.m_dataSize);
        std::swap(l.m_handle, r.m_handle);
    }
};

template <typename T>
inline uint32 ConstantBuffer<T>::GetElemCount() const
{
    return m_elemCount;
}

template <typename T>
inline void ConstantBuffer<T>::SetElemCount(uint32 count, bool reallocate)
{
    m_elemCount = count;
    SafeDeleteArray(m_memData);
    m_isAllocated = false;
    if (reallocate)
        Reallocate();
}

template <typename T>
inline ConstantBuffer<T>::ConstantBuffer(uint16 index, uint16 space, uint16 elemCount, bool allocate)
    : m_index(index), m_space(space), m_key(m_index | m_space << 16), m_dataSize(sizeof(T) * elemCount), m_elemCount(elemNum)
{
    if (allocate)
    {
        m_memData = new T[elemNum];
        m_isAllocated = true;
    }
}

template <typename T>
inline void ConstantBuffer<T>::Set(const T& val, uint16 elemOffset)
{
    *(m_memData + elemOffset) = val;
}

template <typename T>
inline T* ConstantBuffer<T>::Get(uint16 elemOffset)
{
    return m_memData + elemOffset;
}

template <typename T>
inline uint16 ConstantBuffer<T>::GetIndex() const
{
    return m_index;
}

template <typename T>
inline uint16 ConstantBuffer<T>::GetSpace() const
{
    return m_space;
}

template <typename T>
inline uint32 ConstantBuffer<T>::GetKey() const
{
    return m_key;
}

template <typename T>
inline T* ConstantBuffer<T>::GetBufferData()
{
    return m_memData;
}

template <typename T>
inline uint32 ConstantBuffer<T>::GetDataSize() const
{
    return m_dataSize;
}

template <typename T>
inline bool ConstantBuffer<T>::IsAllocated() const
{
    return m_isAllocated;
}

template <typename T>
inline ConstantBufferHandle ConstantBuffer<T>::GetHandle() const
{
    return m_handle;
}

template <typename T>
inline void ConstantBuffer<T>::SetHandle(ConstantBufferHandle handle)
{
    m_handle = handle;
}

template <typename T>
inline void ConstantBuffer<T>::Reallocate()
{
    SafeDeleteArray(m_memData);
    m_memData = new T[m_elemCount];
}

template <typename T>
ConstantBuffer<T>::~ConstantBuffer<T>()
{
    SafeDeleteArray(m_memData);
}
}