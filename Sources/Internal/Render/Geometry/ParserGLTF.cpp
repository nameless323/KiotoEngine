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

}