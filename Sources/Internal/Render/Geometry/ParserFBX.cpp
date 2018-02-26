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

Vector4 FbxColorToKioto(const FbxColor& color)
{
    return { static_cast<float32>(color.mRed), static_cast<float32>(color.mGreen), static_cast<float32>(color.mBlue), static_cast<float32>(color.mAlpha) };
}

Vector2 FbxVector2ToKioto(const FbxVector2& vec)
{
    return { static_cast<float32>(vec.mData[0]), static_cast<float32>(vec.mData[1]) };
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
        if (unit != FbxSystemUnit::m)
            assert(false);


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
                assert(polygonSize == 3);

                for (int32 j = 0; j < polygonSize; ++j)
                {
                    int32 controlPointIndex = mesh->GetPolygonVertex(i, j);
                    FbxVector4 coord = controlPoints[controlPointIndex];
                    Vector4 vectorCoord = FbxVector4ToKioto(coord);

                    for (int32 l = 0; l < mesh->GetElementVertexColorCount(); ++l)
                    {
                        FbxGeometryElementVertexColor* vertexColorElement = mesh->GetElementVertexColor(l);
                        Vector4 color;
                        if (vertexColorElement->GetMappingMode() == FbxLayerElement::eByControlPoint)
                        {
                            switch (vertexColorElement->GetReferenceMode())
                            {
                            case FbxGeometryElement::eDirect:
                                color = FbxColorToKioto(vertexColorElement->GetDirectArray().GetAt(controlPointIndex));
                                break;
                            case FbxGeometryElement::eIndexToDirect:
                            {
                                int id = vertexColorElement->GetIndexArray().GetAt(controlPointIndex);
                                color = FbxColorToKioto(vertexColorElement->GetDirectArray().GetAt(id));
                            }
                            break;
                            default:
                                assert(false);
                            }
                        }
                        else if (vertexColorElement->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
                        {
                            switch (vertexColorElement->GetReferenceMode())
                            {
                            case FbxGeometryElement::eDirect:
                                color = FbxColorToKioto(vertexColorElement->GetDirectArray().GetAt(vertexId));
                                break;
                            case FbxGeometryElement::eIndexToDirect:
                            {
                                int id = vertexColorElement->GetIndexArray().GetAt(vertexId);
                                color = FbxColorToKioto(vertexColorElement->GetDirectArray().GetAt(id));
                            }
                            break;
                            default:
                                assert(false);;
                            }
                        }
                        else
                        {
                            assert(false);
                        }
                    }


                    //************************** UV
                    for (uint32 l = 0; l < mesh->GetElementUVCount(); ++l)
                    {
                        Vector2 uvCoord;
                        FbxGeometryElementUV* uv = mesh->GetElementUV(l);
                        if (uv->GetMappingMode() == FbxLayerElement::eByControlPoint)
                        {
                            switch (uv->GetReferenceMode())
                            {
                            case FbxGeometryElement::eDirect:
                                uvCoord = FbxVector2ToKioto(uv->GetDirectArray().GetAt(controlPointIndex));
                                break;
                            case FbxGeometryElement::eIndexToDirect:
                            {
                                int id = uv->GetIndexArray().GetAt(controlPointIndex);
                                uvCoord = FbxVector2ToKioto(uv->GetDirectArray().GetAt(id));
                            }
                            break;
                            default:
                                assert(false);;
                            }
                        }
                        else if (uv->GetMappingMode() == FbxLayerElement::eByPolygonVertex)
                        {
                            int textureUVIndex = mesh->GetTextureUVIndex(i, j);
                            uvCoord = FbxVector2ToKioto(uv->GetDirectArray().GetAt(textureUVIndex));
                        }
                        else
                        {
                            assert(false);
                        }
                    }

                    // normals////////////////////////////////////////

                    for (int32 l = 0; l < mesh->GetElementNormalCount(); ++l)
                    {
                        Vector4 normal;
                        FbxGeometryElementNormal* normals = mesh->GetElementNormal(l);
                        if (normals->GetMappingMode() == FbxLayerElement::eByPolygonVertex)
                        {
                            switch (normals->GetReferenceMode())
                            {
                            case FbxGeometryElement::eDirect:
                                normal = FbxVector4ToKioto(normals->GetDirectArray().GetAt(vertexId));
                                break;
                            case FbxGeometryElement::eIndexToDirect:
                            {
                                int id = normals->GetIndexArray().GetAt(vertexId);
                                normal = FbxVector4ToKioto(normals->GetDirectArray().GetAt(id));
                            }
                            break;
                            default:
                                assert(false);
                            }
                        }
                        else
                        {
                            assert(false);
                        }
                    }

                    // tangent////////////////////////////////////////

                    for (int32 l = 0; l < mesh->GetElementTangentCount(); ++l)
                    {
                        Vector4 tangent;
                        FbxGeometryElementTangent* tangents = mesh->GetElementTangent(l);
                        if (tangents->GetMappingMode() == FbxLayerElement::eByPolygonVertex)
                        {
                            switch (tangents->GetReferenceMode())
                            {
                            case FbxGeometryElement::eDirect:
                                tangent = FbxVector4ToKioto(tangents->GetDirectArray().GetAt(vertexId));
                                break;
                            case FbxGeometryElement::eIndexToDirect:
                            {
                                int id = tangents->GetIndexArray().GetAt(vertexId);
                                tangent = FbxVector4ToKioto(tangents->GetDirectArray().GetAt(id));
                            }
                            break;
                            default:
                                assert(false);
                            }
                        }
                        else
                        {
                            assert(false);
                        }
                    }


                    // binormal////////////////////////////////////////

                    for (int32 l = 0; l < mesh->GetElementBinormalCount(); ++l)
                    {
                        Vector4 binormal;
                        FbxGeometryElementBinormal* binormals = mesh->GetElementBinormal(l);
                        if (binormals->GetMappingMode() == FbxLayerElement::eByPolygonVertex)
                        {
                            switch (binormals->GetReferenceMode())
                            {
                            case FbxGeometryElement::eDirect:
                                binormal = FbxVector4ToKioto(binormals->GetDirectArray().GetAt(vertexId));
                                break;
                            case FbxGeometryElement::eIndexToDirect:
                            {
                                int id = binormals->GetIndexArray().GetAt(vertexId);
                                binormal = FbxVector4ToKioto(binormals->GetDirectArray().GetAt(id));
                            }
                            break;
                            default:
                                assert(false);
                            }
                        }
                        else
                        {
                            assert(false);
                        }
                    }

                    ++vertexId;
                }


            }

        }
    }




    for (int32 i = 0; i < node->GetChildCount(); ++i)
        TraverseHiererchy(node->GetChild(i), depth + 1);
}

}