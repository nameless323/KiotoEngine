//
// Copyright (C) Aleksandr Vorontcov. 2017
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//
// Adapted from http://wiki.unity3d.com/index.php/ProceduralPrimitives

#include "stdafx.h"

#include "Render/Geometry/GeometryGenerator.h"

#include <vector>
#include <map>

#include "Render/Geometry/Mesh.h"
#include "Render/VertexLayout.h"
#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"

namespace Kioto::Renderer::GeometryGenerator
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

// [a_vorontcov] Return index of point in the middle of p1 and p2 (for the icosphere).
template <typename T, typename = std::enable_if_t<std::is_same_v<T, byte> || std::is_same_v<T, uint16> || std::is_same_v<T, uint32> || std::is_same_v<T, uint64>>>
T GetMiddlePoint(T p1, T p2, std::vector<Vector3>& vertices, std::map<uint64, T>& cache, float radius)
{
    // [a_vorontcov] First check if we have it already.
    bool firstIsSmaller = p1 < p2;
    uint64 smallerIndex = firstIsSmaller ? p1 : p2;
    uint64 greaterIndex = firstIsSmaller ? p2 : p1;
    uint64 key = (smallerIndex << 32) + greaterIndex;

    auto it = cache.find(key);
    if (it != cache.end())
    {
        return it->second;
    }

    // [a_vorontcov] Not in cache, calculate it.
    Vector3 point1 = vertices[p1];
    Vector3 point2 = vertices[p2];
    Vector3 middle
    (
        (point1.x + point2.x) / 2.0f,
        (point1.y + point2.y) / 2.0f,
        (point1.z + point2.z) / 2.0f
    );

    // [a_vorontcov] Add vertex makes sure point is on unit sphere.
    T i = static_cast<T>(vertices.size());
    vertices.push_back(middle.Normalized() * radius);

    // [a_vorontcov] Store it, return index.
    cache[key] = i;
    return i;
}

Mesh* m_plane = nullptr;
Mesh* m_unitCube = nullptr;
Mesh* m_tube = nullptr;
Mesh* m_cone = nullptr;
Mesh* m_unitSphere = nullptr;
Mesh* m_unitIcosphere = nullptr;
}

void Init()
{
    m_plane = new Mesh(GeneratePlane());
    m_cone = new Mesh(GenerateCone());
    m_unitCube = new Mesh(GenerateCube());
    m_unitSphere = new Mesh(GenerateSphere());
    m_tube = new Mesh(GenerateTube());
    m_unitIcosphere = new Mesh(GenerateIcosphere());
}

void Shutdown()
{
    SafeDelete(m_plane);
    SafeDelete(m_cone);
    SafeDelete(m_unitCube);
    SafeDelete(m_unitSphere);
    SafeDelete(m_unitIcosphere);
}

