//
// Copyright (C) Alexandr Vorontsov. 2018
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#pragma once

#include "Render/Geometry/MeshParser.h"

#include "fbxsdk.h"

namespace Kioto
{
class Mesh;

class ParserFBX : public MeshParser
{
public:
    void Init() override;
    void Shutdown() override;

    Mesh* ParseMesh(const std::string& path) override;
    void ParseMesh(Mesh* src) override;

private:
    struct IntermediateMesh
    {
        enum Elements
        {
            COLOR = 1 << 1,
            NORMAL = 1 << 2,
            UV = 1 << 3,
            TANGENT = 1 << 4,
            BITANGENT = 1 << 5
        };

        struct Vertex
        {
            Vector4 Pos;
            std::vector<Vector4> Color;
            Vector4 Norm;
            std::vector<Vector2> Uv;
            Vector4 Tangent;
            Vector4 Binormal;

            bool marked = false;

            bool operator== (const Vertex& other) const
            {
                bool p = Pos == other.Pos;
                for (size_t i = 0; i < Color.size(); ++i)
                {
                    if (Color[i] != other.Color[i])
                        return false;
                }
                bool n = Norm == other.Norm;
                for (size_t i = 0; i < Uv.size(); ++i)
                {
                    if (Uv[i] != other.Uv[i])
                        return false;
                }
                bool t = Tangent == other.Tangent;
                bool b = Binormal == other.Binormal;
                return p && n && t && b;
            }

        };

        std::vector<Vertex> Vertices;
        std::vector<uint32> Indices;
        uint8 mask;

        void Indexate()
        {
            Indices.reserve(Vertices.size());
            for (uint32 i = 0; i < Vertices.size(); ++i)
                Indices.push_back(i);

            uint32 markedBefore = 0;
            for (size_t i = 0; i < Vertices.size(); ++i)
            {
                if (Vertices[i].marked)
                {
                    ++markedBefore;
                    continue;
                }

                for (size_t j = i + 1; j < Vertices.size(); ++j)
                {
                    if (Vertices[j].marked)
                        continue;

                    if (Vertices[i] == Vertices[j])
                    {
                        Indices[j] = static_cast<uint32>(i) - markedBefore;
                        Vertices[j].marked = true;
                    }
                }
                Indices[i] -= markedBefore;
            }
            Vertices.erase(std::remove_if(Vertices.begin(), Vertices.end(), [](const Vertex& v) { return v.marked; }), Vertices.end());
        }

    };

    bool LoadScene(FbxManager* manager, FbxScene* scene, const char* filename);

    FbxManager* m_fbxManager = nullptr;

    void TraverseHiererchy(FbxScene* scene);
    void TraverseHiererchy(FbxNode* node, int32 depth);
    void ParseMesh(FbxNode* node);
    void ParseColors(IntermediateMesh::Vertex* vertex, FbxMesh* src, int32 vertexId, int32 controlPointIndex);
    void ParseUVs(IntermediateMesh::Vertex* vertex, FbxMesh* src, int32 vertexId, int32 controlPointIndex, int32 polygonIndex, int32 positionInPolygon);
    void ParseNormal(IntermediateMesh::Vertex* vertex, FbxMesh* src, int32 vertexId, int32 controlPointIndex);
    void ParseTangent(IntermediateMesh::Vertex* vertex, FbxMesh* src, int32 vertexId, int32 controlPointIndex);
    void ParseBinormal(IntermediateMesh::Vertex* vertex, FbxMesh* src, int32 vertexId, int32 controlPointIndex);
};
}