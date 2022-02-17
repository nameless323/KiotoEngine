//
// Copyright (C) Aleksandr Vorontcov. 2017
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#pragma once

#include "Core/Core.h"
#include "Core/CoreTypes.h"

#include <vector>

namespace YAML
{
class Node;
class Emitter;
}

namespace Kioto
{
class SceneSystem;
class Entity;
class CameraSystem;
class RenderSystem;
class EventSystem;
class LightSystem;

class Scene
{
public:
    KIOTO_API Scene(std::string name);
    Scene(const Scene& scene) = delete;
    Scene& operator=(Scene scene) = delete;
    KIOTO_API virtual ~Scene();

    ///
    /// Init scene.
    ///
    KIOTO_API virtual void Init();

    ///
    /// Update scene.
    ///
    KIOTO_API virtual void Update(float32 dt);

    ///
    /// Shutdown scene.
    ///
    KIOTO_API virtual void Shutdown();

    ///
    /// Add system to the end of systems list.
    ///
    KIOTO_API void AddSystem(SceneSystem* system);

    ///
    /// Insert system before the system of type T in systems list. Returns success status.
    ///
    template <typename T, typename = std::enable_if_t<std::is_convertible_v<T*, SceneSystem*>>>
    bool AddSystemBefore(SceneSystem* system);

    ///
    /// Insert system after the system of type T in systems list. Returns success status.
    ///
    template <typename T, typename = std::enable_if_t<std::is_convertible_v<T*, SceneSystem*>>>
    bool AddSystemAfter(SceneSystem* system);

    ///
    /// Remove system of type T from systems list.
    ///
    template <typename T, typename = std::enable_if_t<std::is_convertible_v<T*, SceneSystem*>>>
    void RemoveSystem();

    ///
    /// Remove system from systems list.
    ///
    KIOTO_API void RemoveSystem(SceneSystem* system);

    ///
    /// Finds system of type T in systems list. Returns success status.
    ///
    template <typename T, typename U, typename = 
        std::enable_if_t<
            std::is_convertible_v<T*, SceneSystem*> 
            && std::is_convertible_v<U*, SceneSystem*>
                        >
            >
    bool FindSystem(U* resultSystem) const;

    ///
    /// Get all scene systems.
    ///
    const std::vector<SceneSystem*>& GetSystems() const;

    KIOTO_API void AddEntity(Entity* entity);
    KIOTO_API void RemoveEntity(Entity* entity);
    KIOTO_API Entity* FindEntity(const std::string& name) const;

    KIOTO_API const CameraSystem* GetCameraSystem() const;

    void Serialize(YAML::Emitter& out) const;
    void Deserialize(const YAML::Node& in);

private:
    void AddSystemInternal(SceneSystem* system);

    std::vector<SceneSystem*> mSystems; // [a_vorontcov] TODO: linked list in custom arena? Also, maybe updatable system or smth like that, to not call useless update.
    std::vector<Entity*> mEntities; // [a_vorontcov] Same as above.
    CameraSystem* mCameraSystem = nullptr;
    RenderSystem* mRenderSystem = nullptr;

    std::string mName = "";
};

template <typename T, typename>
bool Scene::AddSystemBefore(SceneSystem* system)
{
    std::vector<SceneSystem*>::const_iterator* it = nullptr;
    bool success = FindSystem<T>(it);
    if (success)
        mSystems.insert(it, system);
    return success;
}

template <typename T, typename>
bool Scene::AddSystemAfter(SceneSystem* system)
{
    std::vector<SceneSystem*>::const_iterator* it = nullptr;
    bool success = FindSystem<T>(it);
    if (success)
        mSystems.insert(it + 1, system);
    return success;
}

template <typename T, typename>
void Scene::RemoveSystem()
{
    std::vector<SceneSystem*>::const_iterator* it = nullptr;
    bool success = FindSystem<T>(it);
    if (success)
    {
        delete &(*it);
        mSystems.erase(it);
    }
}

template <typename T, typename U, typename>
bool Scene::FindSystem(U* resultSystem) const
{
    auto it = std::find_if(mSystems.begin(), mSystems.end(),
        [](const SceneSystem* system)
        {
            return static_cast<T*>(system) != nullptr;
        });
    if (it != mSystems.end())
    {
        resultSystem = it;
        return true;
    }
    return false;
}

inline const std::vector<SceneSystem*>& Scene::GetSystems() const
{
    return mSystems;
}

inline const CameraSystem* Scene::GetCameraSystem() const
{
    return mCameraSystem;
}
}
