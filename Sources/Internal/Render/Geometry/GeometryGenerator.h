#pragma once

#include "Core/CoreTypes.h"

namespace Kioto::Renderer
{
class Mesh;

namespace GeometryGenerator
{
void Init();
void Shutdown();

///
/// Generate XZ aligned plane.
///
Mesh GeneratePlane(float32 sizeX = 1.0f, float32 sizeY = 1.0f);

///
/// Generate cube.
///
Mesh GenerateCube(float32 sizeX = 1.0f, float32 sizeY = 1.0f, float32 sizeZ = 1.0f);

///
/// Generate cone.
///
Mesh GenerateCone(float32 height = 1.0f, float32 bottomRadius = 0.25f, float32 topRadius = 0.05f);

///
/// Generate sphere.
///
Mesh GenerateSphere(float32 radius = 1.0f);

///
/// Generate tube.
///
Mesh GenerateTube(float32 height = 1.0f, float32 bottomRadius1 = 0.5f, float32 bottomRadius2 = 0.15f, float32 topRadius1 = 0.5f, float32 topRadius2 = 0.15f);

///
/// Generate icosphere.
///
Mesh GenerateIcosphere(int32 recursionLevel = 3, float32 radius = 1.0f);

Mesh* GetPlane();
Mesh* GetUnitCube();
Mesh* GetCone();
Mesh* GetUnitSphere();
Mesh* GetTube();
Mesh* GetUnitIcosphere();
}
}
