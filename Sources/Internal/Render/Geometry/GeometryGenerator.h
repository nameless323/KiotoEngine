//
// Copyright (C) Alexandr Vorontsov. 2017
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#pragma once

#include "Core/CoreTypes.h"

namespace Kioto
{
class Mesh;

namespace GeometryGenerator
{
///
/// Generate XY aligned plane.
///
Mesh GeneratePlane(float32 sizeX = 1.0f, float32 sizeY = 1.0f);
///
/// Generate cube.
///
Mesh GenerateCube(float32 sizeX = 1.0f, float32 sizeY = 1.0f, float32 sizeZ = 1.0f);
}
}
