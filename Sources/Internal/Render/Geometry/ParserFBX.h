//
// Copyright (C) Alexandr Vorontsov. 2018
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#pragma once

#include "Render/Geometry/MeshParser.h"

#include "Render/Geometry/IntermediateMesh.h"

#include "fbxsdk.h"

namespace Kioto
{
class Mesh;

class ParserFBX : public MeshParser
{
public:
    void Init() override;
    void Shutdown() override;

    Mesh2* ParseMesh(const std::string& path) override;
    void ParseMesh(Mesh2* dst) override;

private:
    bool LoadScene(FbxManager* manager, FbxScene* scene, const char* filename);

    FbxManager* m_fbxManager = nullptr;

    void TraverseHiererchy(FbxScene* scene, Mesh2* dst);
    void TraverseHiererchy(FbxNode* node, int32 depth, Mesh2* dst);
    void ParseFbxMesh(FbxNode* node, Mesh2* dst);
    void ParseColors(IntermediateMesh::Vertex* vertex, FbxMesh* src, int32 vertexId, int32 controlPointIndex, uint32& mask);
    void ParseUVs(IntermediateMesh::Vertex* vertex, FbxMesh* src, int32 vertexId, int32 controlPointIndex, int32 polygonIndex, int32 positionInPolygon, uint32& mask);
    void ParseNormal(IntermediateMesh::Vertex* vertex, FbxMesh* src, int32 vertexId, int32 controlPointIndex, uint32& mask);
    void ParseTangent(IntermediateMesh::Vertex* vertex, FbxMesh* src, int32 vertexId, int32 controlPointIndex, uint32& mask);
    void ParseBinormal(IntermediateMesh::Vertex* vertex, FbxMesh* src, int32 vertexId, int32 controlPointIndex, uint32& mask);
};
}