Mesh GeometryGenerator::GeneratePlane(float32 sizeX /*= 1.0f*/, float32 sizeZ /*= 1.0f*/)
{
    uint32 resX = 2; // [a_vorontcov] 2 minimum.
    uint32 resZ = 2;
    uint32 vCount = resX * resZ;

    uint32 nbFaces = (resX - 1) * (resZ - 1);
    uint16 iCount = nbFaces * 6;

    Mesh mesh(Renderer::VertexLayout::LayoutPos3Norm3Uv2, vCount, iCount);

    uint32 index = 0;
    for (uint32 z = 0; z < resZ; z++)
    {
        // [a_vorontcov] [ -length / 2, length / 2 ]
        float32 zPos = (static_cast<float32>(z) / (resZ - 1) - .5f) * sizeX;
        for (uint32 x = 0; x < resX; x++)
        {
            // [a_vorontcov] [ -width / 2, width / 2 ]
            float32 xPos = (static_cast<float32>(x) / (resX - 1) - .5f) * sizeZ;
            *mesh.GetPositionPtr(index++) = Vector3(xPos, 0.0f, zPos);
        }
    }
    index = 0;

    for (uint32 n = 0; n < vCount; n++)
        *mesh.GetNormalPtr(index++) = Vector3(0.0f, 1.0f, 0.0f);
    index = 0;

    for (uint32 v = 0; v < resZ; v++)
    {
        for (uint32 u = 0; u < resX; u++)
            *mesh.GetUv0Ptr(index++) = Vector2(static_cast<float32>(u) / (resX - 1), static_cast<float32>(v) / (resZ - 1));
    }
    index = 0;

    for (uint16 face = 0; face < nbFaces; face++)
    {
        // [a_vorontcov] Retrieve lower left corner from face ind.
        uint32 i = face % (resX - 1) + (face / (resZ - 1) * resX);

        *mesh.GetIndexPtr(index++) = i + resX;
        *mesh.GetIndexPtr(index++) = i + 1;
        *mesh.GetIndexPtr(index++) = i;

        *mesh.GetIndexPtr(index++) = i + resX;
        *mesh.GetIndexPtr(index++) = i + resX + 1;
        *mesh.GetIndexPtr(index++) = i + 1;
    }

    return mesh;
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

    Mesh res(Renderer::VertexLayout::LayoutPos3Norm3Uv2, 24, 36);
    uint32 ind = 0;

    // [a_vorontcov] Bottom.
    *res.GetPositionPtr(ind++) = p0;
    *res.GetPositionPtr(ind++) = p1;
    *res.GetPositionPtr(ind++) = p2;
    *res.GetPositionPtr(ind++) = p3;
    // [a_vorontcov] Left.
    *res.GetPositionPtr(ind++) = p7;
    *res.GetPositionPtr(ind++) = p4;
    *res.GetPositionPtr(ind++) = p0;
    *res.GetPositionPtr(ind++) = p3;
    // [a_vorontcov] Front.
    *res.GetPositionPtr(ind++) = p4;
    *res.GetPositionPtr(ind++) = p5;
    *res.GetPositionPtr(ind++) = p1;
    *res.GetPositionPtr(ind++) = p0;
    // [a_vorontcov] Back.
    *res.GetPositionPtr(ind++) = p6;
    *res.GetPositionPtr(ind++) = p7;
    *res.GetPositionPtr(ind++) = p3;
    *res.GetPositionPtr(ind++) = p2;
    // [a_vorontcov] Right.
    *res.GetPositionPtr(ind++) = p5;
    *res.GetPositionPtr(ind++) = p6;
    *res.GetPositionPtr(ind++) = p2;
    *res.GetPositionPtr(ind++) = p1;
    // [a_vorontcov] Top.
    *res.GetPositionPtr(ind++) = p7;
    *res.GetPositionPtr(ind++) = p6;
    *res.GetPositionPtr(ind++) = p5;
    *res.GetPositionPtr(ind++) = p4;
    ind = 0;

    Vector3 up(0.0f, 1.0f, 0.0f);
    Vector3 down(0.0f, -1.0f, 0.0f);
    Vector3 front(0.0f, 0.0f, 1.0f);
    Vector3 back(0.0f, 0.0f, -1.0f);
    Vector3 left(1.0f, 0.0f, 0.0f);
    Vector3 right(-1.0f, 0.0f, 0.0f);

    // [a_vorontcov] Normals. Bottom.
    *res.GetNormalPtr(ind++) = down;
    *res.GetNormalPtr(ind++) = down;
    *res.GetNormalPtr(ind++) = down;
    *res.GetNormalPtr(ind++) = down;
    // [a_vorontcov] Left.
    *res.GetNormalPtr(ind++) = left;
    *res.GetNormalPtr(ind++) = left;
    *res.GetNormalPtr(ind++) = left;
    *res.GetNormalPtr(ind++) = left;
    // [a_vorontcov] Front.
    *res.GetNormalPtr(ind++) = front;
    *res.GetNormalPtr(ind++) = front;
    *res.GetNormalPtr(ind++) = front;
    *res.GetNormalPtr(ind++) = front;
    // [a_vorontcov] Back.
    *res.GetNormalPtr(ind++) = back;
    *res.GetNormalPtr(ind++) = back;
    *res.GetNormalPtr(ind++) = back;
    *res.GetNormalPtr(ind++) = back;
    // [a_vorontcov] Right.
    *res.GetNormalPtr(ind++) = right;
    *res.GetNormalPtr(ind++) = right;
    *res.GetNormalPtr(ind++) = right;
    *res.GetNormalPtr(ind++) = right;
    // [a_vorontcov] Top.
    *res.GetNormalPtr(ind++) = up;
    *res.GetNormalPtr(ind++) = up;
    *res.GetNormalPtr(ind++) = up;
    *res.GetNormalPtr(ind++) = up;
    ind = 0;

    Vector2 _00(0.0f, 0.0f);
    Vector2 _10(1.0f, 0.0f);
    Vector2 _01(0.0f, 1.0f);
    Vector2 _11(1.0f, 1.0f);

    // [a_vorontcov] UV. Bottom.
    *res.GetUv0Ptr(ind++) = _11;
    *res.GetUv0Ptr(ind++) = _01;
    *res.GetUv0Ptr(ind++) = _00;
    *res.GetUv0Ptr(ind++) = _10;
    // [a_vorontcov] Left.
    *res.GetUv0Ptr(ind++) = _11;
    *res.GetUv0Ptr(ind++) = _01;
    *res.GetUv0Ptr(ind++) = _00;
    *res.GetUv0Ptr(ind++) = _10;
    // [a_vorontcov] Front.
    *res.GetUv0Ptr(ind++) = _11;
    *res.GetUv0Ptr(ind++) = _01;
    *res.GetUv0Ptr(ind++) = _00;
    *res.GetUv0Ptr(ind++) = _10;
    // [a_vorontcov] Back.
    *res.GetUv0Ptr(ind++) = _11;
    *res.GetUv0Ptr(ind++) = _01;
    *res.GetUv0Ptr(ind++) = _00;
    *res.GetUv0Ptr(ind++) = _10;
    // [a_vorontcov] Right.
    *res.GetUv0Ptr(ind++) = _11;
    *res.GetUv0Ptr(ind++) = _01;
    *res.GetUv0Ptr(ind++) = _00;
    *res.GetUv0Ptr(ind++) = _10;
    // [a_vorontcov] Top.
    *res.GetUv0Ptr(ind++) = _11;
    *res.GetUv0Ptr(ind++) = _01;
    *res.GetUv0Ptr(ind++) = _00;
    *res.GetUv0Ptr(ind++) = _10;
    ind = 0;

    // [a_vorontcov] Bottom.
    *res.GetIndexPtr(ind++) = 3;
    *res.GetIndexPtr(ind++) = 1;
    *res.GetIndexPtr(ind++) = 0;

    *res.GetIndexPtr(ind++) = 3;
    *res.GetIndexPtr(ind++) = 2;
    *res.GetIndexPtr(ind++) = 1;

    // [a_vorontcov] Left.
    *res.GetIndexPtr(ind++) = 7;
    *res.GetIndexPtr(ind++) = 5;
    *res.GetIndexPtr(ind++) = 4;

    *res.GetIndexPtr(ind++) = 7;
    *res.GetIndexPtr(ind++) = 6;
    *res.GetIndexPtr(ind++) = 5;

    // [a_vorontcov] Front.
    *res.GetIndexPtr(ind++) = 11;
    *res.GetIndexPtr(ind++) = 9;
    *res.GetIndexPtr(ind++) = 8;

    *res.GetIndexPtr(ind++) = 11;
    *res.GetIndexPtr(ind++) = 10;
    *res.GetIndexPtr(ind++) = 9;

    // [a_vorontcov] Back.
    *res.GetIndexPtr(ind++) = 15;
    *res.GetIndexPtr(ind++) = 13;
    *res.GetIndexPtr(ind++) = 12;

    *res.GetIndexPtr(ind++) = 15;
    *res.GetIndexPtr(ind++) = 14;
    *res.GetIndexPtr(ind++) = 13;

    // [a_vorontcov] Right.
    *res.GetIndexPtr(ind++) = 19;
    *res.GetIndexPtr(ind++) = 17;
    *res.GetIndexPtr(ind++) = 16;

    *res.GetIndexPtr(ind++) = 19;
    *res.GetIndexPtr(ind++) = 18;
    *res.GetIndexPtr(ind++) = 17;

    // [a_vorontcov] Top.
    *res.GetIndexPtr(ind++) = 23;
    *res.GetIndexPtr(ind++) = 21;
    *res.GetIndexPtr(ind++) = 20;

    *res.GetIndexPtr(ind++) = 23;
    *res.GetIndexPtr(ind++) = 22;
    *res.GetIndexPtr(ind++) = 21;

    return res;
}

