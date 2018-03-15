//
// Copyright (C) Alexandr Vorontsov. 2018
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#pragma once

#include <string>

namespace Kioto
{
class Mesh2;

class MeshParser
{
public:
    virtual void Init() abstract;
    virtual void Shutdown() abstract;
    virtual Mesh2* ParseMesh(const std::string& path) abstract;
    virtual void ParseMesh(Mesh2* dst) abstract;

    virtual ~MeshParser() = default;
};
}