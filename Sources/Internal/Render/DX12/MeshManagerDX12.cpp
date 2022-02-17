#include "Render/DX12/MeshManagerDX12.h"

#include <cassert>

#include "Core/CoreHelpers.h"
#include "Render/DX12/Geometry/MeshDX12.h"
#include "Render/Geometry/Mesh.h"

namespace Kioto::Renderer
{
MeshManagerDX12::MeshManagerDX12()
{
    mMeshQueue.reserve(256);
}

MeshManagerDX12::~MeshManagerDX12()
{
    for (auto& mesh : mMeshes)
        SafeDelete(mesh.second);
    mMeshes.clear();
}

void MeshManagerDX12::RegisterMesh(Mesh* mesh)
{
    auto it = mMeshes.find(mesh->GetHandle());
    if (it != mMeshes.end())
    {
        assert(false);
        return;
    }
    MeshDX12* dxmesh = new MeshDX12();
    dxmesh->SetHandle(GetNewHandle());
    mesh->SetHandle(dxmesh->GetHandle());
    mMeshes[mesh->GetHandle()] = dxmesh;

    mMeshQueue.emplace_back(mesh->GetVertexData(), mesh->GetIndexData(), mesh->GetVertexDataSize(), mesh->GetIndexDataSize(), mesh->GetVertexDataStride(), mesh->GetVertexCount(), mesh->GetIndexCount(), dxmesh);
}

void MeshManagerDX12::ProcessRegistrationQueue(const StateDX& state)
{
    for (auto& m : mMeshQueue)
        m.DstMesh->Create(m.VertexData, m.IndexData, m.VertexDataSize, m.IndexDataSize, m.VertexDataStride, m.VertexCount, m.IndexCount, state);
    mMeshQueue.clear();
}

MeshDX12* MeshManagerDX12::Find(MeshHandle handle)
{
    auto it = mMeshes.find(handle);
    if (it == mMeshes.end())
        return nullptr;
    return it->second;
}
}