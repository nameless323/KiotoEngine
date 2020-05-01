#pragma once

#include <unordered_map>
#include <string>
#include "Render/Texture/Texture.h"

namespace Kioto::Renderer
{
enum class eResourceStates
{
    Read,
    Write,
    UnorderedAccess
};

class ResourcesBlackboard
{
public:
    struct ResourceCreationRequest
    {
        std::string Name;
        TextureDescriptor Desc;
    };
    struct ResourceTransitionRequest
    {
        std::string Name;
        eResourceStates TransitionTo;
    };

    ResourcesBlackboard();
    ~ResourcesBlackboard();

    void NewTexture(std::string name, TextureDescriptor desc);

    void ScheduleRead(std::string name);
    void ScheduleWrite(std::string name);
    void ScheduleUnorderedAccess(std::string name);
    void Clear();

private:
    std::vector<ResourceCreationRequest> m_creationRequests;
    std::vector<ResourceTransitionRequest> m_transitionRequests;
};
}