Mesh GenerateCone(float32 height /*= 1.0f*/, float32 bottomRadius /*= 0.25f*/, float32 topRadius /*= 0.05f*/)
{
    uint32 nbSides = 18;
    uint32 nbHeightSeg = 1; // [a_vorontcov] Not implemented yet.

    uint32 nbVerticesCap = nbSides + 1;

    uint32 vCount = nbVerticesCap + nbVerticesCap + nbSides * nbHeightSeg * 2 + 2;

    uint32 nbTriangles = nbSides + nbSides + nbSides * 2;
    uint32 iCount = nbTriangles * 3 + 3;

    Mesh mesh(Renderer::VertexLayout::LayoutPos3Norm3Uv2, vCount, iCount);

    uint32 index = 0;


    // [a_vorontcov] Bottom cap.
    *mesh.GetPositionPtr(index++) = Vector3(0.0f, 0.0f, 0.0f);

    while (index <= nbSides)
    {
        float32 rad = static_cast<float32>(index) / nbSides * Math::TwoPI;
        *mesh.GetPositionPtr(index++) = Vector3(std::cos(rad) * bottomRadius, 0.0f, std::sin(rad) * bottomRadius);
    }

    // [a_vorontcov] Top cap.
    *mesh.GetPositionPtr(index++) = Vector3(0.0f, height, 0.0f);
    while (index <= nbSides * 2 + 1)
    {
        float32 rad = static_cast<float32>(index - nbSides - 1) / nbSides * Math::TwoPI;
        *mesh.GetPositionPtr(index++) = Vector3(std::cos(rad) * topRadius, height, std::sin(rad) * topRadius);
    }

    // [a_vorontcov] Sides.
    int32 v = 0;
    while (index <= vCount - 4)
    {
        float32 rad = static_cast<float32>(v) / nbSides * Math::TwoPI;
        *mesh.GetPositionPtr(index++) = Vector3(std::cos(rad) * topRadius, height, std::sin(rad) * topRadius);
        *mesh.GetPositionPtr(index++) = Vector3(std::cos(rad) * bottomRadius, 0.0f, std::sin(rad) * bottomRadius);
        v++;
    }
    *mesh.GetPositionPtr(index++) = *mesh.GetPositionPtr(nbSides * 2 + 2);
    *mesh.GetPositionPtr(index++) = *mesh.GetPositionPtr(nbSides * 2 + 3);

    index = 0;

    // [a_vorontcov] Bottom cap.
    while (index <= nbSides)
        *mesh.GetNormalPtr(index++) = Vector3(0.0f, -1.0f, 0.0f);

    // [a_vorontcov] Top cap.
    while (index <= nbSides * 2 + 1)
        *mesh.GetNormalPtr(index++) = Vector3(0.0f, 1.0f, 0.0f);

    // [a_vorontcov] Sides.
    v = 0;
    while (index <= vCount - 4)
    {
        float32 rad = static_cast<float32>(v) / nbSides * Math::TwoPI;
        float32 cos = std::cos(rad);
        float32 sin = std::sin(rad);

        *mesh.GetNormalPtr(index++) = Vector3(cos, 0.0f, sin);
        *mesh.GetNormalPtr(index++) = Vector3(cos, 0.0f, sin);
        v++;
    }

    *mesh.GetNormalPtr(index++) = *mesh.GetNormalPtr(nbSides * 2 + 2);
    *mesh.GetNormalPtr(index++) = *mesh.GetNormalPtr(nbSides * 2 + 2);

    index = 0;
    // [a_vorontcov] Bottom cap.
    uint32 u = 0;
    *mesh.GetUv0Ptr(index++) = Vector2(0.5f, 0.5f);
    u++;
    while (u <= nbSides)
    {
        float32 rad = static_cast<float32>(u) / nbSides * Math::TwoPI;
        *mesh.GetUv0Ptr(index++) = Vector2(std::cos(rad) * 0.5f + 0.5f, std::sin(rad) * 0.5f + 0.5f);
        u++;
    }

    // [a_vorontcov] Top cap.
    *mesh.GetUv0Ptr(index++) = Vector2(0.5f, 0.5f);
    u++;
    while (u <= nbSides * 2 + 1)
    {
        float32 rad = static_cast<float32>(u) / nbSides * Math::TwoPI;
        *mesh.GetUv0Ptr(index++) = Vector2(std::cos(rad) * 0.5f + 0.5f, std::sin(rad) * 0.5f + 0.5f);
        u++;
    }

    // [a_vorontcov] Sides.
    int32 u_sides = 0;
    while (u <= vCount - 4)
    {
        float32 t = static_cast<float32>(u_sides) / nbSides;
        *mesh.GetUv0Ptr(index++) = Vector2(t, 1.0f);
        *mesh.GetUv0Ptr(index++) = Vector2(t, 0.0f);
        u += 2;
        u_sides++;
    }
    *mesh.GetUv0Ptr(index++) = Vector2(1.0f, 1.0f);
    *mesh.GetUv0Ptr(index++) = Vector2(1.0f, 0.0f);

    index = 0;

    // [a_vorontcov] Bottom cap.
    uint32 tri = 0;
    while (tri < nbSides - 1)
    {
        *mesh.GetIndexPtr(index++) = 0;
        *mesh.GetIndexPtr(index++) = tri + 1;
        *mesh.GetIndexPtr(index++) = tri + 2;
        tri++;
    }
    *mesh.GetIndexPtr(index++) = 0;
    *mesh.GetIndexPtr(index++) = tri + 1;
    *mesh.GetIndexPtr(index++) = 1;
    tri++;

    // [a_vorontcov] Top cap.
    while (tri < nbSides * 2)
    {
        *mesh.GetIndexPtr(index++) = tri + 2;
        *mesh.GetIndexPtr(index++) = tri + 1;
        *mesh.GetIndexPtr(index++) = nbVerticesCap;
        tri++;
    }

    *mesh.GetIndexPtr(index++) = nbVerticesCap + 1;
    *mesh.GetIndexPtr(index++) = tri + 1;
    *mesh.GetIndexPtr(index++) = nbVerticesCap;
    tri += 2;

    // [a_vorontcov] Sides.
    while (tri <= nbTriangles)
    {
        *mesh.GetIndexPtr(index++) = tri + 2;
        *mesh.GetIndexPtr(index++) = tri + 1;
        *mesh.GetIndexPtr(index++) = tri + 0;
        tri++;

        *mesh.GetIndexPtr(index++) = tri + 1;
        *mesh.GetIndexPtr(index++) = tri + 2;
        *mesh.GetIndexPtr(index++) = tri + 0;
        tri++;
    }
    return mesh;
}

