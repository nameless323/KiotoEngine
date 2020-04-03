#include "stdafx.h"

#include "Render/Geometry/ParserGLTF.h"

#include "Core/Logger/Logger.h"

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
            ParseModelNodes(model, model.nodes[node]);
        }
    }

    bool ParserGLTF::LoadModel(const std::string& path, tinygltf::Model& model)
    {
        tinygltf::TinyGLTF loader;
        std::string err;
        std::string warn;

        bool res = loader.LoadBinaryFromFile(&model, &err, &warn, path.c_str());
        if (!warn.empty())
        {
            LOG("WARN: ", warn);
        }

        if (!err.empty())
        {
            LOG("ERR: ", err);
        }

        if (!res)
            LOG("Failed to load glTF: ", path);
        else
            LOG("Loaded glTF: ", path);

        return res;
    }

    void ParserGLTF::ParseModelNodes(const tinygltf::Model& model, const tinygltf::Node& node)
    {
        ParseGLTFMesh(model, model.meshes[node.mesh]);
        for (int i : node.children)
            ParseModelNodes(model, model.nodes[i]);
    }

    void ParserGLTF::ParseGLTFMesh(const tinygltf::Model& model, const tinygltf::Mesh& mesh)
    {
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

                uint32 size = 1;
                if (accessor.type != TINYGLTF_TYPE_SCALAR)
                    size = accessor.type;

                uint32 elementSize = size * 4;

                int vaa = -1;
                if (attrib.first.compare("POSITION") == 0) vaa = 0;
                if (attrib.first.compare("NORMAL") == 0) vaa = 1;
                if (attrib.first.compare("TEXCOORD_0") == 0) vaa = 2;
                /*if (vaa > -1) {
                    glEnableVertexAttribArray(vaa);
                    glVertexAttribPointer(vaa, size, accessor.componentType,
                        accessor.normalized ? GL_TRUE : GL_FALSE,
                        byteStride, BUFFER_OFFSET(accessor.byteOffset));
                }
                else
                    std::cout << "vaa missing: " << attrib.first << std::endl;*/
            }
        }
    }

}