//
// Copyright (C) Alexandr Vorontsov. 2018
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#pragma once

#include <string>

namespace Kioto
{
class Mesh;

class MeshParser
{
public:
    virtual void Init() abstract;
    virtual void Shutdown() abstract;
    virtual Mesh* ParseMesh(const std::string& path) abstract;
    virtual void ParseMesh(Mesh* dst) abstract;

    virtual ~MeshParser() = default;
};
}