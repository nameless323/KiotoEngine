//
// Copyright (C) Aleksandr Vorontcov. 2017
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

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
        if (m_componentsMap.find(id) == m_componentsMap.end())
            return nullptr;
        return m_componentsMap[id]();
    }

private:
    std::map<uint64, std::function<Component*()>> m_componentsMap;

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
        ComponentFactory::Instance().m_componentsMap.insert(std::make_pair(T::GetTypeS(), &Create));
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
