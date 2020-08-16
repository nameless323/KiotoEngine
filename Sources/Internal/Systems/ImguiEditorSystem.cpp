#include "stdafx.h"

#include "Systems/ImguiEditorSystem.h"

#include "Core/ECS/Entity.h"

#include "Component/TransformComponent.h"
#include "Component/LightComponent.h"

#include "Render/Color.h"

#include "IMGUI/imgui.h"

namespace Kioto
{
ImguiEditorSystem::ImguiEditorSystem()
{
    m_entities.reserve(512);
}

ImguiEditorSystem::~ImguiEditorSystem()
{
    m_entities.clear();
}

void ImguiEditorSystem::OnEntityAdd(Entity* entity)
{
    auto it = std::find(m_entities.begin(), m_entities.end(), entity);
    assert(it == m_entities.end());

    m_entities.push_back(entity);
    m_entitiesNames.push_back(entity->GetName().c_str());
}

void ImguiEditorSystem::OnEntityRemove(Entity* entity)
{
    auto it = std::find(m_entities.begin(), m_entities.end(), entity);
    assert(it != m_entities.end());

    const char* name = entity->GetName().c_str();
    m_entitiesNames.erase(std::find(m_entitiesNames.begin(), m_entitiesNames.end(), name));
    m_entities.erase(it);
}

void ImguiEditorSystem::Update(float32 dt)
{
    static int selectedEntityIndex = 1;

    //ImGui::ShowDemoWindow();

    ImGui::Begin("Scene", NULL, ImGuiWindowFlags_NoFocusOnAppearing);
    ImGui::ListBox("", &selectedEntityIndex, m_entitiesNames.data(), int(m_entitiesNames.size()), 15);

    const Entity* selectedEntity = m_entities[selectedEntityIndex];

    TransformComponent* transform = selectedEntity->GetTransform();
    if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
    {
        Vector3 currPos = transform->GetWorldPosition();
        ImGui::InputFloat3("Position", currPos.data);
        transform->SetWorldPosition(currPos);

        Quaternion currRot = transform->GetWorldRotation();
        Vector3 rotEuler = Quaternion::ToEuler(currRot);
        ImGui::InputFloat3("Rotation", rotEuler.data);
        Quaternion finalRot = Quaternion::FromEuler(rotEuler.x, rotEuler.y, rotEuler.z);
        transform->SetWorldRotation(finalRot);
    }
    LightComponent* lightComponent = selectedEntity->GetComponent<LightComponent>();

    if (lightComponent != nullptr && ImGui::CollapsingHeader("Light", ImGuiTreeNodeFlags_DefaultOpen))
    {
        Renderer::Light* light = lightComponent->GetLight();
        Renderer::Color col = light->Color;
        ImGui::ColorPicker4("Light Color", col.data);
        light->Color = col;

        ImGui::Text("");

        const char* lightTypeNames[] = { "Directional", "Point", "Spot" };
        static int selectedType = 0;
        ImGui::Combo("Light Type", &selectedType, lightTypeNames, IM_ARRAYSIZE(lightTypeNames));
        light->LightType = Renderer::eLightType(selectedType);

        if (light->LightType == Renderer::eLightType::Directional)
        {
            ImGui::InputFloat3("Direction", light->Direction.data);
            light->Direction.Normalize();
        }
        if (light->LightType != Renderer::eLightType::Directional)
        {
            ImGui::InputFloat3("Attenuation", light->Attenuation.data);
        }
    }

    ImGui::End();
}

}