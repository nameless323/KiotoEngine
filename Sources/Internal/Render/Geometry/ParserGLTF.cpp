#include "stdafx.h"

#include "Render/Geometry/ParserGLTF.h"

#include "AssetsSystem/AssetsSystem.h"
#include "Core/Logger/Logger.h"
#include "Render/Geometry/IntermediateMesh.h"

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define TINYGLTF_NOEXCEPTION
#define JSON_NOEXCEPTION
#include "Sources/External/TinyGLTF/tiny_gltf.h"

namespace Kioto
{
    void ParserGLTF::Init()
    {
    }

    void ParserGLTF::Shutdown()
    {
    }

    Renderer::Mesh* ParserGLTF::ParseMesh(const std::string& path)
    {
        return nullptr;
    }

    void ParserGLTF::ParseMesh(Renderer::Mesh* dst)
    {
        tinygltf::Model model;
        bool res = LoadModel(dst->GetAssetPath(), model);

        const tinygltf::Scene& scene = model.scenes[model.defaultScene];
        for (int node : scene.nodes)
        {
            ParseModelNodes(model, model.nodes[node], dst);
        }
    }

    bool ParserGLTF::LoadModel(const std::string& path, tinygltf::Model& model)
    {
        tinygltf::TinyGLTF loader;
        std::string err;
        std::string warn;

        bool res = false;
        std::string ext = AssetsSystem::GetFileExtension(path);
        if (ext == ".glb")
            res = loader.LoadBinaryFromFile(&model, &err, &warn, path.c_str());
        else if (ext == ".gltf")
            res = loader.LoadASCIIFromFile(&model, &err, &warn, path.c_str());
        else
            assert(false);

        if (!warn.empty())
            LOG("WARN: ", warn);

        if (!err.empty())
            LOG("ERR: ", err);

        if (!res)
            LOG("Failed to load glTF: ", path);
        else
            LOG("Loaded glTF: ", path);

        return res;
    }

    void ParserGLTF::ParseModelNodes(const tinygltf::Model& model, const tinygltf::Node& node, Renderer::Mesh* dst)
    {
        ParseGLTFMesh(model, model.meshes[node.mesh], dst);
        for (int i : node.children) // [a_vorontsov] Not really supported at the moment...
            ParseModelNodes(model, model.nodes[i], dst);
    }

    void ParserGLTF::ParseGLTFMesh(const tinygltf::Model& model, const tinygltf::Mesh& mesh, Renderer::Mesh* dst)
    {
        Renderer::IntermediateMesh resMesh;

        for (size_t i = 0; i < mesh.primitives.size(); ++i)
        {
            tinygltf::Primitive primitive = mesh.primitives[i];
            tinygltf::Accessor indexAccessor = model.accessors[primitive.indices];

            for (auto& attrib : primitive.attributes)
            {
                tinygltf::Accessor accessor = model.accessors[attrib.second];
                const tinygltf::BufferView& bufferView = model.bufferViews[accessor.bufferView];

                const tinygltf::Buffer& buffer = model.buffers[bufferView.buffer];
                const byte* bufferData = &model.buffers[bufferView.buffer].data.at(0);
                size_t byteOffset = bufferView.byteOffset;
                size_t byteLength = bufferView.byteLength;

                uint32 byteStride = accessor.ByteStride(bufferView);
                size_t elemNumber = byteLength / byteStride;
                if (resMesh.Vertices.size() == 0)
                    resMesh.Vertices.resize(elemNumber);

                uint32 size = 1;
                if (accessor.type != TINYGLTF_TYPE_SCALAR)
                    size = accessor.type;

                uint32 elementByteSize = size * 4;

                std::vector<Vector3> positions;
                if (attrib.first.compare("POSITION") == 0)
                {
                    resMesh.LayoutMask |= Renderer::IntermediateMesh::Position;
                    const byte* bufferStart = bufferData + byteOffset;
                    for (size_t i = 0; i < elemNumber; ++i)
                    {
                        float32 x = *(reinterpret_cast<const float32*>(bufferStart + size_t(byteStride) * size_t(i) + 0));
                        float32 y = *(reinterpret_cast<const float32*>(bufferStart + size_t(byteStride) * size_t(i) + 4));
                        float32 z = *(reinterpret_cast<const float32*>(bufferStart + size_t(byteStride) * size_t(i) + 8));
                        resMesh.Vertices[i].Pos = { x, y, z, 0.0f };
                    }
                }
                else if (attrib.first.compare("NORMAL") == 0)
                {
                    resMesh.LayoutMask |= Renderer::IntermediateMesh::Normal;
                    const byte* bufferStart = bufferData + byteOffset;
                    for (size_t i = 0; i < elemNumber; ++i)
                    {
                        float32 x = *(reinterpret_cast<const float32*>(bufferStart + size_t(byteStride) * size_t(i) + 0));
                        float32 y = *(reinterpret_cast<const float32*>(bufferStart + size_t(byteStride) * size_t(i) + 4));
                        float32 z = *(reinterpret_cast<const float32*>(bufferStart + size_t(byteStride) * size_t(i) + 8));
                        resMesh.Vertices[i].Norm = { x, y, z, 0.0f };
                    }
                }
                else if (attrib.first.compare("TEXCOORD_0") == 0)
                {
                    resMesh.LayoutMask |= Renderer::IntermediateMesh::UV0;
                    const byte* bufferStart = bufferData + byteOffset;
                    for (size_t i = 0; i < elemNumber; ++i)
                    {
                        float32 u = *(reinterpret_cast<const float32*>(bufferStart + size_t(byteStride) * size_t(i) + 0));
                        float32 v = *(reinterpret_cast<const float32*>(bufferStart + size_t(byteStride) * size_t(i) + 4));
                        resMesh.Vertices[i].Uv.push_back({ u, v });
                    }
                }
                else
                    assert(false);
            }
        }
        resMesh.Indexate();
        dst->FromIntermediateMesh(resMesh);
        // [a_vorontcov] You can also find image part of the parsing here https://github.com/syoyo/tinygltf/blob/master/examples/basic/main.cpp
    }

}