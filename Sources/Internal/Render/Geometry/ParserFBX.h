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
namespace Renderer
{
class Mesh;
}

class ParserFBX : public MeshParser
{
public:
    void Init() override;
    void Shutdown() override;

    Renderer::Mesh* ParseMesh(const std::string& path) override;
    void ParseMesh(Renderer::Mesh* dst) override;

private:
    bool LoadScene(FbxManager* manager, FbxScene* scene, const char* filename);

    FbxManager* m_fbxManager = nullptr;

    void TraverseHiererchy(FbxScene* scene, Renderer::Mesh* dst);
    void TraverseHiererchy(FbxNode* node, int32 depth, Renderer::Mesh* dst);
    void ParseFbxMesh(FbxNode* node, Renderer::Mesh* dst);
    void ParseColors(Renderer::IntermediateMesh::Vertex* vertex, FbxMesh* src, int32 vertexId, int32 controlPointIndex, uint32& mask);
    void ParseUVs(Renderer::IntermediateMesh::Vertex* vertex, FbxMesh* src, int32 vertexId, int32 controlPointIndex, int32 polygonIndex, int32 positionInPolygon, uint32& mask);
    void ParseNormal(Renderer::IntermediateMesh::Vertex* vertex, FbxMesh* src, int32 vertexId, int32 controlPointIndex, uint32& mask);
    void ParseTangent(Renderer::IntermediateMesh::Vertex* vertex, FbxMesh* src, int32 vertexId, int32 controlPointIndex, uint32& mask);
    void ParseBinormal(Renderer::IntermediateMesh::Vertex* vertex, FbxMesh* src, int32 vertexId, int32 controlPointIndex, uint32& mask);
};
}