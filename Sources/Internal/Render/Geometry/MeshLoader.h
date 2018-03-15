//
// Copyright (C) Alexandr Vorontsov. 2018
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#pragma once

#include <string>

namespace Kioto
{
class Mesh2;

namespace MeshLoader
{
void Init();
void Shutdown();
Mesh2* LoadMesh(const std::string& path);
void LoadMesh(Mesh2* src);
}
}
