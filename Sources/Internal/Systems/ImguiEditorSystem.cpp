#include "stdafx.h"

#include "Systems/ImguiEditorSystem.h"

#include "Core/ECS/Entity.h"

#include "Component/TransformComponent.h"
#include "Component/LightComponent.h"
#include "Component/RenderComponent.h"

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

    RenderComponent* renderComponent = selectedEntity->GetComponent<RenderComponent>();
    if (renderComponent != nullptr && ImGui::CollapsingHeader("Render", ImGuiTreeNodeFlags_DefaultOpen))
    {
        bool isEnabled = renderComponent->GetIsEnabled();
        ImGui::Checkbox("Enabled", &isEnabled);
        renderComponent->SetIsEnabled(isEnabled);

        ImGui::Text("");
        ImGui::TextColored(ImVec4(0.3f, 0.6f, 0.4f, 1), "Mesh: "); ImGui::SameLine();
        ImGui::Text(renderComponent->GetMesh().c_str());
        ImGui::TextColored(ImVec4(0.3f, 0.6f, 0.4f, 1), "Material: "); ImGui::SameLine();
        ImGui::Text(renderComponent->GetMaterial().c_str());
    }

    LightComponent* lightComponent = selectedEntity->GetComponent<LightComponent>();
    if (lightComponent != nullptr && ImGui::CollapsingHeader("Light", ImGuiTreeNodeFlags_DefaultOpen))
    {
        bool isEnabled = lightComponent->GetIsEnabled();
        ImGui::Checkbox("Enabled", &isEnabled);
        lightComponent->SetIsEnabled(isEnabled);
        ImGui::Text("");

        Renderer::Light* light = lightComponent->GetLight();
        Renderer::Color col = light->Color;
        ImGui::ColorPicker4("Light Color", col.data);
        light->Color = col;

        ImGui::Text("");

        const char* lightTypeNames[] = { "Directional", "Point", "Spot" };
        static int selectedType = 0;
        selectedType = (int)light->LightType;
        ImGui::Combo("Light Type", &selectedType, lightTypeNames, IM_ARRAYSIZE(lightTypeNames));
        light->LightType = Renderer::eLightType(selectedType);

        if (light->LightType == Renderer::eLightType::Directional)
        {
            ImGui::InputFloat3("Direction", light->Direction.data);
            light->Direction.Normalize();
        }
        else if (light->LightType == Renderer::eLightType::Point)
        {
            float32& radis = light->Data.w;
            ImGui::InputFloat("Radius", &radis);
            ImGui::InputFloat3("Attenuation", light->Data.data);
        }
        else if (light->LightType == Renderer::eLightType::Spot)
        {
            ImGui::InputFloat3("Direction", light->Direction.data);
            light->Direction.Normalize();
            float32 innerRad = light->Data.x;
            float32 outerRad = light->Data.y;
            innerRad = Math::RadToDeg(innerRad);
            outerRad = Math::RadToDeg(outerRad);
            ImGui::InputFloat("Inner radius", &innerRad);
            ImGui::InputFloat("Outer radius", &outerRad);
            light->Data.x = Math::DegToRad(innerRad);
            light->Data.y = Math::DegToRad(outerRad);
        }
    }

    ImGui::End();
}

}