//
// Copyright (C) Alexandr Vorontsov. 2017
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#include "Core/CoreTypes.h"

#pragma once

namespace Kioto::Renderer
{
using Handle = uint32; // [a_vorontsov] Maybe some template class to some type safe.
const Handle InvalidHandle = static_cast<Handle>(-1);
}