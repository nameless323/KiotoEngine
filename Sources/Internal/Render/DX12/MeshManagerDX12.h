#pragma once

#include <vector>
#include <map>

#include "Render/RendererPublic.h"

namespace Kioto::Renderer
{
class Mesh;
struct StateDX;
class MeshDX12;

class MeshManagerDX12
{
public:
    MeshManagerDX12();
    ~MeshManagerDX12();
    void RegisterMesh(Mesh* mesh);
    void ProcessRegistrationQueue(const StateDX& state);
    MeshDX12* Find(MeshHandle handle);

private:
    struct TempMeshData // [a_vorontcov] Dangerous stuff, rethink later.
    {
        const byte* VertexData = nullptr;
        const byte* IndexData = nullptr;
        uint32 VertexDataSize = 0;
        uint32 IndexDataSize = 0;
        uint32 VertexDataStride = 0;
        uint32 VertexCount = 0;
        uint32 IndexCount = 0;
        MeshDX12* DstMesh = nullptr;

        TempMeshData(const byte* vertexData, const byte* indexData, uint32 vertexDataSize, uint32 indexDataSize, uint32 vertexDataStride, uint32 vertexCount, uint32 indexCount, MeshDX12* dstMesh)
            : VertexData(vertexData)
            , IndexData(indexData)
            , VertexDataSize(vertexDataSize)
            , IndexDataSize(indexDataSize)
            , VertexDataStride(vertexDataStride)
            , VertexCount(vertexCount)
            , IndexCount(indexCount)
            , DstMesh(dstMesh)
        {}
    };

    std::vector<TempMeshData> mMeshQueue;
    std::map<MeshHandle, MeshDX12*> mMeshes;
};
}