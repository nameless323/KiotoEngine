//
// Copyright (C) Alexandr Vorontsov. 2017
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#pragma once

namespace Kioto
{
template<typename T>
inline void SafeDelete(T*& ptr)
{
    if (ptr != nullptr)
    {
        delete ptr;
        ptr = nullptr;
    }
    // [a_vorontsov] Anton Smetanin suggest to add some logs here in future.
}

template<typename T>
inline void SafeDeleteArray(T*& ptr)
{
    if (ptr != nullptr)
    {
        delete[] ptr;
        ptr = nullptr;
    }
    // [a_vorontsov] Anton Smetanin suggest to add some logs here in future
}
}