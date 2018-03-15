//
// Copyright (C) Alexandr Vorontsov. 2018
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#include "stdafx.h"

#include "Render/Geometry/Mesh2.h"

#include "Render/Geometry/MeshParser.h"
#include "Render/Geometry/ParserFBX.h"

#include <map>

namespace Kioto::MeshLoader
{
namespace
{
std::map<std::string, MeshParser*> MeshParsers;
static const std::string fbxExt = ".fbx";
}

void Init()
{
    MeshParsers[fbxExt] = new ParserFBX();
    MeshParsers[fbxExt]->Init();
}

void Shutdown()
{
    MeshParsers[fbxExt]->Shutdown();
    SafeDelete(MeshParsers[fbxExt]);
}

void LoadMesh(Mesh2* src)
{
    std::string& path = src->GetAssetPath();
    size_t lastPeriod = path.find_last_of('.');
    if (lastPeriod == std::string::npos)
        assert(false);
    std::string ext = path.substr(lastPeriod);
    auto it = MeshParsers.find(ext);
    if (it == MeshParsers.end())
        assert(false);
    it->second->ParseMesh(src);
}

}