Mesh GenerateSphere(float32 radius /*=1.0f*/)
{
    int32 nbLong = 24;
    int32 nbLat = 16;

    uint32 vCount = (nbLong + 1) * nbLat + 2;

    uint32 nbFaces = vCount;
    uint32 nbTriangles = nbFaces * 2;
    uint32 iCount = nbTriangles * 3;

    Mesh mesh(Renderer::VertexLayout::LayoutPos3Norm3Uv2, vCount, iCount);

    uint32 index = 0;
    *mesh.GetPositionPtr(index++) = Vector3(0.0f, 1.0f, 0.0) * radius;

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

            *mesh.GetPositionPtr(index++) = Vector3(sin1 * cos2, cos1, sin1 * sin2) * radius;
        }
    }
    *mesh.GetPositionPtr(index++) = Vector3(0.0f, -1.0f, 0.0f) * radius;
    index = 0;

    for (uint32 n = 0; n < vCount; n++)
    {
        *mesh.GetNormalPtr(index) = *mesh.GetPositionPtr(index);
        ++index;
    }
    index = 0;

    *mesh.GetUv0Ptr(index++) = Vector2(0.0f, 1.0f);

    for (int lat = 0; lat < nbLat; lat++)
    {
        for (int lon = 0; lon <= nbLong; lon++)
            *mesh.GetUv0Ptr(index++) = Vector2(static_cast<float32>(lon) / nbLong, 1.0f - static_cast<float32>(lat + 1) / (nbLat + 1));
    }
    *mesh.GetUv0Ptr(index++) = Vector2(0.0f, 0.0f);

    index = 0;

    // [a_vorontcov] Top cap.
    for (int32 lon = 0; lon < nbLong; lon++)
    {
        *mesh.GetIndexPtr(index++) = lon + 2;
        *mesh.GetIndexPtr(index++) = lon + 1;
        *mesh.GetIndexPtr(index++) = 0;
    }

    // [a_vorontcov] Middle.
    for (int32 lat = 0; lat < nbLat - 1; lat++)
    {
        for (int32 lon = 0; lon < nbLong; lon++)
        {
            int32 current = lon + lat * (nbLong + 1) + 1;
            int32 next = current + nbLong + 1;

            *mesh.GetIndexPtr(index++) = current;
            *mesh.GetIndexPtr(index++) = current + 1;
            *mesh.GetIndexPtr(index++) = next + 1;

            *mesh.GetIndexPtr(index++) = current;
            *mesh.GetIndexPtr(index++) = next + 1;
            *mesh.GetIndexPtr(index++) = next;
        }
    }

    // [a_vorontcov] Bottom cap.
    for (int32 lon = 0; lon < nbLong; lon++)
    {
        *mesh.GetIndexPtr(index++)  = vCount - 1;
        *mesh.GetIndexPtr(index++)  = vCount - (lon + 2) - 1;
        *mesh.GetIndexPtr(index++)  = vCount - (lon + 1) - 1;
    }
    return mesh;
}

