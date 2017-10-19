//
// Copyright (C) Alexandr Vorontsov. 2017
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//
// Adapted from http://wiki.unity3d.com/index.php/ProceduralPrimitives

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
    float length = 1.0f;
    float width = 1.0f;
    int resX = 2; // 2 minimum
    int resZ = 2;
    std::string s;
    s.length();

    m.Position.reserve(resX * resZ);
    for (int z = 0; z < resZ; z++)
    {
        // [ -length / 2, length / 2 ]
        float zPos = ((float)z / (resZ - 1) - .5f) * length;
        for (int x = 0; x < resX; x++)
        {
            // [ -width / 2, width / 2 ]
            float xPos = ((float)x / (resX - 1) - .5f) * width;
            m.Position.emplace_back(xPos, 0.0f, zPos);
        }
    }

    m.Normal.reserve(m.Position.size());
    for (int n = 0; n < m.Position.size(); n++)
        m.Normal.emplace_back(0.0f, 1.0f, 0.0f);

    m.UV0.reserve(m.Position.size());
    for (int v = 0; v < resZ; v++)
    {
        for (int u = 0; u < resX; u++)
        {
            m.UV0.emplace_back((float)u / (resX - 1), (float)v / (resZ - 1));
        }
    }

    int nbFaces = (resX - 1) * (resZ - 1);
    m.Triangles.reserve(nbFaces * 6);
    int t = 0;
    for (int face = 0; face < nbFaces; face++)
    {
        // Retrieve lower left corner from face ind
        int i = face % (resX - 1) + (face / (resZ - 1) * resX);

        m.Triangles.push_back(i + resX);
        m.Triangles.push_back(i + 1);
        m.Triangles.push_back(i);

        m.Triangles.push_back(i + resX);
        m.Triangles.push_back(i + resX + 1);
        m.Triangles.push_back(i + 1);
    }

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

Mesh GenerateCone()
{
    Mesh m;
    float height = 1.0f;
    float bottomRadius = .25f;
    float topRadius = .05f;
    int nbSides = 18;
    int nbHeightSeg = 1; // Not implemented yet

    int nbVerticesCap = nbSides + 1;

    m.Position.reserve(nbVerticesCap + nbVerticesCap + nbSides * nbHeightSeg * 2 + 2);
    int vertNumber = nbVerticesCap + nbVerticesCap + nbSides * nbHeightSeg * 2 + 2;
    int vert = 0;

    // Bottom cap
    m.Position.emplace_back(0.0f, 0.0f, 0.0f);
    vert++;
    while (vert <= nbSides)
    {
        float rad = (float)vert / nbSides * Math::TwoPI;
        m.Position.emplace_back(std::cos(rad) * bottomRadius, 0.0f, std::sin(rad) * bottomRadius);
        vert++;
    }

    // Top cap
    m.Position.emplace_back(0.0f, height, 0.0f);
    vert++;
    while (vert <= nbSides * 2 + 1)
    {
        float rad = (float)(vert - nbSides - 1) / nbSides * Math::TwoPI;
        m.Position.emplace_back(std::cos(rad) * topRadius, height, std::sin(rad) * topRadius);
        vert++;
    }

    // Sides
    int v = 0;
    while (vert <= vertNumber - 4)
    {
        float rad = (float)v / nbSides * Math::TwoPI;
        m.Position.emplace_back(std::cos(rad) * topRadius, height, std::sin(rad) * topRadius);
        m.Position.emplace_back(std::cos(rad) * bottomRadius, 0.0f, std::sin(rad) * bottomRadius);
        vert += 2;
        v++;
    }
    m.Position.push_back(m.Position[nbSides * 2 + 2]);
    m.Position.push_back(m.Position[nbSides * 2 + 3]);

    m.Normal.reserve(m.Position.size());
    vert = 0;

    // Bottom cap
    while (vert <= nbSides)
    {
        m.Normal.emplace_back(0.0f, -1.0f, 0.0f);
        vert++;
    }

    // Top cap
    while (vert <= nbSides * 2 + 1)
    {
        m.Normal.emplace_back(0.0f, 1.0f, 0.0f);
        vert++;
    }

    // Sides
    v = 0;
    while (vert <= vertNumber - 4)
    {
        float rad = (float)v / nbSides * Math::TwoPI;
        float cos = std::cos(rad);
        float sin = std::sin(rad);

        m.Normal.emplace_back(cos, 0.0f, sin);
        m.Normal.emplace_back(cos, 0.0f, sin);

        vert += 2;
        v++;
    }
    m.Normal.push_back(m.Normal[nbSides * 2 + 2]);
    m.Normal.push_back(m.Normal[nbSides * 2 + 3]);

    // Bottom cap
    int u = 0;
    m.UV0.emplace_back(0.5f, 0.5f);
    u++;
    while (u <= nbSides)
    {
        float rad = (float)u / nbSides * Math::TwoPI;
        m.UV0.emplace_back(std::cos(rad) * .5f + .5f, std::sin(rad) * .5f + .5f);
        u++;
    }

    // Top cap
    m.UV0.emplace_back(0.5f, 0.5f);
    u++;
    while (u <= nbSides * 2 + 1)
    {
        float rad = (float)u / nbSides * Math::TwoPI;
        m.UV0.emplace_back(std::cos(rad) * .5f + .5f, std::sin(rad) * .5f + .5f);
        u++;
    }

    // Sides
    int u_sides = 0;
    while (u <= vertNumber - 4)
    {
        float t = (float)u_sides / nbSides;
        m.UV0.emplace_back(t, 1.0f);
        m.UV0.emplace_back(t, 0.0f);
        u += 2;
        u_sides++;
    }
    m.UV0.emplace_back(1.0f, 1.0f);
    m.UV0.emplace_back(1.0f, 0.0f);

    int nbTriangles = nbSides + nbSides + nbSides * 2;
    m.Triangles.reserve(nbTriangles);

    // Bottom cap
    int tri = 0;
    int i = 0;
    while (tri < nbSides - 1)
    {
        m.Triangles.push_back(0);
        m.Triangles.push_back(tri + 1);
        m.Triangles.push_back(tri + 2);
        tri++;
        i += 3;
    }
    m.Triangles.push_back(0);
    m.Triangles.push_back(tri + 1);
    m.Triangles.push_back(1);
    tri++;
    i += 3;

    // Top cap
    //tri++;
    while (tri < nbSides * 2)
    {
        m.Triangles.push_back(tri + 2);
        m.Triangles.push_back(tri + 1);
        m.Triangles.push_back(nbVerticesCap);
        tri++;
        i += 3;
    }

    m.Triangles.push_back(nbVerticesCap + 1);
    m.Triangles.push_back(tri + 1);
    m.Triangles.push_back(nbVerticesCap);
    tri++;
    i += 3;
    tri++;

    // Sides
    while (tri <= nbTriangles)
    {
        m.Triangles.push_back(tri + 2);
        m.Triangles.push_back(tri + 1);
        m.Triangles.push_back(tri + 0);
        tri++;
        i += 3;

        m.Triangles.push_back(tri + 1);
        m.Triangles.push_back(tri + 2);
        m.Triangles.push_back(tri + 0);
        tri++;
        i += 3;
    }
    m.PrepareForUpload();
    return m;
}

}
