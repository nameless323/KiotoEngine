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
struct TriangleIndices
{
    int v1 = 0;
    int v2 = 0;
    int v3 = 0;

    TriangleIndices(int v1_, int v2_, int v3_)
    {
        v1 = v1_;
        v2 = v2_;
        v3 = v3_;
    }
};

// return index of point in the middle of p1 and p2
int GetMiddlePoint(int p1, int p2, std::vector<Vector3>& vertices, std::map<uint64, int>& cache, float radius)
{
    // first check if we have it already
    bool firstIsSmaller = p1 < p2;
    uint64 smallerIndex = firstIsSmaller ? p1 : p2;
    uint64 greaterIndex = firstIsSmaller ? p2 : p1;
    uint64 key = (smallerIndex << 32) + greaterIndex;

    auto it = cache.find(key);
    if (it != cache.end())
    {
        return it->second;
    }

    // not in cache, calculate it
    Vector3 point1 = vertices[p1];
    Vector3 point2 = vertices[p2];
    Vector3 middle
    (
        (point1.x + point2.x) / 2.0f,
        (point1.y + point2.y) / 2.0f,
        (point1.z + point2.z) / 2.0f
    );

    // add vertex makes sure point is on unit sphere
    int i = static_cast<int>(vertices.size());
    vertices.push_back(middle.Normalized() * radius);

    // store it, return index
    cache[key] = i;
    return i;
}
}

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

Mesh GenerateSphere()
{
    Mesh m;
    float radius = 1.0f;
    // Longitude |||
    int nbLong = 24;
    // Latitude ---
    int nbLat = 16;

    int vertSize = (nbLong + 1) * nbLat + 2;

    m.Position.reserve(vertSize);
    m.Position.push_back(Vector3(0.0f, 1.0f, 0.0) * radius);
    for (int lat = 0; lat < nbLat; lat++)
    {
        float a1 = Math::PI * (float)(lat + 1) / (nbLat + 1);
        float sin1 = std::sin(a1);
        float cos1 = std::cos(a1);

        for (int lon = 0; lon <= nbLong; lon++)
        {
            float a2 = Math::TwoPI * (float)(lon == nbLong ? 0 : lon) / nbLong;
            float sin2 = std::sin(a2);
            float cos2 = std::cos(a2);

            m.Position.push_back(Vector3(sin1 * cos2, cos1, sin1 * sin2) * radius);
        }
    }
    m.Position.push_back(Vector3(0.0f, 1.0f, 0.0f) * -radius);

    m.Normal.reserve(vertSize);
    for (int n = 0; n < vertSize; n++)
        m.Normal.push_back(Vector3::Normalized(m.Position[n]));

    m.UV0.reserve(vertSize);
    m.UV0.emplace_back(0.0f, 1.0f);
    for (int lat = 0; lat < nbLat; lat++)
        for (int lon = 0; lon <= nbLong; lon++)
            m.UV0.emplace_back((float)lon / nbLong, 1.0f - (float)(lat + 1) / (nbLat + 1));
    m.UV0.emplace_back(0.0f, 0.0f);

    int nbFaces = vertSize;
    int nbTriangles = nbFaces * 2;
    int nbIndexes = nbTriangles * 3;
    m.Triangles.reserve(nbIndexes);

    //Top Cap
    int i = 0;
    for (int lon = 0; lon < nbLong; lon++)
    {
        m.Triangles.push_back(lon + 2);
        m.Triangles.push_back(lon + 1);
        m.Triangles.push_back(0);
    }

    //Middle
    for (int lat = 0; lat < nbLat - 1; lat++)
    {
        for (int lon = 0; lon < nbLong; lon++)
        {
            int current = lon + lat * (nbLong + 1) + 1;
            int next = current + nbLong + 1;

            m.Triangles.push_back(current);
            m.Triangles.push_back(current + 1);
            m.Triangles.push_back(next + 1);

            m.Triangles.push_back(current);
            m.Triangles.push_back(next + 1);
            m.Triangles.push_back(next);
        }
    }

    //Bottom Cap
    for (int lon = 0; lon < nbLong; lon++)
    {
        m.Triangles.push_back(vertSize - 1);
        m.Triangles.push_back(vertSize - (lon + 2) - 1);
        m.Triangles.push_back(vertSize - (lon + 1) - 1);
    }
    m.PrepareForUpload();
    return m;
}

