#pragma once

#include <map>
#include <functional>

#include "Core/Core.h"
#include "Core/CoreTypes.h"

namespace Kioto
{
class Component;

class ComponentFactory
{
public:
    KIOTO_API static ComponentFactory& Instance()
    {
        static ComponentFactory i;
        return i;
    }

    Component* CreateComponent(uint64 id)
    {
        if (!mComponentsMap.contains(id))
            return nullptr;
        return mComponentsMap[id]();
    }

private:
    std::map<uint64, std::function<Component*()>> mComponentsMap;

    template<typename T>
    friend class ComponentRegistrator;
};

template <typename T>
class ComponentRegistrator
{
public:
    ComponentRegistrator()
    {
        uint64 i = T::GetTypeS();
        ComponentFactory::Instance().mComponentsMap.insert(std::make_pair(T::GetTypeS(), &Create));
    }

private:
    static T* Create()
    {
        return new T;
    }
};

#define REGISTER_COMPONENT(ComponentName) \
__declspec(selectany) ComponentRegistrator<ComponentName> m_componentRegistrator_##ComponentName {}
}
