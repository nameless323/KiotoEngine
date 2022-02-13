#pragma once

#include "Core/CoreTypes.h"
#include "Core/Core.h"
#include "Core/ECS/ComponentFactory.h"

#include <functional>

namespace YAML
{
class Node;
class Emitter;
}

namespace Kioto
{
class Entity;

#define DECLARE_COMPONENT(type) \
public:\
KIOTO_API uint64 GetType() const override \
{ \
    return type::GetTypeS(); \
} \
KIOTO_API static uint64 GetTypeS() \
{ \
    static std::hash<std::string> stringHasher; \
    static uint64 hash = stringHasher(#type); \
    return hash; \
} \
KIOTO_API const std::string& GetTypeName() const override \
{ \
    static std::string name = type::GetTypeNameS(); \
    return name; \
} \
KIOTO_API static const std::string& GetTypeNameS() \
{ \
    static std::string name = #type; \
    return name; \
}

class Component
{
public:
    KIOTO_API Component() = default;
    KIOTO_API virtual ~Component() = default;

    KIOTO_API Entity* GetEntity() const;
    KIOTO_API virtual Component* Clone() const abstract;
    KIOTO_API virtual uint64 GetType() const;
    KIOTO_API virtual const std::string& GetTypeName() const;

    KIOTO_API virtual bool GetIsEnabled() const;
    KIOTO_API virtual void SetIsEnabled(bool enabled);

    virtual void Serialize(YAML::Emitter& out) const abstract;
    virtual void Deserialize(const YAML::Node& in) abstract;

protected:
    virtual void SetEntity(Entity* entity);

private:
    Entity* mEntity = nullptr;
    bool mIsEnabled = true;

    friend class Entity;
};

inline Entity* Component::GetEntity() const
{
    return mEntity;
}

inline uint64 Component::GetType() const
{
    return -1;
}

inline const std::string& Component::GetTypeName() const
{
    return this->GetTypeName();
}

inline bool Component::GetIsEnabled() const
{
    return mIsEnabled;
}

inline void Component::SetIsEnabled(bool enabled)
{
    mIsEnabled = enabled;
}
}
