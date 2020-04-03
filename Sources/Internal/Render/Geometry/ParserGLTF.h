#pragma once

#pragma once

#include "Render/Geometry/MeshParser.h"

namespace tinygltf
{
    class Model;
    class Node;
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
        void ParseModelNodes(const tinygltf::Model& model, const tinygltf::Node& node);
        void ParseGLTFMesh(const tinygltf::Model& model, const tinygltf::Mesh& mesh);
    };
}