Mesh GenerateTube()
{
    float height = 1.0f;
    int nbSides = 24;

    // Outter shell is at radius1 + radius2 / 2, inner shell at radius1 - radius2 / 2
    float bottomRadius1 = .5f;
    float bottomRadius2 = .15f;
    float topRadius1 = .5f;
    float topRadius2 = .15f;

    int nbVerticesCap = nbSides * 2 + 2;
    int nbVerticesSides = nbSides * 2 + 2;
    int vertSize = nbVerticesCap * 2 + nbVerticesSides * 2;

    Mesh m;
    int vert = 0;

    // Bottom cap
    int sideCounter = 0;
    while (vert < nbVerticesCap)
    {
        sideCounter = sideCounter == nbSides ? 0 : sideCounter;

        float r1 = (float)(sideCounter++) / nbSides * Math::TwoPI;
        float cos = std::cos(r1);
        float sin = std::sin(r1);
        m.Position.emplace_back(cos * (bottomRadius1 - bottomRadius2 * .5f), 0.0f, sin * (bottomRadius1 - bottomRadius2 * 0.5f));
        m.Position.emplace_back(cos * (bottomRadius1 + bottomRadius2 * .5f), 0.0f, sin * (bottomRadius1 + bottomRadius2 * 0.5f));
        vert += 2;
    }

    // Top cap
    sideCounter = 0;
    while (vert < nbVerticesCap * 2)
    {
        sideCounter = sideCounter == nbSides ? 0 : sideCounter;

        float r1 = (float)(sideCounter++) / nbSides * Math::TwoPI;
        float cos = std::cos(r1);
        float sin = std::sin(r1);
        m.Position.emplace_back(cos * (topRadius1 - topRadius2 * 0.5f), height, sin * (topRadius1 - topRadius2 * 0.5f));
        m.Position.emplace_back(cos * (topRadius1 + topRadius2 * 0.5f), height, sin * (topRadius1 + topRadius2 * 0.5f));
        vert += 2;
    }

    // Sides (out)
    sideCounter = 0;
    while (vert < nbVerticesCap * 2 + nbVerticesSides)
    {
        sideCounter = sideCounter == nbSides ? 0 : sideCounter;

        float r1 = (float)(sideCounter++) / nbSides * Math::TwoPI;
        float cos = std::cos(r1);
        float sin = std::sin(r1);

        m.Position.emplace_back(cos * (topRadius1 + topRadius2 * .5f), height, sin * (topRadius1 + topRadius2 * .5f));
        m.Position.emplace_back(cos * (bottomRadius1 + bottomRadius2 * .5f), 0.0f, sin * (bottomRadius1 + bottomRadius2 * .5f));
        vert += 2;
    }

    // Sides (in)
    sideCounter = 0;
    while (vert < vertSize)
    {
        sideCounter = sideCounter == nbSides ? 0 : sideCounter;

        float r1 = (float)(sideCounter++) / nbSides * Math::TwoPI;
        float cos = std::cos(r1);
        float sin = std::sin(r1);

        m.Position.emplace_back(cos * (topRadius1 - topRadius2 * .5f), height, sin * (topRadius1 - topRadius2 * .5f));
        m.Position.emplace_back(cos * (bottomRadius1 - bottomRadius2 * .5f), 0.0f, sin * (bottomRadius1 - bottomRadius2 * .5f));
        vert += 2;
    }

    vert = 0;
    // Bottom cap
    while (vert < nbVerticesCap)
    {
        m.Normal.emplace_back(0.0f, -1.0f, 1.0f);
        vert++;
    }

    // Top cap
    while (vert < nbVerticesCap * 2)
    {
        m.Normal.emplace_back(0.0f, 1.0f, 1.0f);
        vert++;
    }

    // Sides (out)
    sideCounter = 0;
    while (vert < nbVerticesCap * 2 + nbVerticesSides)
    {
        sideCounter = sideCounter == nbSides ? 0 : sideCounter;

        float r1 = (float)(sideCounter++) / nbSides * Math::TwoPI;

        m.Normal.emplace_back(std::cos(r1), 0.0f, std::sin(r1));
        m.Normal.push_back(m.Normal[vert]);
        vert += 2;
    }

    // Sides (in)
    sideCounter = 0;
    while (vert < vertSize)
    {
        sideCounter = sideCounter == nbSides ? 0 : sideCounter;

        float r1 = (float)(sideCounter++) / nbSides * Math::TwoPI;

        m.Normal.push_back(Vector3(-std::cos(r1), 0.0f, -std::sin(r1)));
        m.Normal.push_back(m.Normal[vert]);
        vert += 2;
    }

    vert = 0;
    // Bottom cap
    sideCounter = 0;
    while (vert < nbVerticesCap)
    {
        float t = (float)(sideCounter++) / nbSides;
        m.UV0.emplace_back(0.0f, t);
        m.UV0.emplace_back(1.0f, t);
        vert += 2;
    }

    // Top cap
    sideCounter = 0;
    while (vert < nbVerticesCap * 2)
    {
        float t = (float)(sideCounter++) / nbSides;
        m.UV0.emplace_back(0.0f, t);
        m.UV0.emplace_back(1.0f, t);
        vert += 2;
    }

    // Sides (out)
    sideCounter = 0;
    while (vert < nbVerticesCap * 2 + nbVerticesSides)
    {
        float t = (float)(sideCounter++) / nbSides;
        m.UV0.emplace_back(0.0f, t);
        m.UV0.emplace_back(1.0f, t);
        vert += 2;
    }

    // Sides (in)
    sideCounter = 0;
    while (vert < vertSize)
    {
        float t = (float)(sideCounter++) / nbSides;
        m.UV0.emplace_back(0.0f, t);
        m.UV0.emplace_back(1.0f, t);
        vert += 2;
    }

        int nbFace = nbSides * 4;
    int nbTriangles = nbFace * 2;
    int nbIndexes = nbTriangles * 3;

    // Bottom cap
    int i = 0;
    sideCounter = 0;
    while (sideCounter < nbSides)
    {
        int current = sideCounter * 2;
        int next = sideCounter * 2 + 2;

        m.Triangles.push_back(next + 1);
        m.Triangles.push_back(next);
        m.Triangles.push_back(current);

        m.Triangles.push_back(current + 1);
        m.Triangles.push_back(next + 1);
        m.Triangles.push_back(current);

        sideCounter++;
    }

    // Top cap
    while (sideCounter < nbSides * 2)
    {
        int current = sideCounter * 2 + 2;
        int next = sideCounter * 2 + 4;

        m.Triangles.push_back(current);
        m.Triangles.push_back(next);
        m.Triangles.push_back(next + 1);

        m.Triangles.push_back(current);
        m.Triangles.push_back(next + 1);
        m.Triangles.push_back(current + 1);

        sideCounter++;
    }

    // Sides (out)
    while (sideCounter < nbSides * 3)
    {
        int current = sideCounter * 2 + 4;
        int next = sideCounter * 2 + 6;

        m.Triangles.push_back(current);
        m.Triangles.push_back(next);
        m.Triangles.push_back(next + 1);

        m.Triangles.push_back(current);
        m.Triangles.push_back(next + 1);
        m.Triangles.push_back(current + 1);

        sideCounter++;
    }


    // Sides (in)
    while (sideCounter < nbSides * 4)
    {
        int current = sideCounter * 2 + 6;
        int next = sideCounter * 2 + 8;

        m.Triangles.push_back(next + 1);
        m.Triangles.push_back(next);
        m.Triangles.push_back(current);

        m.Triangles.push_back(current + 1);
        m.Triangles.push_back(next + 1);
        m.Triangles.push_back(current);

        sideCounter++;
    }
    m.PrepareForUpload();
    return m;
}

