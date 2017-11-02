//
// Copyright (C) Alexandr Vorontsov. 2017
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#pragma once

#include "Core/CoreTypes.h"

namespace Kioto
{
class SceneSystem
{
public:
    KIOTO_API virtual void Update(float32 dt) abstract;
    KIOTO_API virtual ~SceneSystem() = default;
};
}
