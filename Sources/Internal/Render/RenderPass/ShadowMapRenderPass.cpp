#include "stdafx.h"

#include "Render/RenderPass/ShadowMapRenderPass.h"

#include "Core/KiotoEngine.h"
#include "Render/Camera.h"
#include "Render/Geometry/GeometryGenerator.h"
#include "Render/Material.h"
#include "Render/Renderer.h"
#include "Render/RenderCommand.h"
#include "Render/RenderObject.h"
#include "Render/RenderPacket.h"
#include "Render/RenderSettings.h"
#include "Render/Shader.h"
#include "Render/RenderGraph/ResourcesBlackboard.h"
#include "Render/RenderGraph/ResourceTable.h"

namespace Kioto::Renderer
{
ShadowMapRenderPass::ShadowMapRenderPass()
    : RenderPass("ShadowMap")
{
    Renderer::RegisterRenderPass(this);

    SetRenderTargetCount(1);
    m_debugQuad = GeometryGenerator::GetFullscreenQuad();
}

bool ShadowMapRenderPass::ConfigureInputsAndOutputs(ResourcesBlackboard& resources)
{
    TextureDescriptor desc;
    desc.Dimension = eResourceDim::Texture2D;
    desc.Format = eResourceFormat::Format_R8G8B8A8_UNORM;
    desc.Flags = eResourceFlags::AllowRenderTarget;
    desc.Width = m_shadowmapSize;
    desc.Height = m_shadowmapSize;
    desc.InitialState = eResourceState::Common;
    desc.FastClear = true;
    desc.FastClearValue = Color{ 0.0f, 1.0f, 0.0f, 1.0f };
    desc.Name = "ShadowMap";

    resources.NewTexture("ShadowMap", std::move(desc));
    resources.ScheduleWrite("ShadowMap");

    return true;
}

void ShadowMapRenderPass::BuildRenderPackets(CommandList* commandList, ResourceTable& resources)
{

}

void ShadowMapRenderPass::Cleanup()
{

}

void ShadowMapRenderPass::SetRenderTargets(CommandList* commandList, ResourceTable& resources)
{

}

}