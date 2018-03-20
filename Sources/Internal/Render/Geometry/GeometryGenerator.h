//
// Copyright (C) Alexandr Vorontsov. 2017
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#pragma once

#include "Core/CoreTypes.h"

namespace Kioto
{
class Mesh;
class Mesh2;

namespace GeometryGenerator
{
void Init();
void Shutdown();

///
/// Generate XZ aligned plane.
///
Mesh2 GeneratePlane(float32 sizeX = 1.0f, float32 sizeY = 1.0f);
///
/// Generate cube.
///
Mesh2 GenerateCube(float32 sizeX = 1.0f, float32 sizeY = 1.0f, float32 sizeZ = 1.0f);
///
/// Generate cone.
///
Mesh2 GenerateCone(float32 height = 1.0f, float32 bottomRadius = 0.25f, float32 topRadius = 0.05f);
///
/// Generate sphere.
///
Mesh2 GenerateSphere(float32 radius = 1.0f);
///
/// Generate tube.
///
Mesh2 GenerateTube(float32 height = 1.0f, float32 bottomRadius1 = 0.5f, float32 bottomRadius2 = 0.15f, float32 topRadius1 = 0.5f, float32 topRadius2 = 0.15f);
///
/// Generate icosphere.
///
Mesh2 GenerateIcosphere(int32 recursionLevel = 3, float32 radius = 1.0f);

Mesh2* GetPlane();
Mesh2* GetUnitCube();
Mesh2* GetCone();
Mesh2* GetUnitSphere();
Mesh2* GetTube();
Mesh2* GetUnitIcosphere();
}
}