Mesh GenerateTube(float32 height /*= 1.0f*/, float32 bottomRadius1 /*= 0.5f*/, float32 bottomRadius2 /*= 0.15f*/, float32 topRadius1 /*= 0.5f*/, float32 topRadius2 /*= 0.15f*/)
{
    int32 nbSides = 24;

    // [a_vorontcov] Outter shell is at radius1 + radius2 / 2, inner shell at radius1 - radius2 / 2.
    uint32 nbVerticesCap = nbSides * 2 + 2;
    uint32 nbVerticesSides = nbSides * 2 + 2;
    uint32 vCount = nbVerticesCap * 2 + nbVerticesSides * 2;

    int32 nbFace = nbSides * 4;
    int32 nbTriangles = nbFace * 2;
    uint32 iCount = nbTriangles * 3;

    Mesh mesh(Renderer::VertexLayout::LayoutPos3Norm3Uv2, vCount, iCount);

    uint32 index = 0;

    // [a_vorontcov] Bottom cap.
    int32 sideCounter = 0;
    while (index < nbVerticesCap)
    {
        sideCounter = sideCounter == nbSides ? 0 : sideCounter;

        float32 r1 = static_cast<float32>(sideCounter++) / nbSides * Math::TwoPI;
        float32 cos = std::cos(r1);
        float32 sin = std::sin(r1);
        *mesh.GetPositionPtr(index++) = Vector3(cos * (bottomRadius1 - bottomRadius2 * 0.5f), 0.0f, sin * (bottomRadius1 - bottomRadius2 * 0.5f));
        *mesh.GetPositionPtr(index++) = Vector3(cos * (bottomRadius1 + bottomRadius2 * 0.5f), 0.0f, sin * (bottomRadius1 + bottomRadius2 * 0.5f));
    }

    // [a_vorontcov] Top cap.
    sideCounter = 0;
    while (index < nbVerticesCap * 2)
    {
        sideCounter = sideCounter == nbSides ? 0 : sideCounter;

        float32 r1 = static_cast<float32>(sideCounter++) / nbSides * Math::TwoPI;
        float32 cos = std::cos(r1);
        float32 sin = std::sin(r1);
        *mesh.GetPositionPtr(index++) = Vector3(cos * (topRadius1 - topRadius2 * 0.5f), height, sin * (topRadius1 - topRadius2 * 0.5f));
        *mesh.GetPositionPtr(index++) = Vector3(cos * (topRadius1 + topRadius2 * 0.5f), height, sin * (topRadius1 + topRadius2 * 0.5f));
    }

    // [a_vorontcov] Sides (out).
    sideCounter = 0;
    while (index < nbVerticesCap * 2 + nbVerticesSides)
    {
        sideCounter = sideCounter == nbSides ? 0 : sideCounter;

        float32 r1 = static_cast<float>(sideCounter++) / nbSides * Math::TwoPI;
        float32 cos = std::cos(r1);
        float32 sin = std::sin(r1);

        *mesh.GetPositionPtr(index++) = Vector3(cos * (topRadius1 + topRadius2 * .5f), height, sin * (topRadius1 + topRadius2 * 0.5f));
        *mesh.GetPositionPtr(index++) = Vector3(cos * (bottomRadius1 + bottomRadius2 * 0.5f), 0.0f, sin * (bottomRadius1 + bottomRadius2 * 0.5f));
    }

    // [a_vorontcov] Sides (in).
    sideCounter = 0;
    while (index < vCount)
    {
        sideCounter = sideCounter == nbSides ? 0 : sideCounter;

        float32 r1 = static_cast<float32>(sideCounter++) / nbSides * Math::TwoPI;
        float32 cos = std::cos(r1);
        float32 sin = std::sin(r1);

        *mesh.GetPositionPtr(index++) = Vector3(cos * (topRadius1 - topRadius2 * 0.5f), height, sin * (topRadius1 - topRadius2 * 0.5f));
        *mesh.GetPositionPtr(index++) = Vector3(cos * (bottomRadius1 - bottomRadius2 * 0.5f), 0.0f, sin * (bottomRadius1 - bottomRadius2 * 0.5f));
    }

    index = 0;
    // [a_vorontcov] Bottom cap.
    while (index < nbVerticesCap)
        *mesh.GetNormalPtr(index++) = Vector3(0.0f, -1.0f, 0.0f);

    // [a_vorontcov] Top cap.
    while (index < nbVerticesCap * 2)
        *mesh.GetNormalPtr(index++) = Vector3(0.0f, 1.0f, 0.0f);

    // [a_vorontcov] Sides (out).
    sideCounter = 0;
    while (index < nbVerticesCap * 2 + nbVerticesSides)
    {
        sideCounter = sideCounter == nbSides ? 0 : sideCounter;

        float32 r1 = static_cast<float32>(sideCounter++) / nbSides * Math::TwoPI;

        *mesh.GetNormalPtr(index++) = Vector3(std::cos(r1), 0.0f, std::sin(r1));
        *mesh.GetNormalPtr(index++) = Vector3(std::cos(r1), 0.0f, std::sin(r1));
    }

    // [a_vorontcov] Sides (in).
    sideCounter = 0;
    while (index < vCount)
    {
        sideCounter = sideCounter == nbSides ? 0 : sideCounter;

        float32 r1 = static_cast<float32>(sideCounter++) / nbSides * Math::TwoPI;

        *mesh.GetNormalPtr(index++) = Vector3(-std::cos(r1), 0.0f, -std::sin(r1));
        *mesh.GetNormalPtr(index++) = Vector3(-std::cos(r1), 0.0f, -std::sin(r1));
    }
    index = 0;

    // [a_vorontcov] Bottom cap.
    sideCounter = 0;
    while (index < nbVerticesCap)
    {
        float32 t = static_cast<float32>(sideCounter++) / nbSides;
        *mesh.GetUv0Ptr(index++) = Vector2(0.0f, t);
        *mesh.GetUv0Ptr(index++) = Vector2(1.0f, t);
    }

    // [a_vorontcov] Top cap.
    sideCounter = 0;
    while (index < nbVerticesCap * 2)
    {
        float32 t = static_cast<float32>(sideCounter++) / nbSides;
        *mesh.GetUv0Ptr(index++) = Vector2(0.0f, t);
        *mesh.GetUv0Ptr(index++) = Vector2(1.0f, t);
    }

    // [a_vorontcov] Sides (out).
    sideCounter = 0;
    while (index < nbVerticesCap * 2 + nbVerticesSides)
    {
        float32 t = static_cast<float32>(sideCounter++) / nbSides;
        *mesh.GetUv0Ptr(index++) = Vector2(0.0f, t);
        *mesh.GetUv0Ptr(index++) = Vector2(1.0f, t);
    }

    // [a_vorontcov] Sides (in).
    sideCounter = 0;
    while (index < vCount)
    {
        float32 t = static_cast<float>(sideCounter++) / nbSides;
        *mesh.GetUv0Ptr(index++) = Vector2(0.0f, t);
        *mesh.GetUv0Ptr(index++) = Vector2(1.0f, t);
    }
    index = 0;

    // [a_vorontcov] Bottom cap.
    sideCounter = 0;
    while (sideCounter < nbSides)
    {
        int16 current = sideCounter * 2;
        int16 next = sideCounter * 2 + 2;

        *mesh.GetIndexPtr(index++) = next + 1;
        *mesh.GetIndexPtr(index++) = next;
        *mesh.GetIndexPtr(index++) = current;

        *mesh.GetIndexPtr(index++) = current + 1;
        *mesh.GetIndexPtr(index++) = next + 1;
        *mesh.GetIndexPtr(index++) = current;

        sideCounter++;
    }

    // [a_vorontcov] Top cap.
    while (sideCounter < nbSides * 2)
    {
        int16 current = sideCounter * 2 + 2;
        int16 next = sideCounter * 2 + 4;

        *mesh.GetIndexPtr(index++) = current;
        *mesh.GetIndexPtr(index++) = next;
        *mesh.GetIndexPtr(index++) = next + 1;

        *mesh.GetIndexPtr(index++) = current;
        *mesh.GetIndexPtr(index++) = next + 1;
        *mesh.GetIndexPtr(index++) = current + 1;

        sideCounter++;
    }

    // [a_vorontcov] Sides (out).
    while (sideCounter < nbSides * 3)
    {
        int16 current = sideCounter * 2 + 4;
        int16 next = sideCounter * 2 + 6;

        *mesh.GetIndexPtr(index++) = current;
        *mesh.GetIndexPtr(index++) = next;
        *mesh.GetIndexPtr(index++) = next + 1;

        *mesh.GetIndexPtr(index++) = current;
        *mesh.GetIndexPtr(index++) = next + 1;
        *mesh.GetIndexPtr(index++) = current + 1;

        sideCounter++;
    }


    // [a_vorontcov] Sides (in).
    while (sideCounter < nbSides * 4)
    {
        int16 current = sideCounter * 2 + 6;
        int16 next = sideCounter * 2 + 8;

        *mesh.GetIndexPtr(index++) = next + 1;
        *mesh.GetIndexPtr(index++) = next;
        *mesh.GetIndexPtr(index++) = current;

        *mesh.GetIndexPtr(index++) = current + 1;
        *mesh.GetIndexPtr(index++) = next + 1;
        *mesh.GetIndexPtr(index++) = current;

        sideCounter++;
    }
    return mesh;
}

