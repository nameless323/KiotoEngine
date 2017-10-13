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
    Mesh m;
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

    return m;
}

Mesh GenerateCube(float32 sizeX /*= 1.0f*/, float32 sizeY /*= 1.0f*/, float32 sizeZ /*= 1.0f*/)
{
    Mesh m;
    m.Position.reserve(24);
    m.UV0.reserve(24);
    m.Normal.reserve(24);
    m.Triangles.reserve(36);
    float32 xHalf = sizeX * 0.5f;
    float32 yHalf = sizeY * 0.5f;
    float32 zHalf = sizeZ * 0.5f;

    // [a_vorontsov] Front.
    m.Position.emplace_back(-xHalf, -yHalf, -zHalf);
    m.Position.emplace_back(-xHalf, yHalf, -zHalf);
    m.Position.emplace_back(xHalf, yHalf, -zHalf);
    m.Position.emplace_back(xHalf, -yHalf, -zHalf);

    m.UV0.emplace_back(0.0f, 0.0f);
    m.UV0.emplace_back(0.0f, 1.0f);
    m.UV0.emplace_back(1.0f, 1.0f);
    m.UV0.emplace_back(1.0f, 0.0f);

    m.Normal.emplace_back(0.0f, 0.0f, -1.0f);
    m.Normal.emplace_back(0.0f, 0.0f, -1.0f);
    m.Normal.emplace_back(0.0f, 0.0f, -1.0f);
    m.Normal.emplace_back(0.0f, 0.0f, -1.0f);

    // [a_vorontsov] Back.
    m.Position.emplace_back(-xHalf, -yHalf, zHalf);
    m.Position.emplace_back(-xHalf, yHalf, zHalf);
    m.Position.emplace_back(xHalf, yHalf, zHalf);
    m.Position.emplace_back(xHalf, -yHalf, zHalf);

    m.UV0.emplace_back(1.0f, 1.0f);
    m.UV0.emplace_back(1.0f, 0.0f);
    m.UV0.emplace_back(0.0f, 0.0f);
    m.UV0.emplace_back(0.0f, 1.0f);

    m.Normal.emplace_back(0.0f, 0.0f, 1.0f);
    m.Normal.emplace_back(0.0f, 0.0f, 1.0f);
    m.Normal.emplace_back(0.0f, 0.0f, 1.0f);
    m.Normal.emplace_back(0.0f, 0.0f, 1.0f);

    // [a_vorontsov] Left.
    m.Position.emplace_back(-xHalf, -yHalf, zHalf);
    m.Position.emplace_back(-xHalf, yHalf, zHalf);
    m.Position.emplace_back(-xHalf, yHalf, -zHalf);
    m.Position.emplace_back(-xHalf, -yHalf, -zHalf);

    m.UV0.emplace_back(0.0f, 0.0f);
    m.UV0.emplace_back(0.0f, 1.0f);
    m.UV0.emplace_back(1.0f, 1.0f);
    m.UV0.emplace_back(1.0f, 0.0f);

    m.Normal.emplace_back(-1.0f, 0.0f, 0.0f);
    m.Normal.emplace_back(-1.0f, 0.0f, 0.0f);
    m.Normal.emplace_back(-1.0f, 0.0f, 0.0f);
    m.Normal.emplace_back(-1.0f, 0.0f, 0.0f);

    // [a_vorontsov] Right.
    m.Position.emplace_back(xHalf, -yHalf, zHalf);
    m.Position.emplace_back(xHalf, yHalf, zHalf);
    m.Position.emplace_back(xHalf, yHalf, -zHalf);
    m.Position.emplace_back(xHalf, -yHalf, -zHalf);

    m.UV0.emplace_back(1.0f, 1.0f);
    m.UV0.emplace_back(1.0f, 0.0f);
    m.UV0.emplace_back(0.0f, 0.0f);
    m.UV0.emplace_back(0.0f, 1.0f);

    m.Normal.emplace_back(1.0f, 0.0f, 0.0f);
    m.Normal.emplace_back(1.0f, 0.0f, 0.0f);
    m.Normal.emplace_back(1.0f, 0.0f, 0.0f);
    m.Normal.emplace_back(1.0f, 0.0f, 0.0f);

    // [a_vorontsov] Top.
    m.Position.emplace_back(-xHalf, yHalf, -zHalf);
    m.Position.emplace_back(-xHalf, yHalf, zHalf);
    m.Position.emplace_back(xHalf, yHalf, zHalf);
    m.Position.emplace_back(xHalf, yHalf, -zHalf);

    m.UV0.emplace_back(0.0f, 0.0f);
    m.UV0.emplace_back(0.0f, 1.0f);
    m.UV0.emplace_back(1.0f, 1.0f);
    m.UV0.emplace_back(1.0f, 0.0f);

    m.Normal.emplace_back(0.0f, 1.0f, 0.0f);
    m.Normal.emplace_back(0.0f, 1.0f, 0.0f);
    m.Normal.emplace_back(0.0f, 1.0f, 0.0f);
    m.Normal.emplace_back(0.0f, 1.0f, 0.0f);

    // [a_vorontsov] Bottom.
    m.Position.emplace_back(-xHalf, -yHalf, -zHalf);
    m.Position.emplace_back(-xHalf, -yHalf, zHalf);
    m.Position.emplace_back(xHalf, -yHalf, zHalf);
    m.Position.emplace_back(xHalf, -yHalf, -zHalf);

    m.UV0.emplace_back(1.0f, 1.0f);
    m.UV0.emplace_back(1.0f, 0.0f);
    m.UV0.emplace_back(0.0f, 0.0f);
    m.UV0.emplace_back(0.0f, 1.0f);

    m.Normal.emplace_back(0.0f, 1.0f, 0.0f);
    m.Normal.emplace_back(0.0f, 1.0f, 0.0f);
    m.Normal.emplace_back(0.0f, 1.0f, 0.0f);
    m.Normal.emplace_back(0.0f, 1.0f, 0.0f);
    
    m.Triangles =
    {
        0, 1, 3,
        3, 1, 2,

        4, 5, 7,
        7, 5, 6,

        8, 9, 11,
        11, 9, 10,

        12, 13, 15,
        15, 13, 14,

        16, 17, 19,
        19, 17, 18,

        20, 21, 23,
        23, 21, 22
    };

    return m;
}
}
