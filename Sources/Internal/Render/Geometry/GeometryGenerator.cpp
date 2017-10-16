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
    Mesh m;
    m.Position.reserve(24); // [a_vorontsov] TODO:: Via pointer to data.
    m.UV0.reserve(24);
    m.Normal.reserve(24);
    m.Triangles.reserve(36);
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

    m.Position =
    {
        // Bottom
        p0, p1, p2, p3,

        // Left
        p7, p4, p0, p3,

        // Front
        p4, p5, p1, p0,

        // Back
        p6, p7, p3, p2,

        // Right
        p5, p6, p2, p1,

        // Top
        p7, p6, p5, p4
    };

    Vector3 up(0.0f, 1.0f, 0.0f);
    Vector3 down(0.0f, -1.0f, 0.0f);
    Vector3 front(0.0f, 0.0f, 1.0f);
    Vector3 back(0.0f, 0.0f, -1.0f);
    Vector3 left(1.0f, 0.0f, 0.0f);
    Vector3 right(-1.0f, 0.0f, 0.0f);

    m.Normal =
    {
        // Bottom
        down, down, down, down,

        // Left
        left, left, left, left,

        // Front
        front, front, front, front,

        // Back
        back, back, back, back,

        // Right
        right, right, right, right,

        // Top
        up, up, up, up
    };

    Vector2 _00(0.0f, 0.0f);
    Vector2 _10(1.0f, 0.0f);
    Vector2 _01(0.0f, 1.0f);
    Vector2 _11(1.0f, 1.0f);

    m.UV0 =
    {
        // Bottom
        _11, _01, _00, _10,

        // Left
        _11, _01, _00, _10,

        // Front
        _11, _01, _00, _10,

        // Back
        _11, _01, _00, _10,

        // Right
        _11, _01, _00, _10,

        // Top
        _11, _01, _00, _10,
    };

        m.Triangles =
    {
        // Bottom
        3, 1, 0,
            3, 2, 1,

            // Left
            3 + 4 * 1, 1 + 4 * 1, 0 + 4 * 1,
            3 + 4 * 1, 2 + 4 * 1, 1 + 4 * 1,

            // Front
            3 + 4 * 2, 1 + 4 * 2, 0 + 4 * 2,
            3 + 4 * 2, 2 + 4 * 2, 1 + 4 * 2,

            // Back
            3 + 4 * 3, 1 + 4 * 3, 0 + 4 * 3,
            3 + 4 * 3, 2 + 4 * 3, 1 + 4 * 3,

            // Right
            3 + 4 * 4, 1 + 4 * 4, 0 + 4 * 4,
            3 + 4 * 4, 2 + 4 * 4, 1 + 4 * 4,

            // Top
            3 + 4 * 5, 1 + 4 * 5, 0 + 4 * 5,
            3 + 4 * 5, 2 + 4 * 5, 1 + 4 * 5,

    };

    m.PrepareForUpload();

    return m;
}
}
