#pragma once

#pragma once

#include "Render/Geometry/MeshParser.h"

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
    };
}