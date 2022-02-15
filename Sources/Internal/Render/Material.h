#pragma once

#include <string>

#include "AssetsSystem/Asset.h"
#include "Core/CoreTypes.h"
#include "Render/ConstantBuffer.h"
#include "Render/PipelineState.h"
#include "Render/RendererPublic.h"
#include "Render/ShaderData.h"

namespace Kioto::Renderer
{
class Shader;

struct TextureAssetDescription
{
    std::string Name;
    std::string Path;
    uint16 Offset = -1;
};

using PassName = std::string;

class Material : public Asset
{
public:
    Material() = default;
    Material(const std::string& path);
    ~Material();

    void SetHandle(MaterialHandle handle);
    MaterialHandle GetHandle() const;

    void BuildMaterialForPass(const RenderPass* pass);

    const PipelineState& GetPipelineState(const PassName& passName) const;
    PipelineState& GetPipelineState(const PassName& passName);
    const std::unordered_map<PassName, PipelineState>& GetPipelineStates() const;

    const std::vector<TextureAssetDescription>& GetTextureAssetDescriptions(const PassName& passName) const;
    const std::unordered_map<PassName, std::vector<TextureAssetDescription>>& GetTextureAssetDescriptions() const;

private:
    void DeserializeRenderPassConfig(const YAML::Node& pass);

    MaterialHandle mHandle;

    // [a_vorontcov] For each pass contains appropriate pipeline state
    std::unordered_map<PassName, PipelineState> mMaterialPipelineStates;
    // [a_vorontcov] All textures for each pass
    std::unordered_map<PassName, std::vector<TextureAssetDescription>> mTextures;

    std::vector<RenderPassHandle> mBuildedPassesHandles;
};

inline void Material::SetHandle(MaterialHandle handle)
{
    mHandle = handle;
}

inline MaterialHandle Material::GetHandle() const
{
    return mHandle;
}
}