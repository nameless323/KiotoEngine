#include "Render/RenderGraph/ResourcesBlackboard.h"

#include "Render/Renderer.h"

namespace Kioto::Renderer
{

ResourcesBlackboard::ResourcesBlackboard()
{
    mCreationRequests.reserve(16);
    mTransitionRequests.reserve(128);
}

ResourcesBlackboard::~ResourcesBlackboard()
{
}

void ResourcesBlackboard::NewTexture(std::string name, TextureDescriptor desc)
{
    mCreationRequests.push_back({ name, desc });
}

void ResourcesBlackboard::ScheduleRead(std::string name)
{
    mTransitionRequests.push_back({ name, eResourceState::PixelShaderResource });
}

void ResourcesBlackboard::ScheduleWrite(std::string name)
{
    mTransitionRequests.push_back({ name, eResourceState::RenderTarget });
}

void ResourcesBlackboard::ScheduleWriteDS(std::string name)
{
    mTransitionRequests.push_back({ name, eResourceState::DepthWrite });
}

void ResourcesBlackboard::ScheduleUnorderedAccess(std::string name)
{
    mTransitionRequests.push_back({ name, eResourceState::UnorderedAccess });
}

void ResourcesBlackboard::Clear()
{
    mCreationRequests.clear();
    mTransitionRequests.clear();
}

}