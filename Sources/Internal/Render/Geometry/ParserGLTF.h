#pragma once

#pragma once

#include "Render/Geometry/MeshParser.h"

#include "Render/Geometry/IntermediateMesh.h"

namespace tinygltf
{
    class Model;
    class Node;
    struct Primitive;
    struct Mesh;
}

namespace Kioto
{
    namespace Renderer
    {
        class Mesh;
    }

    class ParserGLTF : public MeshParser
    {
    public:
        void Init() override;
        void Shutdown() override;

        Renderer::Mesh* ParseMesh(const std::string& path) override;
        void ParseMesh(Renderer::Mesh* dst) override;

    private:
        bool LoadModel(const std::string& path, tinygltf::Model& model);
        void ParseModelNodes(const tinygltf::Model& model, const tinygltf::Node& node, Renderer::Mesh* dst);
        void ParseGLTFMesh(const tinygltf::Model& model, const tinygltf::Mesh& mesh, Renderer::Mesh* dst);
        void ParseVertices(const tinygltf::Model& model, const tinygltf::Mesh& mesh, const tinygltf::Primitive& primitive, std::vector<Renderer::IntermediateMesh::Vertex>& vertices, uint32& layoutMask);
        void ParseIndices(const tinygltf::Model& model, const tinygltf::Mesh& mesh, const tinygltf::Primitive& primitive, std::vector<uint32>& indices);
    };
}