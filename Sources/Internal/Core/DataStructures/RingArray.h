#pragma once

#include <algorithm>
#include <array>
#include <functional>
#include <numeric>

#include "Core/CoreTypes.h"

#pragma warning(push)
#pragma warning(disable : 4521)
namespace Kioto
{
template <typename T, size_t ContainerSize>
class RingArray
{
public:
    template <typename... T>
    RingArray(T&&... args) : mBuffer({ std::forward<T>(args)... })
    {
        RecalculateSum();
    }

    RingArray() = default;

    RingArray(RingArray<T, ContainerSize>& rhs) : RingArray(const_cast<const RingArray<T, ContainerSize>&>(rhs))
    {}

    RingArray(const RingArray<T, ContainerSize>& rhs) : RingArray()
    {
        mBuffer = rhs.mBuffer;
        mSum = rhs.mSum;
    }

    RingArray(RingArray<T, ContainerSize>&& rhs) : RingArray()
    {
        using namespace std;
        swap(*this, rhs);
    }

    RingArray<T, ContainerSize>& operator=(RingArray<T, ContainerSize> rhs)
    {
        using namespace std;
        swap(*this, rhs);
        return *this;
    }

    void Reset()
    {
        mBuffer = {};
        mSum = {};
    }

    template <typename U, typename = std::enable_if_t<std::is_convertible_v<T, std::remove_reference_t<U>>> >
    void Reset(U&& value)
    {
        if (value == U{})
            mValsAdded = 0;
        mBuffer.fill(std::forward<const U>(value));
        RecalculateSum();
    }

    template <typename U, typename = std::enable_if_t<std::is_convertible_v<T, std::remove_reference_t<U>>> >
    void Add(U&& value)
    {
        mSum -= mBuffer[mI];
        mBuffer[mI] = value;
        mSum += value;
        mI = ++mI % mBuffer.size();
        ++mValsAdded;
    }

    T GetAverage()
    {
        if (mBuffer.empty() || mValsAdded == 0)
            return {};
        float32 valsToCount = static_cast<float32>(std::min<int32>(static_cast<uint32>(mBuffer.size()), mValsAdded));
        return static_cast<T>(mSum * (1.0f / valsToCount)); // [a_vorontcov] 1.0f / valsToCount for cases when for T type div operation with scalar is undefined, but mul is well defined.
    }

    T GetMax(std::function<bool(const T&, const T&)> comparer = std::less<T>())
    {
        return *std::max_element(std::begin(mBuffer), std::end(mBuffer), comparer);
    }

    T GetMin(std::function<bool(const T&, const T&)> comparer = std::less<T>())
    {
        return *std::min_element(std::begin(mBuffer), std::end(mBuffer), comparer);
    }

    friend void swap(RingArray<T, ContainerSize>& rhs, RingArray<T, ContainerSize>& lhs)
    {
        std::swap(rhs.mSum, lhs.mSum);
        std::swap(rhs.mI, lhs.mI);
        std::swap(rhs.mValsAdded, lhs.mValsAdded);
        std::swap(rhs.mBuffer, lhs.mBuffer);
    }

private:
    void RecalculateSum();

    std::array<T, ContainerSize> mBuffer = {};
    uint32 mI = 0;
    uint32 mValsAdded = 0;
    T mSum = {};
};

template <typename T, size_t ContainerSize>
void RingArray<T, ContainerSize>::RecalculateSum()
{
    mSum = {};
    mI = 0;
    for (auto& i : mBuffer)
    {
        mSum += i;
    }
}
}
#pragma warning(pop)