Mesh GenerateIcosphere()
{
    std::vector<Vector3> vertList;
    std::map<uint64, int> middlePointIndexCache;
    int index = 0;

    int recursionLevel = 3;
    float radius = 1.0f;
    Mesh m;

    // create 12 vertices of a icosahedron
    float t = (1.0f + std::sqrt(5.0f)) / 2.0f;

    vertList.push_back(Vector3(-1.0f, t, 0.0f).Normalized() * radius);
    vertList.push_back(Vector3(1.0f, t, 0.0f).Normalized() * radius);
    vertList.push_back(Vector3(-1.0f, -t, 0.0f).Normalized() * radius);
    vertList.push_back(Vector3(1.0f, -t, 0.0f).Normalized() * radius);

    vertList.push_back(Vector3(0.0f, -1.0f, t).Normalized() * radius);
    vertList.push_back(Vector3(0.0f, 1.0f, t).Normalized() * radius);
    vertList.push_back(Vector3(0.0f, -1.0f, -t).Normalized() * radius);
    vertList.push_back(Vector3(0.0f, 1.0f, -t).Normalized() * radius);

    vertList.push_back(Vector3(t, 0.0f, -1.0f).Normalized() * radius);
    vertList.push_back(Vector3(t, 0.0f, 1.0f).Normalized() * radius);
    vertList.push_back(Vector3(-t, 0.0f, -1.0f).Normalized() * radius);
    vertList.push_back(Vector3(-t, 0.0f, 1.0f).Normalized() * radius);


    // create 20 triangles of the icosahedron
    std::vector<TriangleIndices> faces;

    // 5 faces around point 0
    faces.emplace_back(0, 11, 5);
    faces.emplace_back(0, 5, 1);
    faces.emplace_back(0, 1, 7);
    faces.emplace_back(0, 7, 10);
    faces.emplace_back(0, 10, 11);

    // 5 adjacent faces 
    faces.emplace_back(1, 5, 9);
    faces.emplace_back(5, 11, 4);
    faces.emplace_back(11, 10, 2);
    faces.emplace_back(10, 7, 6);
    faces.emplace_back(7, 1, 8);

    // 5 faces around point 3
    faces.emplace_back(3, 9, 4);
    faces.emplace_back(3, 4, 2);
    faces.emplace_back(3, 2, 6);
    faces.emplace_back(3, 6, 8);
    faces.emplace_back(3, 8, 9);

    // 5 adjacent faces 
    faces.emplace_back(4, 9, 5);
    faces.emplace_back(2, 4, 11);
    faces.emplace_back(6, 2, 10);
    faces.emplace_back(8, 6, 7);
    faces.emplace_back(9, 8, 1);


    // refine triangles
    for (int i = 0; i < recursionLevel; i++)
    {
        std::vector<TriangleIndices> faces2;
        for(auto& tri : faces)
        {
            // replace triangle by 4 triangles
            int a = GetMiddlePoint(tri.v1, tri.v2, vertList, middlePointIndexCache, radius);
            int b = GetMiddlePoint(tri.v2, tri.v3, vertList, middlePointIndexCache, radius);
            int c = GetMiddlePoint(tri.v3, tri.v1, vertList, middlePointIndexCache, radius);

            faces2.emplace_back(tri.v1, a, c);
            faces2.emplace_back(tri.v2, b, a);
            faces2.emplace_back(tri.v3, c, b);
            faces2.emplace_back(a, b, c);
        }
        faces = faces2;
    }

    m.Position = std::move(vertList);

    for (int i = 0; i < faces.size(); i++)
    {
        m.Triangles.push_back(faces[i].v1);
        m.Triangles.push_back(faces[i].v2);
        m.Triangles.push_back(faces[i].v3);
    }

    for (int i = 0; i < m.Position.size(); i++)
        m.Normal.push_back(m.Position[i].Normalized());

    m.PrepareForUpload();
    return m;
}

}
