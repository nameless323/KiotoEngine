//
// Copyright (C) Alexandr Vorontsov. 2018
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#pragma once

#include "fbxsdk.h"

namespace Kioto
{
class ParserFBX
{
public:
    void Initialize(std::string path);
    void Shutdown();

private:
    bool LoadScene(FbxManager* manager, FbxDocument* scene, const char* filename);

    FbxManager* m_fbxManager = nullptr;
    FbxScene* m_scene = nullptr;

    void TraverseHiererchy(FbxScene* scene);
    void TraverseHiererchy(FbxNode* node, int32 depth);
};
}