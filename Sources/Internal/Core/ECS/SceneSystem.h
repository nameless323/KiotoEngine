#pragma once

#include "Core/CoreTypes.h"

namespace Kioto
{
class Entity;

class SceneSystem
{
public:
    ///
    /// This method will be called when any entity will be added to scene.
    ///
    KIOTO_API virtual void OnEntityAdd(Entity* entity) abstract;
    ///
    /// This method will be called when any entity will be removed from scene.
    ///
    KIOTO_API virtual void OnEntityRemove(Entity* entity) abstract;
    ///
    /// This method will be called when system will be registered in scene.
    ///
    KIOTO_API virtual void Init();
    ///
    /// This method will be called every frame if system is updatable. dt - previous frame delta time.
    ///
    KIOTO_API virtual void Update(float32 dt) abstract;
    KIOTO_API virtual ~SceneSystem() = default;
    ///
    /// This method will be called every frame if system is unregistred from scene.
    ///
    KIOTO_API virtual void Shutdown();
    ///
    /// Get if system needs update.
    ///
    bool GetNeedUpdatable() const;

private:
    bool mUpdatable = true;
};

inline bool SceneSystem::GetNeedUpdatable() const
{
    return mUpdatable;
}

inline void SceneSystem::Init()
{
}

inline void SceneSystem::Shutdown()
{
}
}
