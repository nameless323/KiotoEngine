//
// Copyright (C) Alexandr Vorontsov. 2017
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#include "stdafx.h"

#include "Render/Geometry/GeometryGenerator.h"

#include <vector>

#include "Render/Geometry/Mesh.h"
#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"

namespace Kioto::GeometryGenerator
{
Mesh GeometryGenerator::GeneratePlane(float32 sizeX /*= 1.0f*/, float32 sizeY /*= 1.0f*/)
{
    Mesh m; // [a_vorontsov] TODO:: Via pointer to data.
    m.Position.reserve(4);
    m.UV0.reserve(4);
    m.Normal.reserve(4);
    m.Triangles.reserve(6);
    float32 xHalf = sizeX * 0.5f;
    float32 yHalf = sizeY * 0.5f;

    m.Position.emplace_back(-xHalf, -yHalf, 0.0f);
    m.Position.emplace_back(-xHalf, yHalf, 0.0f);
    m.Position.emplace_back(xHalf, yHalf, 0.0f);
    m.Position.emplace_back(xHalf, -yHalf, 0.0f);

    m.UV0.emplace_back(0.0f, 0.0f);
    m.UV0.emplace_back(0.0f, 1.0f);
    m.UV0.emplace_back(1.0f, 1.0f);
    m.UV0.emplace_back(1.0f, 0.0f);

    m.Normal.emplace_back(0.0f, 0.0f, -1.0f);
    m.Normal.emplace_back(0.0f, 0.0f, -1.0f);
    m.Normal.emplace_back(0.0f, 0.0f, -1.0f);
    m.Normal.emplace_back(0.0f, 0.0f, -1.0f);

    m.Triangles.push_back(0);
    m.Triangles.push_back(1);
    m.Triangles.push_back(4);

    m.Triangles.push_back(4);
    m.Triangles.push_back(1);
    m.Triangles.push_back(3);

    m.PrepareForUpload();

    return m;
}

Mesh GenerateCube(float32 sizeX /*= 1.0f*/, float32 sizeY /*= 1.0f*/, float32 sizeZ /*= 1.0f*/)
{
    float32 xHalf = sizeX * 0.5f;
    float32 yHalf = sizeY * 0.5f;
    float32 zHalf = sizeZ * 0.5f;

    Vector3 p0(-zHalf, -xHalf, yHalf);
    Vector3 p1(zHalf, -xHalf, yHalf);
    Vector3 p2(zHalf, -xHalf, -yHalf);
    Vector3 p3(-zHalf, -xHalf, -yHalf);

    Vector3 p4(-zHalf, xHalf, yHalf);
    Vector3 p5(zHalf, xHalf, yHalf);
    Vector3 p6(zHalf, xHalf, -yHalf);
    Vector3 p7(-zHalf, xHalf, -yHalf);

    uint32 stride = sizeof(Vector3) + sizeof(Vector2) + sizeof(Vector3);
    byte* vertexData = new byte[stride * 24];
    byte* begin = vertexData;
    // [a_vorontsov] Bottom.
    *reinterpret_cast<Vector3*>(vertexData) = p0; vertexData += stride;
    *reinterpret_cast<Vector3*>(vertexData) = p1; vertexData += stride;
    *reinterpret_cast<Vector3*>(vertexData) = p2; vertexData += stride;
    *reinterpret_cast<Vector3*>(vertexData) = p3; vertexData += stride;
    // [a_vorontsov] Left.
    *reinterpret_cast<Vector3*>(vertexData) = p7; vertexData += stride;
    *reinterpret_cast<Vector3*>(vertexData) = p4; vertexData += stride;
    *reinterpret_cast<Vector3*>(vertexData) = p0; vertexData += stride;
    *reinterpret_cast<Vector3*>(vertexData) = p3; vertexData += stride;
    // [a_vorontsov] Front.
    *reinterpret_cast<Vector3*>(vertexData) = p4; vertexData += stride;
    *reinterpret_cast<Vector3*>(vertexData) = p5; vertexData += stride;
    *reinterpret_cast<Vector3*>(vertexData) = p1; vertexData += stride;
    *reinterpret_cast<Vector3*>(vertexData) = p0; vertexData += stride;
    // [a_vorontsov] Back.
    *reinterpret_cast<Vector3*>(vertexData) = p6; vertexData += stride;
    *reinterpret_cast<Vector3*>(vertexData) = p7; vertexData += stride;
    *reinterpret_cast<Vector3*>(vertexData) = p3; vertexData += stride;
    *reinterpret_cast<Vector3*>(vertexData) = p2; vertexData += stride;
    // [a_vorontsov] Right.
    *reinterpret_cast<Vector3*>(vertexData) = p5; vertexData += stride;
    *reinterpret_cast<Vector3*>(vertexData) = p6; vertexData += stride;
    *reinterpret_cast<Vector3*>(vertexData) = p2; vertexData += stride;
    *reinterpret_cast<Vector3*>(vertexData) = p1; vertexData += stride;
    // [a_vorontsov] Top.
    *reinterpret_cast<Vector3*>(vertexData) = p7; vertexData += stride;
    *reinterpret_cast<Vector3*>(vertexData) = p6; vertexData += stride;
    *reinterpret_cast<Vector3*>(vertexData) = p5; vertexData += stride;
    *reinterpret_cast<Vector3*>(vertexData) = p4; vertexData = begin;
    vertexData += sizeof(Vector3);

    Vector3 up(0.0f, 1.0f, 0.0f);
    Vector3 down(0.0f, -1.0f, 0.0f);
    Vector3 front(0.0f, 0.0f, 1.0f);
    Vector3 back(0.0f, 0.0f, -1.0f);
    Vector3 left(1.0f, 0.0f, 0.0f);
    Vector3 right(-1.0f, 0.0f, 0.0f);

    // [a_vorontsov] Normals. Bottom.
    *reinterpret_cast<Vector3*>(vertexData) = down; vertexData += stride;
    *reinterpret_cast<Vector3*>(vertexData) = down; vertexData += stride;
    *reinterpret_cast<Vector3*>(vertexData) = down; vertexData += stride;
    *reinterpret_cast<Vector3*>(vertexData) = down; vertexData += stride;
    // [a_vorontsov] Left.
    *reinterpret_cast<Vector3*>(vertexData) = left; vertexData += stride;
    *reinterpret_cast<Vector3*>(vertexData) = left; vertexData += stride;
    *reinterpret_cast<Vector3*>(vertexData) = left; vertexData += stride;
    *reinterpret_cast<Vector3*>(vertexData) = left; vertexData += stride;
    // [a_vorontsov] Front.
    *reinterpret_cast<Vector3*>(vertexData) = front; vertexData += stride;
    *reinterpret_cast<Vector3*>(vertexData) = front; vertexData += stride;
    *reinterpret_cast<Vector3*>(vertexData) = front; vertexData += stride;
    *reinterpret_cast<Vector3*>(vertexData) = front; vertexData += stride;
    // [a_vorontsov] Back.
    *reinterpret_cast<Vector3*>(vertexData) = back; vertexData += stride;
    *reinterpret_cast<Vector3*>(vertexData) = back; vertexData += stride;
    *reinterpret_cast<Vector3*>(vertexData) = back; vertexData += stride;
    *reinterpret_cast<Vector3*>(vertexData) = back; vertexData += stride;
    // [a_vorontsov] Right.
    *reinterpret_cast<Vector3*>(vertexData) = right; vertexData += stride;
    *reinterpret_cast<Vector3*>(vertexData) = right; vertexData += stride;
    *reinterpret_cast<Vector3*>(vertexData) = right; vertexData += stride;
    *reinterpret_cast<Vector3*>(vertexData) = right; vertexData += stride;
    // [a_vorontsov] Top.
    *reinterpret_cast<Vector3*>(vertexData) = up; vertexData += stride;
    *reinterpret_cast<Vector3*>(vertexData) = up; vertexData += stride;
    *reinterpret_cast<Vector3*>(vertexData) = up; vertexData += stride;
    *reinterpret_cast<Vector3*>(vertexData) = up; vertexData = begin;
    vertexData += sizeof(Vector3) + sizeof(Vector3);

    Vector2 _00(0.0f, 0.0f);
    Vector2 _10(1.0f, 0.0f);
    Vector2 _01(0.0f, 1.0f);
    Vector2 _11(1.0f, 1.0f);

    // [a_vorontsov] UV. Bottom.
    *reinterpret_cast<Vector2*>(vertexData) = _11; vertexData += stride;
    *reinterpret_cast<Vector2*>(vertexData) = _01; vertexData += stride;
    *reinterpret_cast<Vector2*>(vertexData) = _00; vertexData += stride;
    *reinterpret_cast<Vector2*>(vertexData) = _10; vertexData += stride;
    // [a_vorontsov] Left.
    *reinterpret_cast<Vector2*>(vertexData) = _11; vertexData += stride;
    *reinterpret_cast<Vector2*>(vertexData) = _01; vertexData += stride;
    *reinterpret_cast<Vector2*>(vertexData) = _00; vertexData += stride;
    *reinterpret_cast<Vector2*>(vertexData) = _10; vertexData += stride;
    // [a_vorontsov] Front.
    *reinterpret_cast<Vector2*>(vertexData) = _11; vertexData += stride;
    *reinterpret_cast<Vector2*>(vertexData) = _01; vertexData += stride;
    *reinterpret_cast<Vector2*>(vertexData) = _00; vertexData += stride;
    *reinterpret_cast<Vector2*>(vertexData) = _10; vertexData += stride;
    // [a_vorontsov] Back.
    *reinterpret_cast<Vector2*>(vertexData) = _11; vertexData += stride;
    *reinterpret_cast<Vector2*>(vertexData) = _01; vertexData += stride;
    *reinterpret_cast<Vector2*>(vertexData) = _00; vertexData += stride;
    *reinterpret_cast<Vector2*>(vertexData) = _10; vertexData += stride;
    // [a_vorontsov] Right.
    *reinterpret_cast<Vector2*>(vertexData) = _11; vertexData += stride;
    *reinterpret_cast<Vector2*>(vertexData) = _01; vertexData += stride;
    *reinterpret_cast<Vector2*>(vertexData) = _00; vertexData += stride;
    *reinterpret_cast<Vector2*>(vertexData) = _10; vertexData += stride;
    // [a_vorontsov] Top.
    *reinterpret_cast<Vector2*>(vertexData) = _11; vertexData += stride;
    *reinterpret_cast<Vector2*>(vertexData) = _01; vertexData += stride;
    *reinterpret_cast<Vector2*>(vertexData) = _00; vertexData += stride;
    *reinterpret_cast<Vector2*>(vertexData) = _10; vertexData = begin;

    uint32* indices = new uint32[36];
    uint32* iBegin = indices;
    // [a_vorontsov] Bottom.
    *indices++ = 3;
    *indices++ = 1;
    *indices++ = 0;

    *indices++ = 3;
    *indices++ = 2;
    *indices++ = 1;

    // [a_vorontsov] Left.
    *indices++ = 7;
    *indices++ = 5;
    *indices++ = 4;

    *indices++ = 7;
    *indices++ = 6;
    *indices++ = 5;

    // [a_vorontsov] Front.
    *indices++ = 11;
    *indices++ = 9;
    *indices++ = 8;

    *indices++ = 11;
    *indices++ = 10;
    *indices++ = 9;

    // [a_vorontsov] Back.
    *indices++ = 15;
    *indices++ = 13;
    *indices++ = 12;

    *indices++ = 15;
    *indices++ = 14;
    *indices++ = 13;

    // [a_vorontsov] Right.
    *indices++ = 19;
    *indices++ = 17;
    *indices++ = 16;

    *indices++ = 19;
    *indices++ = 18;
    *indices++ = 17;

    // [a_vorontsov] Top.
    *indices++ = 23;
    *indices++ = 21;
    *indices++ = 20;

    *indices++ = 23;
    *indices++ = 22;
    *indices++ = 21;

    indices = iBegin;

    return { vertexData, stride * 24, stride, 24, reinterpret_cast<byte*>(indices), 36 * sizeof(uint32), 36, eIndexFormat::Format32Bit };
}
}
