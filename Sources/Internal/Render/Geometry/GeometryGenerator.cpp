//
// Copyright (C) Alexandr Vorontsov. 2017
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//
// Adapted from http://wiki.unity3d.com/index.php/ProceduralPrimitives

#include "stdafx.h"

#include "Render/Geometry/GeometryGenerator.h"

#include <vector>
#include <map>

#include "Render/Geometry/Mesh.h"
#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"

namespace Kioto::GeometryGenerator
{
namespace
{
template <typename T, typename = std::enable_if_t<std::is_same_v<T, byte> || std::is_same_v<T, uint16> || std::is_same_v<T, uint32> || std::is_same_v<T, uint64>>>
struct TriangleIndices
{
    T i1 = 0;
    T i2 = 0;
    T i3 = 0;

    TriangleIndices(T i1_, T i2_, T i3_)
    {
        i1 = i1_;
        i2 = i2_;
        i3 = i3_;
    }
};

// [a_vorontsov] Return index of point in the middle of p1 and p2 (for the icosphere).
template <typename T, typename = std::enable_if_t<std::is_same_v<T, byte> || std::is_same_v<T, uint16> || std::is_same_v<T, uint32> || std::is_same_v<T, uint64>>>
T GetMiddlePoint(T p1, T p2, std::vector<Vector3>& vertices, std::map<uint64, T>& cache, float radius)
{
    // [a_vorontsov] First check if we have it already.
    bool firstIsSmaller = p1 < p2;
    uint64 smallerIndex = firstIsSmaller ? p1 : p2;
    uint64 greaterIndex = firstIsSmaller ? p2 : p1;
    uint64 key = (smallerIndex << 32) + greaterIndex;

    auto it = cache.find(key);
    if (it != cache.end())
    {
        return it->second;
    }

    // [a_vorontsov] Not in cache, calculate it.
    Vector3 point1 = vertices[p1];
    Vector3 point2 = vertices[p2];
    Vector3 middle
    (
        (point1.x + point2.x) / 2.0f,
        (point1.y + point2.y) / 2.0f,
        (point1.z + point2.z) / 2.0f
    );

    // [a_vorontsov] Add vertex makes sure point is on unit sphere.
    T i = static_cast<T>(vertices.size());
    vertices.push_back(middle.Normalized() * radius);

    // [a_vorontsov] Store it, return index.
    cache[key] = i;
    return i;
}
}

Mesh GeometryGenerator::GeneratePlane(float32 sizeX /*= 1.0f*/, float32 sizeZ /*= 1.0f*/)
{
    uint32 resX = 2; // [a_vorontsov] 2 minimum.
    uint32 resZ = 2;
    uint32 vCount = resX * resZ;
    uint32 stride = sizeof(Vector3) + sizeof(Vector3) + sizeof(Vector2);
    byte* vData = new byte[vCount * stride];
    byte* vDataBegin = vData;

    for (uint32 z = 0; z < resZ; z++)
    {
        // [a_vorontsov] [ -length / 2, length / 2 ]
        float32 zPos = (static_cast<float32>(z) / (resZ - 1) - .5f) * sizeX;
        for (uint32 x = 0; x < resX; x++)
        {
            // [a_vorontsov] [ -width / 2, width / 2 ]
            float32 xPos = (static_cast<float32>(x) / (resX - 1) - .5f) * sizeZ;
            *reinterpret_cast<Vector3*>(vData) = Vector3(xPos, 0.0f, zPos);
            vData += stride;
        }
    }
    vData = vDataBegin;
    vData += sizeof(Vector3);

    for (uint32 n = 0; n < vCount; n++)
    {
        *reinterpret_cast<Vector3*>(vData) = Vector3(0.0f, 1.0f, 0.0f);
        vData += stride;
    }

    vData = vDataBegin;
    vData += sizeof(Vector3) + sizeof(Vector3);
    for (uint32 v = 0; v < resZ; v++)
    {
        for (uint32 u = 0; u < resX; u++)
        {
            *reinterpret_cast<Vector2*>(vData) = Vector2(static_cast<float32>(u) / (resX - 1), static_cast<float32>(v) / (resZ - 1));
            vData += stride;
        }
    }
    vData = vDataBegin;

    uint32 nbFaces = (resX - 1) * (resZ - 1);
    uint16 iCount = nbFaces * 6;
    uint16* iData = new uint16[iCount];
    byte* iDataBegin = reinterpret_cast<byte*>(iData);
    uint32 t = 0;
    for (uint16 face = 0; face < nbFaces; face++)
    {
        // [a_vorontsov] Retrieve lower left corner from face ind.
        uint32 i = face % (resX - 1) + (face / (resZ - 1) * resX);

        *iData++ = i + resX;
        *iData++ = i + 1;
        *iData++ = i;

        *iData++ = i + resX;
        *iData++ = i + resX + 1;
        *iData++ = i + 1;
    }

    return { vDataBegin, stride * vCount, stride, vCount, iDataBegin, iCount * sizeof(uint16), iCount, eIndexFormat::Format16Bit };
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
    byte* vData = new byte[stride * 24];
    byte* vertBegin = vData;
    // [a_vorontsov] Bottom.
    *reinterpret_cast<Vector3*>(vData) = p0; vData += stride;
    *reinterpret_cast<Vector3*>(vData) = p1; vData += stride;
    *reinterpret_cast<Vector3*>(vData) = p2; vData += stride;
    *reinterpret_cast<Vector3*>(vData) = p3; vData += stride;
    // [a_vorontsov] Left.
    *reinterpret_cast<Vector3*>(vData) = p7; vData += stride;
    *reinterpret_cast<Vector3*>(vData) = p4; vData += stride;
    *reinterpret_cast<Vector3*>(vData) = p0; vData += stride;
    *reinterpret_cast<Vector3*>(vData) = p3; vData += stride;
    // [a_vorontsov] Front.
    *reinterpret_cast<Vector3*>(vData) = p4; vData += stride;
    *reinterpret_cast<Vector3*>(vData) = p5; vData += stride;
    *reinterpret_cast<Vector3*>(vData) = p1; vData += stride;
    *reinterpret_cast<Vector3*>(vData) = p0; vData += stride;
    // [a_vorontsov] Back.
    *reinterpret_cast<Vector3*>(vData) = p6; vData += stride;
    *reinterpret_cast<Vector3*>(vData) = p7; vData += stride;
    *reinterpret_cast<Vector3*>(vData) = p3; vData += stride;
    *reinterpret_cast<Vector3*>(vData) = p2; vData += stride;
    // [a_vorontsov] Right.
    *reinterpret_cast<Vector3*>(vData) = p5; vData += stride;
    *reinterpret_cast<Vector3*>(vData) = p6; vData += stride;
    *reinterpret_cast<Vector3*>(vData) = p2; vData += stride;
    *reinterpret_cast<Vector3*>(vData) = p1; vData += stride;
    // [a_vorontsov] Top.
    *reinterpret_cast<Vector3*>(vData) = p7; vData += stride;
    *reinterpret_cast<Vector3*>(vData) = p6; vData += stride;
    *reinterpret_cast<Vector3*>(vData) = p5; vData += stride;
    *reinterpret_cast<Vector3*>(vData) = p4; vData = vertBegin;
    vData += sizeof(Vector3);

    Vector3 up(0.0f, 1.0f, 0.0f);
    Vector3 down(0.0f, -1.0f, 0.0f);
    Vector3 front(0.0f, 0.0f, 1.0f);
    Vector3 back(0.0f, 0.0f, -1.0f);
    Vector3 left(1.0f, 0.0f, 0.0f);
    Vector3 right(-1.0f, 0.0f, 0.0f);

    // [a_vorontsov] Normals. Bottom.
    *reinterpret_cast<Vector3*>(vData) = down; vData += stride;
    *reinterpret_cast<Vector3*>(vData) = down; vData += stride;
    *reinterpret_cast<Vector3*>(vData) = down; vData += stride;
    *reinterpret_cast<Vector3*>(vData) = down; vData += stride;
    // [a_vorontsov] Left.
    *reinterpret_cast<Vector3*>(vData) = left; vData += stride;
    *reinterpret_cast<Vector3*>(vData) = left; vData += stride;
    *reinterpret_cast<Vector3*>(vData) = left; vData += stride;
    *reinterpret_cast<Vector3*>(vData) = left; vData += stride;
    // [a_vorontsov] Front.
    *reinterpret_cast<Vector3*>(vData) = front; vData += stride;
    *reinterpret_cast<Vector3*>(vData) = front; vData += stride;
    *reinterpret_cast<Vector3*>(vData) = front; vData += stride;
    *reinterpret_cast<Vector3*>(vData) = front; vData += stride;
    // [a_vorontsov] Back.
    *reinterpret_cast<Vector3*>(vData) = back; vData += stride;
    *reinterpret_cast<Vector3*>(vData) = back; vData += stride;
    *reinterpret_cast<Vector3*>(vData) = back; vData += stride;
    *reinterpret_cast<Vector3*>(vData) = back; vData += stride;
    // [a_vorontsov] Right.
    *reinterpret_cast<Vector3*>(vData) = right; vData += stride;
    *reinterpret_cast<Vector3*>(vData) = right; vData += stride;
    *reinterpret_cast<Vector3*>(vData) = right; vData += stride;
    *reinterpret_cast<Vector3*>(vData) = right; vData += stride;
    // [a_vorontsov] Top.
    *reinterpret_cast<Vector3*>(vData) = up; vData += stride;
    *reinterpret_cast<Vector3*>(vData) = up; vData += stride;
    *reinterpret_cast<Vector3*>(vData) = up; vData += stride;
    *reinterpret_cast<Vector3*>(vData) = up; vData = vertBegin;
    vData += sizeof(Vector3) + sizeof(Vector3);

    Vector2 _00(0.0f, 0.0f);
    Vector2 _10(1.0f, 0.0f);
    Vector2 _01(0.0f, 1.0f);
    Vector2 _11(1.0f, 1.0f);

    // [a_vorontsov] UV. Bottom.
    *reinterpret_cast<Vector2*>(vData) = _11; vData += stride;
    *reinterpret_cast<Vector2*>(vData) = _01; vData += stride;
    *reinterpret_cast<Vector2*>(vData) = _00; vData += stride;
    *reinterpret_cast<Vector2*>(vData) = _10; vData += stride;
    // [a_vorontsov] Left.
    *reinterpret_cast<Vector2*>(vData) = _11; vData += stride;
    *reinterpret_cast<Vector2*>(vData) = _01; vData += stride;
    *reinterpret_cast<Vector2*>(vData) = _00; vData += stride;
    *reinterpret_cast<Vector2*>(vData) = _10; vData += stride;
    // [a_vorontsov] Front.
    *reinterpret_cast<Vector2*>(vData) = _11; vData += stride;
    *reinterpret_cast<Vector2*>(vData) = _01; vData += stride;
    *reinterpret_cast<Vector2*>(vData) = _00; vData += stride;
    *reinterpret_cast<Vector2*>(vData) = _10; vData += stride;
    // [a_vorontsov] Back.
    *reinterpret_cast<Vector2*>(vData) = _11; vData += stride;
    *reinterpret_cast<Vector2*>(vData) = _01; vData += stride;
    *reinterpret_cast<Vector2*>(vData) = _00; vData += stride;
    *reinterpret_cast<Vector2*>(vData) = _10; vData += stride;
    // [a_vorontsov] Right.
    *reinterpret_cast<Vector2*>(vData) = _11; vData += stride;
    *reinterpret_cast<Vector2*>(vData) = _01; vData += stride;
    *reinterpret_cast<Vector2*>(vData) = _00; vData += stride;
    *reinterpret_cast<Vector2*>(vData) = _10; vData += stride;
    // [a_vorontsov] Top.
    *reinterpret_cast<Vector2*>(vData) = _11; vData += stride;
    *reinterpret_cast<Vector2*>(vData) = _01; vData += stride;
    *reinterpret_cast<Vector2*>(vData) = _00; vData += stride;
    *reinterpret_cast<Vector2*>(vData) = _10; vData = vertBegin;

    uint32* iData = new uint32[36];
    uint32* iDataBegin = iData;
    // [a_vorontsov] Bottom.
    *iData++ = 3;
    *iData++ = 1;
    *iData++ = 0;

    *iData++ = 3;
    *iData++ = 2;
    *iData++ = 1;

    // [a_vorontsov] Left.
    *iData++ = 7;
    *iData++ = 5;
    *iData++ = 4;

    *iData++ = 7;
    *iData++ = 6;
    *iData++ = 5;

    // [a_vorontsov] Front.
    *iData++ = 11;
    *iData++ = 9;
    *iData++ = 8;

    *iData++ = 11;
    *iData++ = 10;
    *iData++ = 9;

    // [a_vorontsov] Back.
    *iData++ = 15;
    *iData++ = 13;
    *iData++ = 12;

    *iData++ = 15;
    *iData++ = 14;
    *iData++ = 13;

    // [a_vorontsov] Right.
    *iData++ = 19;
    *iData++ = 17;
    *iData++ = 16;

    *iData++ = 19;
    *iData++ = 18;
    *iData++ = 17;

    // [a_vorontsov] Top.
    *iData++ = 23;
    *iData++ = 21;
    *iData++ = 20;

    *iData++ = 23;
    *iData++ = 22;
    *iData++ = 21;

    iData = iDataBegin;

    return { vData, stride * 24, stride, 24, reinterpret_cast<byte*>(iData), 36 * sizeof(uint32), 36, eIndexFormat::Format32Bit };
}

Mesh GenerateCone(float32 height /*= 1.0f*/, float32 bottomRadius /*= 0.25f*/, float32 topRadius /*= 0.05f*/)
{
    uint32 nbSides = 18;
    uint32 nbHeightSeg = 1; // [a_vorontsov] Not implemented yet.

    uint32 nbVerticesCap = nbSides + 1;

    uint32 vCount = nbVerticesCap + nbVerticesCap + nbSides * nbHeightSeg * 2 + 2;

    uint32 stride = sizeof(Vector3) + sizeof(Vector3) + sizeof(Vector2);
    byte* vData = new byte[stride * vCount];
    byte* vDataBegin = vData;

    uint32 vert = 0;

    // [a_vorontsov] Bottom cap.
    *reinterpret_cast<Vector3*>(vData) = Vector3(0.0f, 0.0f, 0.0f);
    vData += stride;

    vert++;
    while (vert <= nbSides)
    {
        float32 rad = static_cast<float32>(vert) / nbSides * Math::TwoPI;
        *reinterpret_cast<Vector3*>(vData) = Vector3(std::cos(rad) * bottomRadius, 0.0f, std::sin(rad) * bottomRadius);
        vData += stride;
        vert++;
    }

    // [a_vorontsov] Top cap.
    *reinterpret_cast<Vector3*>(vData) = Vector3(0.0f, height, 0.0f);
    vData += stride;
    vert++;
    while (vert <= nbSides * 2 + 1)
    {
        float32 rad = static_cast<float32>(vert - nbSides - 1) / nbSides * Math::TwoPI;
        *reinterpret_cast<Vector3*>(vData) = Vector3(std::cos(rad) * topRadius, height, std::sin(rad) * topRadius);
        vData += stride;
        vert++;
    }

    // [a_vorontsov] Sides.
    int32 v = 0;
    while (vert <= vCount - 4)
    {
        float32 rad = static_cast<float32>(v) / nbSides * Math::TwoPI;
        *reinterpret_cast<Vector3*>(vData) = Vector3(std::cos(rad) * topRadius, height, std::sin(rad) * topRadius);
        vData += stride;
        *reinterpret_cast<Vector3*>(vData) = Vector3(std::cos(rad) * bottomRadius, 0.0f, std::sin(rad) * bottomRadius);
        vData += stride;
        vert += 2;
        v++;
    }
    *reinterpret_cast<Vector3*>(vData) = *reinterpret_cast<Vector3*>(vDataBegin + stride * (nbSides * 2 + 2));
    vData += stride;
    *reinterpret_cast<Vector3*>(vData) = *reinterpret_cast<Vector3*>(vDataBegin + stride * (nbSides * 2 + 3));
    vData = vDataBegin;
    vData += sizeof(Vector3);

    vert = 0;

    // [a_vorontsov] Bottom cap.
    while (vert <= nbSides)
    {
        *reinterpret_cast<Vector3*>(vData) = Vector3(0.0f, -1.0f, 0.0f);
        vData += stride;
        vert++;
    }

    // [a_vorontsov] Top cap.
    while (vert <= nbSides * 2 + 1)
    {
        *reinterpret_cast<Vector3*>(vData) = Vector3(0.0f, 1.0f, 0.0f);
        vData += stride;
        vert++;
    }

    // [a_vorontsov] Sides.
    v = 0;
    while (vert <= vCount - 4)
    {
        float32 rad = static_cast<float32>(v) / nbSides * Math::TwoPI;
        float32 cos = std::cos(rad);
        float32 sin = std::sin(rad);

        *reinterpret_cast<Vector3*>(vData) = Vector3(cos, 0.0f, sin);
        vData += stride;
        *reinterpret_cast<Vector3*>(vData) = Vector3(cos, 0.0f, sin);
        vData += stride;

        vert += 2;
        v++;
    }

    *reinterpret_cast<Vector3*>(vData) = *reinterpret_cast<Vector3*>(vDataBegin + stride * (nbSides * 2 + 2) + sizeof(Vector3));
    vData += stride;
    *reinterpret_cast<Vector3*>(vData) = *reinterpret_cast<Vector3*>(vDataBegin + stride * (nbSides * 2 + 2) + sizeof(Vector3));
    vData = vDataBegin;
    vData += sizeof(Vector3) + sizeof(Vector3);

    // [a_vorontsov] Bottom cap.
    uint32 u = 0;
    *reinterpret_cast<Vector2*>(vData) = Vector2(0.5f, 0.5f);
    vData += stride;
    u++;
    while (u <= nbSides)
    {
        float32 rad = static_cast<float32>(u) / nbSides * Math::TwoPI;
        *reinterpret_cast<Vector2*>(vData) = Vector2(std::cos(rad) * 0.5f + 0.5f, std::sin(rad) * 0.5f + 0.5f);
        vData += stride;
        u++;
    }

    // [a_vorontsov] Top cap.
    *reinterpret_cast<Vector2*>(vData) = Vector2(0.5f, 0.5f);
    vData += stride;
    u++;
    while (u <= nbSides * 2 + 1)
    {
        float32 rad = static_cast<float32>(u) / nbSides * Math::TwoPI;
        *reinterpret_cast<Vector2*>(vData) = Vector2(std::cos(rad) * 0.5f + 0.5f, std::sin(rad) * 0.5f + 0.5f);
        vData += stride;
        u++;
    }

    // [a_vorontsov] Sides.
    int32 u_sides = 0;
    while (u <= vCount - 4)
    {
        float32 t = static_cast<float32>(u_sides) / nbSides;
        *reinterpret_cast<Vector2*>(vData) = Vector2(t, 1.0f);
        vData += stride;
        *reinterpret_cast<Vector2*>(vData) = Vector2(t, 0.0f);
        vData += stride;
        u += 2;
        u_sides++;
    }
    *reinterpret_cast<Vector2*>(vData) = Vector2(1.0f, 1.0f);
    vData += stride;
    *reinterpret_cast<Vector2*>(vData) = Vector2(1.0f, 0.0f);
    vData += stride;

    uint32 nbTriangles = nbSides + nbSides + nbSides * 2;
    uint32 iCount = nbTriangles * 3 + 3;
    uint16* iData = new uint16[iCount];
    byte* iDataBegin = reinterpret_cast<byte*>(iData);

    // [a_vorontsov] Bottom cap.
    uint32 tri = 0;
    while (tri < nbSides - 1)
    {
        *iData++ = 0;
        *iData++ = tri + 1;
        *iData++ = tri + 2;
        tri++;
    }
    *iData++ = 0;
    *iData++ = tri + 1;
    *iData++ = 1;
    tri++;

    // [a_vorontsov] Top cap.
    while (tri < nbSides * 2)
    {
        *iData++ = tri + 2;
        *iData++ = tri + 1;
        *iData++ = nbVerticesCap;
        tri++;
    }

    *iData++ = nbVerticesCap + 1;
    *iData++ = tri + 1;
    *iData++ = nbVerticesCap;
    tri += 2;

    // [a_vorontsov] Sides.
    while (tri <= nbTriangles)
    {
        *iData++ = tri + 2;
        *iData++ = tri + 1;
        *iData++ = tri + 0;
        tri++;

        *iData++ = tri + 1;
        *iData++ = tri + 2;
        *iData++ = tri + 0;
        tri++;
    }
    return { vDataBegin, vCount * stride, stride, vCount, iDataBegin, iCount * sizeof(uint16), iCount, eIndexFormat::Format16Bit };
}

Mesh GenerateSphere(float32 radius /*=1.0f*/)
{
    int32 nbLong = 24;
    int32 nbLat = 16;

    uint32 vCount = (nbLong + 1) * nbLat + 2;
    uint32 stride = sizeof(Vector3) + sizeof(Vector3) + sizeof(Vector2);
    byte* vData = new byte[vCount * stride];
    byte* vDataBegin = vData;

    *reinterpret_cast<Vector3*>(vData) = Vector3(0.0f, 1.0f, 0.0) * radius;
    vData += stride;

    for (int32 lat = 0; lat < nbLat; lat++)
    {
        float32 a1 = Math::PI * static_cast<float32>(lat + 1) / (nbLat + 1);
        float32 sin1 = std::sin(a1);
        float32 cos1 = std::cos(a1);

        for (int32 lon = 0; lon <= nbLong; lon++)
        {
            float32 a2 = Math::TwoPI * static_cast<float32>(lon == nbLong ? 0 : lon) / nbLong;
            float32 sin2 = std::sin(a2);
            float32 cos2 = std::cos(a2);

            *reinterpret_cast<Vector3*>(vData) = Vector3(sin1 * cos2, cos1, sin1 * sin2) * radius;
            vData += stride;
        }
    }
    *reinterpret_cast<Vector3*>(vData) = Vector3(0.0f, -1.0f, 0.0f) * radius;
    vData = vDataBegin;
    vData += sizeof(Vector3);

    for (uint32 n = 0; n < vCount; n++)
    {
        *reinterpret_cast<Vector3*>(vData) = *reinterpret_cast<Vector3*>(vDataBegin + stride * n);
        vData += stride;
    }

    vData = vDataBegin;
    vData += sizeof(Vector3) + sizeof(Vector3);

    *reinterpret_cast<Vector2*>(vData) = Vector2(0.0f, 1.0f);
    vData += stride;

    for (int lat = 0; lat < nbLat; lat++)
    {
        for (int lon = 0; lon <= nbLong; lon++)
        {
            *reinterpret_cast<Vector2*>(vData) = Vector2(static_cast<float32>(lon) / nbLong, 1.0f - static_cast<float32>(lat + 1) / (nbLat + 1));
            vData += stride;
        }
    }
    *reinterpret_cast<Vector2*>(vData) = Vector2(0.0f, 0.0f);

    uint32 nbFaces = vCount;
    uint32 nbTriangles = nbFaces * 2;
    uint32 iCount = nbTriangles * 3;
    uint32* iData = new uint32[iCount];
    byte* iDataBegin = reinterpret_cast<byte*>(iData);

    // [a_vorontsov] Top cap.
    int32 i = 0;
    for (int32 lon = 0; lon < nbLong; lon++)
    {
        *iData++ = lon + 2;
        *iData++ = lon + 1;
        *iData++ = 0;
    }

    // [a_vorontsov] Middle.
    for (int32 lat = 0; lat < nbLat - 1; lat++)
    {
        for (int32 lon = 0; lon < nbLong; lon++)
        {
            int32 current = lon + lat * (nbLong + 1) + 1;
            int32 next = current + nbLong + 1;

            *iData++ = current;
            *iData++ = current + 1;
            *iData++ = next + 1;

            *iData++ = current;
            *iData++ = next + 1;
            *iData++ = next;
        }
    }

    // [a_vorontsov] Bottom cap.
    for (int32 lon = 0; lon < nbLong; lon++)
    {
        *iData++ = vCount - 1;
        *iData++ = vCount - (lon + 2) - 1;
        *iData++ = vCount - (lon + 1) - 1;
    }
    return { vDataBegin, stride * vCount, stride, vCount, iDataBegin, iCount * sizeof(uint32), iCount, eIndexFormat::Format32Bit };
}

Mesh GenerateTube(float32 height /*= 1.0f*/, float32 bottomRadius1 /*= 0.5f*/, float32 bottomRadius2 /*= 0.15f*/, float32 topRadius1 /*= 0.5f*/, float32 topRadius2 /*= 0.15f*/)
{
    int32 nbSides = 24;

    // [a_vorontsov] Outter shell is at radius1 + radius2 / 2, inner shell at radius1 - radius2 / 2.
    uint32 nbVerticesCap = nbSides * 2 + 2;
    uint32 nbVerticesSides = nbSides * 2 + 2;
    uint32 vCount = nbVerticesCap * 2 + nbVerticesSides * 2;

    uint32 stride = sizeof(Vector3) + sizeof(Vector3) + sizeof(Vector2);
    byte* vData = new byte[vCount * stride];
    byte* vDataBegin = vData;

    uint32 vert = 0;


    // [a_vorontsov] Bottom cap.
    int32 sideCounter = 0;
    while (vert < nbVerticesCap)
    {
        sideCounter = sideCounter == nbSides ? 0 : sideCounter;

        float32 r1 = static_cast<float32>(sideCounter++) / nbSides * Math::TwoPI;
        float32 cos = std::cos(r1);
        float32 sin = std::sin(r1);
        *reinterpret_cast<Vector3*>(vData) = Vector3(cos * (bottomRadius1 - bottomRadius2 * 0.5f), 0.0f, sin * (bottomRadius1 - bottomRadius2 * 0.5f));
        vData += stride;
        *reinterpret_cast<Vector3*>(vData) = Vector3(cos * (bottomRadius1 + bottomRadius2 * 0.5f), 0.0f, sin * (bottomRadius1 + bottomRadius2 * 0.5f));
        vData += stride;
        vert += 2;
    }

    // [a_vorontsov] Top cap.
    sideCounter = 0;
    while (vert < nbVerticesCap * 2)
    {
        sideCounter = sideCounter == nbSides ? 0 : sideCounter;

        float32 r1 = static_cast<float32>(sideCounter++) / nbSides * Math::TwoPI;
        float32 cos = std::cos(r1);
        float32 sin = std::sin(r1);
        *reinterpret_cast<Vector3*>(vData) = Vector3(cos * (topRadius1 - topRadius2 * 0.5f), height, sin * (topRadius1 - topRadius2 * 0.5f));
        vData += stride;
        *reinterpret_cast<Vector3*>(vData) = Vector3(cos * (topRadius1 + topRadius2 * 0.5f), height, sin * (topRadius1 + topRadius2 * 0.5f));
        vData += stride;
        vert += 2;
    }

    // [a_vorontsov] Sides (out).
    sideCounter = 0;
    while (vert < nbVerticesCap * 2 + nbVerticesSides)
    {
        sideCounter = sideCounter == nbSides ? 0 : sideCounter;

        float32 r1 = static_cast<float>(sideCounter++) / nbSides * Math::TwoPI;
        float32 cos = std::cos(r1);
        float32 sin = std::sin(r1);

        *reinterpret_cast<Vector3*>(vData) = Vector3(cos * (topRadius1 + topRadius2 * .5f), height, sin * (topRadius1 + topRadius2 * 0.5f));
        vData += stride;
        *reinterpret_cast<Vector3*>(vData) = Vector3(cos * (bottomRadius1 + bottomRadius2 * 0.5f), 0.0f, sin * (bottomRadius1 + bottomRadius2 * 0.5f));
        vData += stride;
        vert += 2;
    }

    // [a_vorontsov] Sides (in).
    sideCounter = 0;
    while (vert < vCount)
    {
        sideCounter = sideCounter == nbSides ? 0 : sideCounter;

        float32 r1 = static_cast<float32>(sideCounter++) / nbSides * Math::TwoPI;
        float32 cos = std::cos(r1);
        float32 sin = std::sin(r1);

        *reinterpret_cast<Vector3*>(vData) = Vector3(cos * (topRadius1 - topRadius2 * 0.5f), height, sin * (topRadius1 - topRadius2 * 0.5f));
        vData += stride;
        *reinterpret_cast<Vector3*>(vData) = Vector3(cos * (bottomRadius1 - bottomRadius2 * 0.5f), 0.0f, sin * (bottomRadius1 - bottomRadius2 * 0.5f));
        vData += stride;
        vert += 2;
    }

    vert = 0;
    vData = vDataBegin;
    vData += sizeof(Vector3);
    // [a_vorontsov] Bottom cap.
    while (vert < nbVerticesCap)
    {
        *reinterpret_cast<Vector3*>(vData) = Vector3(0.0f, -1.0f, 0.0f);
        vData += stride;
        vert++;
    }

    // [a_vorontsov] Top cap.
    while (vert < nbVerticesCap * 2)
    {
        *reinterpret_cast<Vector3*>(vData) = Vector3(0.0f, 1.0f, 0.0f);
        vData += stride;
        vert++;
    }

    // [a_vorontsov] Sides (out).
    sideCounter = 0;
    while (vert < nbVerticesCap * 2 + nbVerticesSides)
    {
        sideCounter = sideCounter == nbSides ? 0 : sideCounter;

        float32 r1 = static_cast<float32>(sideCounter++) / nbSides * Math::TwoPI;

        *reinterpret_cast<Vector3*>(vData) = Vector3(std::cos(r1), 0.0f, std::sin(r1));
        vData += stride;
        *reinterpret_cast<Vector3*>(vData) = Vector3(std::cos(r1), 0.0f, std::sin(r1));
        vData += stride;

        vert += 2;
    }

    // [a_vorontsov] Sides (in).
    sideCounter = 0;
    while (vert < vCount)
    {
        sideCounter = sideCounter == nbSides ? 0 : sideCounter;

        float32 r1 = static_cast<float32>(sideCounter++) / nbSides * Math::TwoPI;

        *reinterpret_cast<Vector3*>(vData) = Vector3(-std::cos(r1), 0.0f, -std::sin(r1));
        vData += stride;
        *reinterpret_cast<Vector3*>(vData) = Vector3(-std::cos(r1), 0.0f, -std::sin(r1));
        vData += stride;

        vert += 2;
    }
    vData = vDataBegin;
    vData += sizeof(Vector3) + sizeof(Vector3);

    vert = 0;
    // [a_vorontsov] Bottom cap.
    sideCounter = 0;
    while (vert < nbVerticesCap)
    {
        float32 t = static_cast<float32>(sideCounter++) / nbSides;
        *reinterpret_cast<Vector2*>(vData) = Vector2(0.0f, t);
        vData += stride;
        *reinterpret_cast<Vector2*>(vData) = Vector2(1.0f, t);
        vData += stride;

        vert += 2;
    }

    // [a_vorontsov] Top cap.
    sideCounter = 0;
    while (vert < nbVerticesCap * 2)
    {
        float32 t = static_cast<float32>(sideCounter++) / nbSides;
        *reinterpret_cast<Vector2*>(vData) = Vector2(0.0f, t);
        vData += stride;
        *reinterpret_cast<Vector2*>(vData) = Vector2(1.0f, t);
        vData += stride;
        vert += 2;
    }

    // [a_vorontsov] Sides (out).
    sideCounter = 0;
    while (vert < nbVerticesCap * 2 + nbVerticesSides)
    {
        float32 t = static_cast<float32>(sideCounter++) / nbSides;
        *reinterpret_cast<Vector2*>(vData) = Vector2(0.0f, t);
        vData += stride;
        *reinterpret_cast<Vector2*>(vData) = Vector2(1.0f, t);
        vData += stride;
        vert += 2;
    }

    // [a_vorontsov] Sides (in).
    sideCounter = 0;
    while (vert < vCount)
    {
        float32 t = static_cast<float>(sideCounter++) / nbSides;
        *reinterpret_cast<Vector2*>(vData) = Vector2(0.0f, t);
        vData += stride;
        *reinterpret_cast<Vector2*>(vData) = Vector2(1.0f, t);
        vData += stride;
        vert += 2;
    }

    int32 nbFace = nbSides * 4;
    int32 nbTriangles = nbFace * 2;
    uint32 iCount = nbTriangles * 3;
    uint16* iData = new uint16[iCount];
    byte* iDataBegin = reinterpret_cast<byte*>(iData);

    // [a_vorontsov] Bottom cap.
    int32 i = 0;
    sideCounter = 0;
    while (sideCounter < nbSides)
    {
        int16 current = sideCounter * 2;
        int16 next = sideCounter * 2 + 2;

        *iData++ = next + 1;
        *iData++ = next;
        *iData++ = current;

        *iData++ = current + 1;
        *iData++ = next + 1;
        *iData++ = current;

        sideCounter++;
    }

    // [a_vorontsov] Top cap.
    while (sideCounter < nbSides * 2)
    {
        int16 current = sideCounter * 2 + 2;
        int16 next = sideCounter * 2 + 4;

        *iData++ = current;
        *iData++ = next;
        *iData++ = next + 1;

        *iData++ = current;
        *iData++ = next + 1;
        *iData++ = current + 1;

        sideCounter++;
    }

    // [a_vorontsov] Sides (out).
    while (sideCounter < nbSides * 3)
    {
        int16 current = sideCounter * 2 + 4;
        int16 next = sideCounter * 2 + 6;

        *iData++ = current;
        *iData++ = next;
        *iData++ = next + 1;

        *iData++ = current;
        *iData++ = next + 1;
        *iData++ = current + 1;

        sideCounter++;
    }


    // [a_vorontsov] Sides (in).
    while (sideCounter < nbSides * 4)
    {
        int16 current = sideCounter * 2 + 6;
        int16 next = sideCounter * 2 + 8;

        *iData++ = next + 1;
        *iData++ = next;
        *iData++ = current;

        *iData++ = current + 1;
        *iData++ = next + 1;
        *iData++ = current;

        sideCounter++;
    }
    return { vDataBegin, vCount * stride, stride, vCount, iDataBegin, iCount * sizeof(uint16), iCount, eIndexFormat::Format16Bit };
}

Mesh GenerateIcosphere(int32 recursionLevel /*= 3*/, float32 radius /*= 1.0f*/)
{
    std::vector<Vector3> positions;
    std::map<uint64, uint16> middlePointIndexCache;

    // [a_vorontsov] Create 12 vertices of a icosahedron.
    float32 t = (1.0f + std::sqrt(5.0f)) / 2.0f;

    positions.push_back(Vector3(-1.0f, t, 0.0f).Normalized() * radius);
    positions.push_back(Vector3(1.0f, t, 0.0f).Normalized() * radius);
    positions.push_back(Vector3(-1.0f, -t, 0.0f).Normalized() * radius);
    positions.push_back(Vector3(1.0f, -t, 0.0f).Normalized() * radius);

    positions.push_back(Vector3(0.0f, -1.0f, t).Normalized() * radius);
    positions.push_back(Vector3(0.0f, 1.0f, t).Normalized() * radius);
    positions.push_back(Vector3(0.0f, -1.0f, -t).Normalized() * radius);
    positions.push_back(Vector3(0.0f, 1.0f, -t).Normalized() * radius);

    positions.push_back(Vector3(t, 0.0f, -1.0f).Normalized() * radius);
    positions.push_back(Vector3(t, 0.0f, 1.0f).Normalized() * radius);
    positions.push_back(Vector3(-t, 0.0f, -1.0f).Normalized() * radius);
    positions.push_back(Vector3(-t, 0.0f, 1.0f).Normalized() * radius);

    // [a_vorontsov] Create 20 triangles of the icosahedron.
    std::vector<TriangleIndices<uint16>> faces;

    // [a_vorontsov] 5 faces around point 0.
    faces.emplace_back(0, 11, 5);
    faces.emplace_back(0, 5, 1);
    faces.emplace_back(0, 1, 7);
    faces.emplace_back(0, 7, 10);
    faces.emplace_back(0, 10, 11);

    // [a_vorontsov] 5 adjacent faces.
    faces.emplace_back(1, 5, 9);
    faces.emplace_back(5, 11, 4);
    faces.emplace_back(11, 10, 2);
    faces.emplace_back(10, 7, 6);
    faces.emplace_back(7, 1, 8);

    // [a_vorontsov] 5 faces around point 3.
    faces.emplace_back(3, 9, 4);
    faces.emplace_back(3, 4, 2);
    faces.emplace_back(3, 2, 6);
    faces.emplace_back(3, 6, 8);
    faces.emplace_back(3, 8, 9);

    // [a_vorontsov] 5 adjacent faces.
    faces.emplace_back(4, 9, 5);
    faces.emplace_back(2, 4, 11);
    faces.emplace_back(6, 2, 10);
    faces.emplace_back(8, 6, 7);
    faces.emplace_back(9, 8, 1);

    // [a_vorontsov] Refine triangles.
    std::vector<TriangleIndices<uint16>> faces2;
    faces2.reserve(20);
    for (int32 i = 0; i < recursionLevel; i++)
    {
        faces2.clear();
        for(auto& tri : faces)
        {
            uint16 a = GetMiddlePoint(tri.i1, tri.i2, positions, middlePointIndexCache, radius);
            uint16 b = GetMiddlePoint(tri.i2, tri.i3, positions, middlePointIndexCache, radius);
            uint16 c = GetMiddlePoint(tri.i3, tri.i1, positions, middlePointIndexCache, radius);

            faces2.emplace_back(tri.i1, a, c);
            faces2.emplace_back(tri.i2, b, a);
            faces2.emplace_back(tri.i3, c, b);
            faces2.emplace_back(a, b, c);
        }
        faces = faces2;
    }
    uint32 stride = sizeof(Vector3) + sizeof(Vector3);
    byte* vData = new byte[stride * positions.size()];
    byte* vDataBegin = vData;
    for (const auto& pos : positions)
    {
        *reinterpret_cast<Vector3*>(vData) = pos; // [a_vorontsov] Position.
        vData += sizeof(Vector3);
        *reinterpret_cast<Vector3*>(vData) = pos; // [a_vorontsov] Normal.
        vData += sizeof(Vector3);
    }

    uint32 iCount = static_cast<uint32>(faces.size()) * 3;
    uint16* indices = new uint16[iCount];
    byte* iDataBegin = reinterpret_cast<byte*>(indices);
    for (const auto& face : faces)
    {
        *indices++ = face.i1;
        *indices++ = face.i2;
        *indices++ = face.i3;
    }
    uint32 vCount = static_cast<uint32>(positions.size());
    return { vDataBegin, stride * vCount, stride, vCount, iDataBegin, iCount * sizeof(uint32), iCount, eIndexFormat::Format16Bit };
}

}
