//
// Copyright (C) Alexandr Vorontsov. 2017
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#include "stdafx.h"

#include "Render/Geometry/ParserFBX.h"

namespace Kioto
{

namespace
{
Vector4 FbxVector4ToKioto(const FbxVector4& vec)
{
    return { static_cast<float32>(vec.mData[0]), static_cast<float32>(vec.mData[1]), static_cast<float32>(vec.mData[2]), static_cast<float32>(vec.mData[3]) };
}
}

void ParserFBX::Initialize(std::string path)
{
    bool result = false;
    m_fbxManager = FbxManager::Create();
    if (m_fbxManager == nullptr)
    {
        throw "Cant create manager.";
    }

    FbxIOSettings* ios = FbxIOSettings::Create(m_fbxManager, IOSROOT);
    m_fbxManager->SetIOSettings(ios);

    m_scene = FbxScene::Create(m_fbxManager, "ImportFbxScene");
    if (m_scene == nullptr)
    {
        throw "cant create fbx scene";
    }

    bool res = LoadScene(m_fbxManager, m_scene, path.c_str());
    FbxDocumentInfo* sceneInfo = m_scene->GetSceneInfo();
    TraverseHiererchy(m_scene);
}

void ParserFBX::Shutdown()
{
    m_fbxManager->Destroy();
}

bool ParserFBX::LoadScene(FbxManager* manager, FbxDocument* scene, const char* filename)
{
    int32 fileMajor = 0;
    int32 fileMinor = 0;
    int32 fileRevision = 0;

    int32 skdMajor = 0;
    int32 sdkMinor = 0;
    int32 sdkRevision = 0;

    int32 i = 0;
    int32 animStackCount = 0;
    bool status = false;

    FbxManager::GetFileFormatVersion(skdMajor, sdkMinor, sdkRevision);

    FbxImporter* importer = FbxImporter::Create(m_fbxManager, "");
    bool importStatus = importer->Initialize(filename, -1, m_fbxManager->GetIOSettings());
    importer->GetFileVersion(fileMajor, fileMinor, fileRevision);
    if (!importStatus)
    {
        FbxString error = importer->GetStatus().GetErrorString();
        //FBXSDK_printf("Call to FbxImporter::Initialize() failed.\n");
        //FBXSDK_printf("Error returned: %s\n\n", error.Buffer());
        
        if (importer->GetStatus().GetCode() == FbxStatus::eInvalidFileVersion)
        {
            //FBXSDK_printf("FBX file format version for this FBX SDK is %d.%d.%d\n", lSDKMajor, lSDKMinor, lSDKRevision);
            //FBXSDK_printf("FBX file format version for file '%s' is %d.%d.%d\n\n", pFilename, lFileMajor, lFileMinor, lFileRevision);
        }
        return false;
    }
    FbxSystemUnit::m.ConvertScene(m_scene);
    if (importer->IsFBX())
    {
        animStackCount = importer->GetAnimStackCount();

        //IOS_REF.SetBoolProp(IMP_FBX_MATERIAL, true);
        //IOS_REF.SetBoolProp(IMP_FBX_TEXTURE, true);
        //IOS_REF.SetBoolProp(IMP_FBX_LINK, true);
        //IOS_REF.SetBoolProp(IMP_FBX_SHAPE, true);
        //IOS_REF.SetBoolProp(IMP_FBX_GOBO, true);
        //IOS_REF.SetBoolProp(IMP_FBX_ANIMATION, true);
        //IOS_REF.SetBoolProp(IMP_FBX_GLOBAL_SETTINGS, true);
    }

    status = importer->Import(scene);
    if (!status)
        throw "cant open fbx";
    importer->Destroy();

    return status;
}

void ParserFBX::TraverseHiererchy(FbxScene* scene)
{
    FbxNode* root = scene->GetRootNode();
    for (int32 i = 0; i < root->GetChildCount(); ++i)
        TraverseHiererchy(root->GetChild(i), 0);
}

void ParserFBX::TraverseHiererchy(FbxNode* node, int32 depth)
{
    FbxString string = node->GetName();
    std::string name(string.Buffer());


    FbxNodeAttribute::EType attribType = FbxNodeAttribute::eUnknown;
    if (node->GetNodeAttribute() == nullptr)
        throw "Null node attribute";
    else
    {
        attribType = node->GetNodeAttribute()->GetAttributeType();
        auto unit = m_scene->GetGlobalSettings().GetSystemUnit();
        int p = 0;
        if (unit == FbxSystemUnit::cm)
        {
            ++p;
        }
        else if (unit == FbxSystemUnit::m)
        {
            ++p;
        }
        else if (unit == FbxSystemUnit::Inch)
        {
            ++p;
        }
        else if (unit == FbxSystemUnit::Foot)
        {
            ++p;
        }


        if (attribType == FbxNodeAttribute::eMesh)
        {
            FbxVector4 mat = node->EvaluateLocalRotation(0);
            //Matrix4 loclaTransform(reinterpret_cast<float32>(mat.mData));

            FbxMesh* mesh = reinterpret_cast<FbxMesh*>(node->GetNodeAttribute());

            int32 polygonCount = mesh->GetPolygonCount();
            FbxVector4* controlPoints = mesh->GetControlPoints();
            int32 vertexId = 0;
            for (int32 i = 0; i < polygonCount; ++i)
            {
                for (int32 l = 0; l < mesh->GetElementPolygonGroupCount(); l++)
                {
                    FbxGeometryElementPolygonGroup* polGroup = mesh->GetElementPolygonGroup(i);
                }
                int32 polygonSize = mesh->GetPolygonSize(i);

                for (int32 j = 0; j < polygonSize; ++j)
                {
                    int32 controlPointIndex = mesh->GetPolygonVertex(i, j);
                    FbxVector4 coord = controlPoints[controlPointIndex];
                    Vector4 v4c = FbxVector4ToKioto(coord);
                }


            }

        }
    }




    for (int32 i = 0; i < node->GetChildCount(); ++i)
        TraverseHiererchy(node->GetChild(i), depth + 1);
}

}