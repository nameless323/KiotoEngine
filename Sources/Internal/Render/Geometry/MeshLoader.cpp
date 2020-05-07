#include "stdafx.h"

#include "Render/Geometry/Mesh.h"

#include <map>

#include "AssetsSystem/FilesystemHelpers.h"
#include "Render/Geometry/MeshParser.h"
#include "Render/Geometry/ParserFBX.h"
#include "Render/Geometry/ParserGLTF.h"

namespace Kioto::MeshLoader
{
namespace
{
std::map<std::string, MeshParser*> MeshParsers;
static const std::string fbxExt = ".fbx";
static const std::string gltfExt = ".glb";
static const std::string gltfTxtExt = ".gltf";
}

void Init()
{
    MeshParsers[fbxExt] = new ParserFBX();
    MeshParsers[fbxExt]->Init();

    MeshParsers[gltfExt] = new ParserGLTF();
    MeshParsers[gltfExt]->Init();

    MeshParsers[gltfTxtExt] = MeshParsers[gltfTxtExt];
}

void Shutdown()
{
    MeshParsers[fbxExt]->Shutdown();
    SafeDelete(MeshParsers[fbxExt]);

    MeshParsers[gltfExt]->Shutdown();
    SafeDelete(MeshParsers[gltfExt]);

    MeshParsers.clear();
}

void LoadMesh(Renderer::Mesh* src)
{
    std::string ext = FilesystemHelpers::GetFileExtension(src->GetAssetPath());
    auto it = MeshParsers.find(ext);
    if (it == MeshParsers.end())
        assert(false);
    it->second->ParseMesh(src);
}

Renderer::Mesh* LoadMesh(const std::string& path)
{
    return nullptr;
}
}