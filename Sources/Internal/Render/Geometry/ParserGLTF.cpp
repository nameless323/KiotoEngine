#include "Render/Geometry/ParserGLTF.h"

#include "AssetsSystem/FilesystemHelpers.h"
#include "Core/Logger/Logger.h"
#include "Render/Geometry/IntermediateMesh.h"
#include "Render/Geometry/Mesh.h"

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define TINYGLTF_NOEXCEPTION
#define JSON_NOEXCEPTION
#include "Sources/External/TinyGLTF/tiny_gltf.h"

namespace Kioto
{
    namespace
    {
        template <typename T>
        T GetElementFromBuffer(const byte* bufferStart, uint32 byteStride, size_t elemIndex, uint32 offsetInElem = 0)
        {
            return *(reinterpret_cast<const T*>(bufferStart + size_t(byteStride) * size_t(elemIndex) + offsetInElem));
        }
    }

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
            ParseModelNodes(model, model.nodes[node], dst);
    }

    bool ParserGLTF::LoadModel(const std::string& path, tinygltf::Model& model)
    {
        tinygltf::TinyGLTF loader;
        std::string err;
        std::string warn;

        bool res = false;
        std::string ext = FilesystemHelpers::GetFileExtension(path);
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
        //else
        //    LOG("Loaded glTF: ", path);

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

            ParseVertices(model, mesh, primitive, resMesh.Vertices, resMesh.LayoutMask);
            ParseIndices(model, mesh, primitive, resMesh.Indices);
        }

        dst->FromIntermediateMesh(resMesh);
        // [a_vorontcov] You can also find image part of the parsing here https://github.com/syoyo/tinygltf/blob/master/examples/basic/main.cpp
    }

    void ParserGLTF::ParseVertices(const tinygltf::Model& model, const tinygltf::Mesh& mesh, const tinygltf::Primitive& primitive, std::vector<Renderer::IntermediateMesh::Vertex>& vertices, uint32& layoutMask)
    {
        for (auto& attrib : primitive.attributes)
        {
            tinygltf::Accessor accessor = model.accessors[attrib.second];
            const tinygltf::BufferView& bufferView = model.bufferViews[accessor.bufferView];

            const tinygltf::Buffer& buffer = model.buffers[bufferView.buffer];
            const byte* bufferData = &model.buffers[bufferView.buffer].data.at(0);
            const byte* bufferStart = bufferData + bufferView.byteOffset;

            size_t elemCount = accessor.count;
            if (vertices.empty())
                vertices.resize(elemCount);
            assert(vertices.size() == elemCount);

            // [a_vorontcov] TODO: don't assume the elem length in the buffer. i.e. UV can be more than 2 floats.
            //uint32 size = 1;
            //if (accessor.type != TINYGLTF_TYPE_SCALAR)
            //    size = accessor.type;

            uint32 byteStride = accessor.ByteStride(bufferView);
            if (attrib.first.compare("POSITION") == 0)
            {
                layoutMask |= Renderer::IntermediateMesh::Position;
                for (size_t i = 0; i < elemCount; ++i)
                {
                    float32 x = GetElementFromBuffer<float32>(bufferStart, byteStride, i, 0);
                    float32 y = GetElementFromBuffer<float32>(bufferStart, byteStride, i, 4);
                    float32 z = GetElementFromBuffer<float32>(bufferStart, byteStride, i, 8);
                    vertices[i].Pos = { x, y, z, 0.0f };
                }
            }
            else if (attrib.first.compare("NORMAL") == 0)
            {
                layoutMask |= Renderer::IntermediateMesh::Normal;
                for (size_t i = 0; i < elemCount; ++i)
                {
                    float32 x = GetElementFromBuffer<float32>(bufferStart, byteStride, i, 0);
                    float32 y = GetElementFromBuffer<float32>(bufferStart, byteStride, i, 4);
                    float32 z = GetElementFromBuffer<float32>(bufferStart, byteStride, i, 8);
                    vertices[i].Norm = { x, y, z, 0.0f };
                }
            }
            else if (attrib.first.compare("TEXCOORD_0") == 0)
            {
                layoutMask |= Renderer::IntermediateMesh::UV0;
                for (size_t i = 0; i < elemCount; ++i)
                {
                    float32 u = GetElementFromBuffer<float32>(bufferStart, byteStride, i, 0);
                    float32 v = GetElementFromBuffer<float32>(bufferStart, byteStride, i, 4);
                    vertices[i].Uv.push_back({ u, v });
                }
            }
            else
                assert(false);
        }
    }

    void ParserGLTF::ParseIndices(const tinygltf::Model& model, const tinygltf::Mesh& mesh, const tinygltf::Primitive& primitive, std::vector<uint32>& indices)
    {
        tinygltf::Accessor indexAccessor = model.accessors[primitive.indices];
        indices.reserve(indexAccessor.count);

        const tinygltf::BufferView& indexView = model.bufferViews[indexAccessor.bufferView];
        const byte* bufferData = &model.buffers[indexView.buffer].data.at(0);
        size_t byteOffset = indexView.byteOffset;
        size_t byteLength = indexView.byteLength;

        uint32 byteStride = indexAccessor.ByteStride(indexView);
        if (byteStride == 2)
        {
            for (size_t i = 0; i < indexAccessor.count; ++i)
            {
                const byte* bufferStart = bufferData + byteOffset;
                int16 index = GetElementFromBuffer<int16>(bufferStart, byteStride, i);
                indices.push_back(index);
            }
        }
        else if (byteStride == 4)
        {
            for (size_t i = 0; i < indexAccessor.count; ++i)
            {
                const byte* bufferStart = bufferData + byteOffset;
                int32 index = GetElementFromBuffer<int32>(bufferStart, byteStride, i);
                indices.push_back(index);
            }
        }
        else
            assert(false);
    }
}