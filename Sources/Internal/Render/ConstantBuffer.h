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

#include "Render/Buffers/EngineBuffers.h"

#include "Render/RendererPublic.h"
#include "Render/Renderer.h"

namespace Kioto::Renderer
{
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
    uint32 GetElemSize() const;
    uint32 GetElemCount() const;
    const std::string& GetName() const;
    template <typename T>
    void SetElemCount(uint32 count, bool reallocate = false);
    bool IsAllocated() const;

    bool IsPerObjectBuffer() const;

    ConstantBufferHandle GetHandle() const;
    void SetHandle(ConstantBufferHandle handle);
    void ScheduleToUpdate();

    uint16 GetIndex() const;
    uint16 GetSpace() const;
    uint32 GetKey() const;

    void MakeShallowCopy(ConstantBuffer& target, bool queueForUpdate = true) const; // [a_vorontcov] Space and key. Doesn't copy memory itself.

private:
    uint16 mIndex = 0;
    uint16 mSpace = 0;
    uint32 mKey = 0;
    bool mIsAllocated = false;

    byte* mMemData = nullptr;
    uint32 mDataSize = 0;
    uint32 mElemCount = 0;
    uint32 mElemSize = 0;
    std::string mName;

    ConstantBufferHandle mHandle;

    friend void swap(ConstantBuffer& l, ConstantBuffer& r)
    {
        std::swap(l.mIndex, r.mIndex);
        std::swap(l.mSpace, r.mSpace);
        std::swap(l.mKey, r.mKey);
        std::swap(l.mIsAllocated, r.mIsAllocated);
        std::swap(l.mMemData, r.mMemData);
        std::swap(l.mDataSize, r.mDataSize);
        std::swap(l.mHandle, r.mHandle);
        std::swap(l.mElemCount, r.mElemCount);
        std::swap(l.mElemSize, r.mElemSize);
        std::swap(l.mName, r.mName);
    }
};

inline uint32 ConstantBuffer::GetElemCount() const
{
    return mElemCount;
}

inline uint32 ConstantBuffer::GetElemSize() const
{
    return mElemSize;
}

inline const std::string& ConstantBuffer::GetName() const
{
    return mName;
}

template <typename T>
inline void ConstantBuffer::SetElemCount(uint32 count, bool reallocate)
{
    mElemCount = count;
    SafeDeleteArray(mMemData);
    mIsAllocated = false;
    mElemSize = sizeof(T);
    mDataSize = mElemCount * mElemSize;
    if (reallocate)
        Reallocate();
}

inline ConstantBuffer::ConstantBuffer(std::string name, uint16 index, uint16 space, uint16 elemSize, uint16 elemCount, bool allocate)
    : mName(std::move(name))
    , mIndex(index)
    , mSpace(space)
    , mKey(mIndex | mSpace << 16)
    , mDataSize(elemSize * elemCount)
    , mElemCount(elemCount)
    , mElemSize(elemSize)
{
    if (allocate)
    {
        mMemData = new byte[mDataSize];
        mIsAllocated = true;
    }
}

inline ConstantBuffer::ConstantBuffer(std::string name, uint16 index, uint16 space)
    : mName(std::move(name))
    , mIndex(index)
    , mSpace(space)
    , mKey(mIndex | mSpace << 16)
{
}

template <typename T>
inline void ConstantBuffer::Set(const T& val, uint16 elemOffset, bool updateHWinstance)
{
    T* mem = reinterpret_cast<T*>(mMemData);
    *(mem + elemOffset) = val;

    if (updateHWinstance)
        ScheduleToUpdate();
}

template <typename T>
inline T* ConstantBuffer::Get(uint16 elemOffset)
{
    T* mem = reinterpret_cast<T*>(mMemData);
    return mem + elemOffset;
}

inline uint16 ConstantBuffer::GetIndex() const
{
    return mIndex;
}

inline uint16 ConstantBuffer::GetSpace() const
{
    return mSpace;
}

inline uint32 ConstantBuffer::GetKey() const
{
    return mKey;
}

template <typename T>
inline T* ConstantBuffer::GetBufferData()
{
    return reinterpret_cast<T*>(mMemData);
}

inline byte* ConstantBuffer::GetBufferData()
{
    return mMemData;
}

inline uint32 ConstantBuffer::GetDataSize() const
{
    return mDataSize;
}

inline bool ConstantBuffer::IsAllocated() const
{
    return mIsAllocated;
}

inline ConstantBufferHandle ConstantBuffer::GetHandle() const
{
    return mHandle;
}

inline void ConstantBuffer::SetHandle(ConstantBufferHandle handle)
{
    mHandle = handle;
}

inline void ConstantBuffer::Reallocate()
{
    SafeDeleteArray(mMemData);
    mMemData = new byte[mDataSize];
    mIsAllocated = true;
}

inline ConstantBuffer::~ConstantBuffer()
{
    SafeDeleteArray(mMemData);
}

inline ConstantBuffer::ConstantBuffer(const ConstantBuffer& other)
    : mName(other.mName)
    , mIndex(other.mIndex)
    , mSpace(other.mSpace)
    , mKey(other.mKey)
    , mIsAllocated(other.mIsAllocated)
    , mDataSize(other.mDataSize)
    , mElemSize(other.mElemSize)
    , mElemCount(other.mElemCount)
{
    if (other.IsAllocated())
        memcpy(mMemData, other.mMemData, other.mDataSize);
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
    target.mIndex = mIndex;
    target.mSpace = mSpace;
    target.mKey = mKey;
    target.mElemCount = mElemCount;
    target.mElemSize = mElemSize;
    target.mDataSize = mDataSize;
    target.mName = mName;
    target.mIsAllocated = false;
}

inline void ConstantBuffer::ScheduleToUpdate()
{
    Renderer::QueueConstantBufferForUpdate(*this);
}

inline bool ConstantBuffer::IsPerObjectBuffer() const
{
    return mSpace != Renderer::EngineBuffers::EngineBuffersSpace;
}

}