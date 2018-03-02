//
// Copyright (C) Alexandr Vorontsov. 2017
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#include "stdafx.h"

#include <vector>

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

void ParserFBX::Init()
{
    bool result = false;
    m_fbxManager = FbxManager::Create();
    if (m_fbxManager == nullptr)
    {
        throw "Cant create manager.";
    }

    FbxIOSettings* ios = FbxIOSettings::Create(m_fbxManager, IOSROOT);
    m_fbxManager->SetIOSettings(ios);
}

void ParserFBX::Shutdown()
{
    m_fbxManager->Destroy();
}

Mesh* ParserFBX::ParseMesh(const std::string& path)
{
    Mesh* mesh = new Mesh();
    mesh->SetAssetPath(path);
    ParseMesh(mesh);
    return mesh;
}

void ParserFBX::ParseMesh(Mesh* src)
{
    FbxScene* scene = FbxScene::Create(m_fbxManager, "ImportFbxScene");
    if (scene == nullptr)
        assert(false);

    bool res = LoadScene(m_fbxManager, scene, src->GetAssetPath().c_str());
    FbxDocumentInfo* sceneInfo = scene->GetSceneInfo();
    TraverseHiererchy(scene);
    scene->Destroy(true);
}

bool ParserFBX::LoadScene(FbxManager* manager, FbxScene* scene, const char* filename)
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
    FbxSystemUnit::m.ConvertScene(scene);
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

    auto unit = scene->GetGlobalSettings().GetSystemUnit();
    if (unit != FbxSystemUnit::m)
        assert(false);

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
        assert(false);
    else
    {
        attribType = node->GetNodeAttribute()->GetAttributeType();
        if (attribType == FbxNodeAttribute::eMesh)
            ParseMesh(node);
    }

    for (int32 i = 0; i < node->GetChildCount(); ++i)
        TraverseHiererchy(node->GetChild(i), depth + 1);
}

void ParserFBX::ParseMesh(FbxNode* node)
{
    FbxVector4 mat = node->EvaluateLocalRotation(0);
    //Matrix4 loclaTransform(reinterpret_cast<float32>(mat.mData));

    FbxMesh* mesh = reinterpret_cast<FbxMesh*>(node->GetNodeAttribute());
 
    IntermediateMesh resMesh;

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

            resMesh.Vertices.emplace_back();
            IntermediateMesh::Vertex* vert = &resMesh.Vertices.back();
            vert->Pos = FbxVector4ToKioto(coord);

            ParseColors(vert, mesh, vertexId, controlPointIndex);
            ParseUVs(vert, mesh, vertexId, controlPointIndex, i, j);
            ParseNormal(vert, mesh, vertexId, controlPointIndex);
            ParseTangent(vert, mesh, vertexId, controlPointIndex);
            ParseBinormal(vert, mesh, vertexId, controlPointIndex);

            ++vertexId;
        }
    }
    resMesh.Indexate();
}

void ParserFBX::ParseColors(IntermediateMesh::Vertex* vertex, FbxMesh* src, int32 vertexId, int32 controlPointIndex)
{
    for (int32 l = 0; l < src->GetElementVertexColorCount(); ++l)
    {
        FbxGeometryElementVertexColor* vertexColorElement = src->GetElementVertexColor(l);
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
                assert(false);
            }
        }
        else
        {
            assert(false);
        }
        vertex->Color.push_back(color);
    }
}

void ParserFBX::ParseUVs(IntermediateMesh::Vertex* vertex, FbxMesh* src, int32 vertexId, int32 controlPointIndex, int32 polygonIndex, int32 positionInPolygon)
{
    for (int32 l = 0; l < src->GetElementUVCount(); ++l)
    {
        Vector2 uv;
        FbxGeometryElementUV* uvElement = src->GetElementUV(l);
        if (uvElement->GetMappingMode() == FbxLayerElement::eByControlPoint)
        {
            switch (uvElement->GetReferenceMode())
            {
            case FbxGeometryElement::eDirect:
                uv = FbxVector2ToKioto(uvElement->GetDirectArray().GetAt(controlPointIndex));
                break;
            case FbxGeometryElement::eIndexToDirect:
            {
                int id = uvElement->GetIndexArray().GetAt(controlPointIndex);
                uv = FbxVector2ToKioto(uvElement->GetDirectArray().GetAt(id));
            }
            break;
            default:
                assert(false);;
            }
        }
        else if (uvElement->GetMappingMode() == FbxLayerElement::eByPolygonVertex)
        {
            int textureUVIndex = src->GetTextureUVIndex(polygonIndex, positionInPolygon);
            uv = FbxVector2ToKioto(uvElement->GetDirectArray().GetAt(textureUVIndex));
        }
        else
        {
            assert(false);
        }
        vertex->Uv.push_back(uv);
    }
}

void ParserFBX::ParseNormal(IntermediateMesh::Vertex* vertex, FbxMesh* src, int32 vertexId, int32 controlPointIndex)
{
    assert(src->GetElementNormalCount() <= 1);
    for (int32 l = 0; l < src->GetElementNormalCount(); ++l)
    {
        Vector4 normal;
        FbxGeometryElementNormal* normalElement = src->GetElementNormal(l);
        if (normalElement->GetMappingMode() == FbxLayerElement::eByPolygonVertex)
        {
            switch (normalElement->GetReferenceMode())
            {
            case FbxGeometryElement::eDirect:
                normal = FbxVector4ToKioto(normalElement->GetDirectArray().GetAt(vertexId));
                break;
            case FbxGeometryElement::eIndexToDirect:
            {
                int id = normalElement->GetIndexArray().GetAt(vertexId);
                normal = FbxVector4ToKioto(normalElement->GetDirectArray().GetAt(id));
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
        vertex->Norm = normal;
    }
}

void ParserFBX::ParseTangent(IntermediateMesh::Vertex* vertex, FbxMesh* src, int32 vertexId, int32 controlPointIndex)
{
    assert(src->GetElementTangentCount() <= 1);
    for (int32 l = 0; l < src->GetElementTangentCount(); ++l)
    {
        Vector4 tangent;
        FbxGeometryElementTangent* tangentElement = src->GetElementTangent(l);
        if (tangentElement->GetMappingMode() == FbxLayerElement::eByPolygonVertex)
        {
            switch (tangentElement->GetReferenceMode())
            {
            case FbxGeometryElement::eDirect:
                tangent = FbxVector4ToKioto(tangentElement->GetDirectArray().GetAt(vertexId));
                break;
            case FbxGeometryElement::eIndexToDirect:
            {
                int id = tangentElement->GetIndexArray().GetAt(vertexId);
                tangent = FbxVector4ToKioto(tangentElement->GetDirectArray().GetAt(id));
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
        vertex->Tangent = tangent;
    }
}

void ParserFBX::ParseBinormal(IntermediateMesh::Vertex* vertex, FbxMesh* src, int32 vertexId, int32 controlPointIndex)
{
    assert(src->GetElementBinormalCount() <= 1);
    for (int32 l = 0; l < src->GetElementBinormalCount(); ++l)
    {
        Vector4 binormal;
        FbxGeometryElementBinormal* binormals = src->GetElementBinormal(l);
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
        vertex->Binormal = binormal;
    }
}

}