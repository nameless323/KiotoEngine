//
// Copyright (C) Alexandr Vorontsov 2016.
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#pragma once

#include <array>
#include <numeric>

#include "Core/CoreTypes.h"

namespace Kioto
{
template <typename T, size_t ContainerSize>
class RingArray
{
public:
    template <typename... T>
    RingArray(T&&... args) : m_buffer({ std::forward<T>(args)... })
    {
        RecalculateSum();
    }

    RingArray() = default;

    RingArray(RingArray<T, ContainerSize>& rhs) : RingArray(const_cast<const RingArray<T, ContainerSize>&>(rhs))
    {}

    RingArray(const RingArray<T, ContainerSize>& rhs) : RingArray()
    {
        m_buffer = rhs.m_buffer;
        m_sum = rhs.m_sum;
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
        m_buffer = {};
        m_sum = {};
    }

    template <typename U, typename = std::enable_if_t<std::is_convertible_v<T, std::remove_reference_t<U>>> >
    void Reset(U&& value)
    {
        m_buffer.fill(std::forward<const U>(value));
        RecalculateSum();
    }

    template <typename U, typename = std::enable_if_t<std::is_convertible_v<T, std::remove_reference_t<U>>> >
    void Add(U&& value)
    {
        m_sum -= m_buffer[m_i];
        m_buffer[m_i] = value;
        m_sum += value;
        m_i = ++m_i % m_buffer.size();
    }

    T GetAverage()
    {
        return static_cast<T>(m_sum * (1.0f / static_cast<float>(m_buffer.size())));
    }

    T GetMax(std::function<bool(const T&, const T&)> comparer = std::less<T>())
    {
        return *std::max_element(std::begin(m_buffer), std::end(m_buffer), comparer);
    }

    T GetMin(std::function<bool(const T&, const T&)> comparer = std::less<T>())
    {
        return *std::min_element(std::begin(m_buffer), std::end(m_buffer), comparer);
    }

    friend void swap(RingArray<T, ContainerSize>& rhs, RingArray<T, ContainerSize>& lhs)
    {
        std::swap(rhs.m_sum, lhs.m_sum);
        std::swap(rhs.m_i, lhs.m_i);
        std::swap(rhs.m_buffer, lhs.m_buffer);
    }

private:
    inline void RecalculateSum();

    std::array<T, ContainerSize> m_buffer = {};
    int32 m_i = 0;
    T m_sum = {};
};

template <typename T, size_t ContainerSize>
void RingArray<T, ContainerSize>::RecalculateSum()
{
    m_sum = {};
    m_i = 0;
    for (auto& i : m_buffer)
    {
        m_sum += i;
    }
}
}