Mesh GenerateIcosphere(int32 recursionLevel /*= 3*/, float32 radius /*= 1.0f*/)
{
    std::vector<Vector3> positions;
    std::map<uint64, uint16> middlePointIndexCache;

    // [a_vorontcov] Create 12 vertices of a icosahedron.
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

    // [a_vorontcov] Create 20 triangles of the icosahedron.
    std::vector<TriangleIndices<uint16>> faces;

    // [a_vorontcov] 5 faces around point 0.
    faces.emplace_back(0, 11, 5);
    faces.emplace_back(0, 5, 1);
    faces.emplace_back(0, 1, 7);
    faces.emplace_back(0, 7, 10);
    faces.emplace_back(0, 10, 11);

    // [a_vorontcov] 5 adjacent faces.
    faces.emplace_back(1, 5, 9);
    faces.emplace_back(5, 11, 4);
    faces.emplace_back(11, 10, 2);
    faces.emplace_back(10, 7, 6);
    faces.emplace_back(7, 1, 8);

    // [a_vorontcov] 5 faces around point 3.
    faces.emplace_back(3, 9, 4);
    faces.emplace_back(3, 4, 2);
    faces.emplace_back(3, 2, 6);
    faces.emplace_back(3, 6, 8);
    faces.emplace_back(3, 8, 9);

    // [a_vorontcov] 5 adjacent faces.
    faces.emplace_back(4, 9, 5);
    faces.emplace_back(2, 4, 11);
    faces.emplace_back(6, 2, 10);
    faces.emplace_back(8, 6, 7);
    faces.emplace_back(9, 8, 1);

    // [a_vorontcov] Refine triangles.
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
    uint32 vCount = static_cast<uint32>(positions.size());
    uint32 iCount = static_cast<uint32>(faces.size()) * 3;

    Mesh mesh(Renderer::VertexLayout::LayoutPos3Norm3, vCount, iCount);
    uint32 index = 0;
    for (const auto& pos : positions)
    {
        *mesh.GetPositionPtr(index) = pos; // [a_vorontcov] Position.
        *mesh.GetNormalPtr(index) = pos; // [a_vorontcov] Normal.
        ++index;
    }
    index = 0;

    for (const auto& face : faces)
    {
        *mesh.GetIndexPtr(index++) = face.i1;
        *mesh.GetIndexPtr(index++) = face.i2;
        *mesh.GetIndexPtr(index++) = face.i3;
    }
    return mesh;
}

Mesh* GetUnitCube()
{
    return m_unitCube;
}

Mesh* GetUnitSphere()
{
    return m_unitSphere;
}

Mesh* GetUnitIcosphere()
{
    return m_unitIcosphere;
}

Mesh* GetPlane()
{
    return m_plane;
}

Mesh* GetCone()
{
    return m_cone;
}

Mesh* GetTube()
{
    return m_tube;
}
}
