//
// Copyright (C) Alexandr Vorontsov. 2018
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#pragma once

#include "fbxsdk.h"

namespace Kioto
{
class ParserFBX
{
public:
    void Initialize(std::string path);
    void Shutdown();

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
        };

        std::vector<Vertex> Vertices;
        std::vector<uint32> Indices;
        uint8 mask;

        void Indexate()
        {
            Indices.reserve(Vertices.size());
            for (uint32 i = 0; i < Vertices.size(); ++i)
                Indices.push_back(i);

            uint32 currIndex = 0;
            for (auto i = Vertices.begin(); i != Vertices.end(); ++i)
            {
                for (auto j = i + 1; j != Vertices.end(); ++j)
                {

                }
            }
        }

    };

    bool LoadScene(FbxManager* manager, FbxDocument* scene, const char* filename);

    FbxManager* m_fbxManager = nullptr;
    FbxScene* m_scene = nullptr;

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