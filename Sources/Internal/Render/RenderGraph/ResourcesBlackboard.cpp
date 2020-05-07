#include "stdafx.h"

#include "Render/RenderGraph/ResourcesBlackboard.h"

#include "Render/Renderer.h"

namespace Kioto::Renderer
{

ResourcesBlackboard::ResourcesBlackboard()
{
    m_creationRequests.reserve(16);
    m_transitionRequests.reserve(128);
}

ResourcesBlackboard::~ResourcesBlackboard()
{
}

void ResourcesBlackboard::NewTexture(std::string name, TextureDescriptor desc)
{
    m_creationRequests.push_back({ name, desc });
}

void ResourcesBlackboard::ScheduleRead(std::string name)
{
    m_transitionRequests.push_back({ name, eResourceState::PixelShaderResource });
}

void ResourcesBlackboard::ScheduleWrite(std::string name)
{
    m_transitionRequests.push_back({ name, eResourceState::RenderTarget });
}

void ResourcesBlackboard::ScheduleUnorderedAccess(std::string name)
{
    m_transitionRequests.push_back({ name, eResourceState::UnorderedAccess });
}

void ResourcesBlackboard::Clear()
{
    m_creationRequests.clear();
    m_transitionRequests.clear();
}

}