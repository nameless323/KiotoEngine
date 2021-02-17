#include "stdafx.h"

#include "Systems/ImguiEditorSystem.h"

#include "Core/ECS/Entity.h"

#include "Component/CameraComponent.h"
#include "Component/TransformComponent.h"
#include "Component/LightComponent.h"
#include "Component/RenderComponent.h"
#include "Render/Camera.h"
#include "Render/RenderObject.h"
#include "Render/Material.h"

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

    CameraComponent* camera = selectedEntity->GetComponent<CameraComponent>();
    DrawComponentEditor(camera);

    TransformComponent* transform = selectedEntity->GetTransform();
    DrawComponentEditor(transform);

    RenderComponent* renderComponent = selectedEntity->GetComponent<RenderComponent>();
    DrawComponentEditor(renderComponent);

    LightComponent* lightComponent = selectedEntity->GetComponent<LightComponent>();
    DrawComponentEditor(lightComponent);

    ImGui::End();
}

void ImguiEditorSystem::DrawComponentEditor(LightComponent* lightComponent)
{
    if (lightComponent != nullptr && ImGui::CollapsingHeader("Light", ImGuiTreeNodeFlags_DefaultOpen))
    {
        bool isEnabled = lightComponent->GetIsEnabled();
        ImGui::Checkbox("Enabled", &isEnabled);
        lightComponent->SetIsEnabled(isEnabled);
        ImGui::Text("");

        bool castsShadows = lightComponent->GetLight()->CastShadow;
        ImGui::Checkbox("Casts shadow", &castsShadows);
        lightComponent->GetLight()->CastShadow = castsShadows;
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
}

void ImguiEditorSystem::DrawComponentEditor(RenderComponent* renderComponent)
{
    if (renderComponent != nullptr && ImGui::CollapsingHeader("Render", ImGuiTreeNodeFlags_DefaultOpen))
    {
        bool isEnabled = renderComponent->GetIsEnabled();
        ImGui::Checkbox("Enabled", &isEnabled);
        renderComponent->SetIsEnabled(isEnabled);

        ImGui::Text("");

        bool isVisible = renderComponent->GetIsVisible();
        ImGui::Checkbox("Is Visible", &isVisible);
        renderComponent->SetIsVisible(isVisible);
        bool castShadow = renderComponent->GetCastShadows();
        ImGui::Checkbox("Cast shadow", &castShadow);
        renderComponent->SetCastShadows(castShadow);

        ImGui::Text("");
        ImGui::TextColored(ImVec4(0.3f, 0.6f, 0.4f, 1), "Mesh: "); ImGui::SameLine();
        ImGui::Text(renderComponent->GetMesh().c_str());
        ImGui::TextColored(ImVec4(0.3f, 0.6f, 0.4f, 1), "Material: "); ImGui::SameLine();
        ImGui::Text(renderComponent->GetMaterial().c_str());
        auto& texDescr = renderComponent->GetRenderObject()->GetMaterial()->GetTextureAssetDescriptions();
        if (texDescr.empty())
            ImGui::TextColored(ImVec4(0.9f, 0.6f, 0.4f, 1), "\t -// No Textures Defined //-");
        else
            ImGui::TextColored(ImVec4(0.3f, 0.6f, 0.4f, 1), "Textures:");

        for (auto& decsr : texDescr)
        {
            ImGui::TextColored(ImVec4(0.3f, 0.6f, 0.4f, 1), "\tFor pass: "); ImGui::SameLine();
            ImGui::Text(decsr.first.c_str());
            if (decsr.second.empty())
                ImGui::TextColored(ImVec4(0.9f, 0.6f, 0.4f, 1), "\t\t -// No Textures Defined //-");

            for (auto& texDesc : decsr.second)
            {
                ImGui::TextColored(ImVec4(0.3f, 0.6f, 0.4f, 1), "\t\t%s", texDesc.Name.c_str()); ImGui::SameLine();
                ImGui::Text(texDesc.Path.c_str());
            }
        }
        ImGui::Text("");
        auto& cbDecr = renderComponent->GetRenderObject()->GetBuffersLayouts();
        if (cbDecr.empty())
            ImGui::TextColored(ImVec4(0.9f, 0.6f, 0.4f, 1), "\t -// No Constant Buffers Defined //-");
        else
            ImGui::TextColored(ImVec4(0.3f, 0.6f, 0.4f, 1), "Constant Buffers:");
        for (auto& decsr : cbDecr)
        {
            ImGui::TextColored(ImVec4(0.3f, 0.6f, 0.4f, 1), "\tFor pass: "); ImGui::SameLine();
            ImGui::Text(decsr.first.c_str());
            if (decsr.second.empty())
                ImGui::TextColored(ImVec4(0.9f, 0.6f, 0.4f, 1), "\t\t -// No Constant Buffers Defined //-");

            for (auto& cb : decsr.second)
            {
                ImGui::TextColored(ImVec4(0.3f, 0.6f, 0.4f, 1), "\t\t%s : ", cb.GetName().c_str());
            }
        }
    }
}

void ImguiEditorSystem::DrawComponentEditor(TransformComponent* transform)
{
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

        Vector3& right = transform->Right();
        Vector3& fwd = transform->Fwd();
        Vector3& up = transform->Up();

        ImGui::Text("Right   : % 8.4f, % 8.4f, % 8.4f", right.x, right.y, right.z);
        ImGui::Text("Forward : % 8.4f, % 8.4f, % 8.4f", fwd.x, fwd.y, fwd.z);
        ImGui::Text("Up      : % 8.4f, % 8.4f, % 8.4f", up.x, up.y, up.z);
    }
}

void ImguiEditorSystem::DrawComponentEditor(CameraComponent* cameraComponent)
{
    if (cameraComponent != nullptr && ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen))
    {
        Renderer::Camera& camera = cameraComponent->GetCamera();
        bool isOrtho = camera.GetOrthographic();
        ImGui::Checkbox("Orthographic", &isOrtho);
        camera.SetOrthographic(isOrtho);

        ImGui::Text("");

        float32 n = camera.GetNearPlane();
        float32 f = camera.GetFarPlane();

        ImGui::InputFloat("Near", &n);
        ImGui::InputFloat("Far", &f);

        camera.SetNearPlane(n);
        camera.SetFarPlane(f);

        ImGui::Text("");

        if (!isOrtho)
        {
            float32 fov = camera.GetFovY();
            fov = Math::RadToDeg(fov);
            ImGui::InputFloat("FOV", &fov);
            camera.SetFovY(Math::DegToRad(fov));

            float32 aspect = camera.GetAspect();
            ImGui::InputFloat("Aspect", &aspect);
            camera.SetAspect(aspect);
        }
        else
        {
            float32 h = camera.GetOrthoHeight();
            float32 w = camera.GetOrthoWidth();
            ImGui::InputFloat("Ortho height", &h);

            static bool useAspect = false;
            ImGui::Checkbox("Use aspect for width", &useAspect);

            if (useAspect)
            {
                float32 aspect = camera.GetAspect();
                ImGui::InputFloat("Aspect", &aspect);
                camera.SetAspect(aspect);

                w = h * aspect;
            }
            else
            {
                ImGui::InputFloat("Ortho width", &w);
            }
            camera.SetOrthoHeight(h);
            camera.SetOrthoWidth(w);
        }
    }
}

}