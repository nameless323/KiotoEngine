#include "stdafx.h"

#include "Render/DX12/MeshManagerDX12.h"

#include "Render/DX12/Geometry/MeshDX12.h"
#include "Render/Geometry/Mesh.h"

namespace Kioto::Renderer
{
MeshManagerDX12::MeshManagerDX12()
{
    m_meshQueue.reserve(256);
}

MeshManagerDX12::~MeshManagerDX12()
{
    for (auto& mesh : m_meshes)
        SafeDelete(mesh.second);
    m_meshes.clear();
}

void MeshManagerDX12::RegisterMesh(Mesh* mesh)
{
    auto it = m_meshes.find(mesh->GetHandle());
    if (it != m_meshes.end())
    {
        assert(false);
        return;
    }
    MeshDX12* dxmesh = new MeshDX12();
    dxmesh->SetHandle(GetNewHandle());
    mesh->SetHandle(dxmesh->GetHandle());
    m_meshes[mesh->GetHandle()] = dxmesh;

    m_meshQueue.emplace_back(mesh->GetVertexData(), mesh->GetIndexData(), mesh->GetVertexDataSize(), mesh->GetIndexDataSize(), mesh->GetVertexDataStride(), mesh->GetVertexCount(), mesh->GetIndexCount(), dxmesh);
}

void MeshManagerDX12::ProcessRegistrationQueue(const StateDX& state)
{
    for (auto& m : m_meshQueue)
        m.DstMesh->Create(m.VertexData, m.IndexData, m.VertexDataSize, m.IndexDataSize, m.VertexDataStride, m.VertexCount, m.IndexCount, state);
    m_meshQueue.clear();
}

MeshDX12* MeshManagerDX12::Find(MeshHandle handle)
{
    auto it = m_meshes.find(handle);
    if (it == m_meshes.end())
        return nullptr;
    return it->second;
}
}