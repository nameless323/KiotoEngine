#pragma once

#include <unordered_map>
#include <string>

#include "Render/Texture/Texture.h"
#include "Render/ResourceStates.h"

namespace Kioto::Renderer
{
struct ResourceCreationRequest
{
    std::string ResourceName;
    TextureDescriptor Desc;
};
struct ResourceTransitionRequest
{
    std::string ResourceName;
    eResourceState TransitionTo;
};

class ResourcesBlackboard
{
public:
    ResourcesBlackboard();
    ~ResourcesBlackboard();

    void NewTexture(std::string name, TextureDescriptor desc);

    void ScheduleRead(std::string name);
    void ScheduleWrite(std::string name);
    void ScheduleWriteDS(std::string name);
    void ScheduleUnorderedAccess(std::string name);
    const std::vector<ResourceTransitionRequest>& GetTransitionRequests() const;
    const std::vector<ResourceCreationRequest>& GetCreationRequest() const;
    void Clear();

private:
    std::vector<ResourceCreationRequest> mCreationRequests;
    std::vector<ResourceTransitionRequest> mTransitionRequests;
};

inline const std::vector<ResourceTransitionRequest>& ResourcesBlackboard::GetTransitionRequests() const
{
    return mTransitionRequests;
}

inline const std::vector<ResourceCreationRequest>& ResourcesBlackboard::GetCreationRequest() const
{
    return mCreationRequests;
}
}