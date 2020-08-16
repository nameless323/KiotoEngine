#include "stdafx.h"

#include "Systems/DebugSystem.h"

#include "Core/KiotoEngine.h"
#include "Render/RenderOptions.h"

#include "IMGUI/imgui.h"

namespace Kioto
{
    DebugSystem::DebugSystem()
    {
    }

    DebugSystem::~DebugSystem()
    {
    }

    void DebugSystem::Init()
    {
    }

    void DebugSystem::OnEntityAdd(Entity* entity)
    {
    }

    void DebugSystem::OnEntityRemove(Entity* entity)
    {
    }

    void DebugSystem::Update(float32 dt)
    {
        static std::array<std::string, 3> items = { "Final", "Wireframe", "FinalAndWireframe" };
        static std::string current_item = items[0];
        const static std::array<RenderOptions::RenderModeOptions, 3> renderModeOptions{ RenderOptions::RenderModeOptions::Final,RenderOptions::RenderModeOptions::Wireframe, RenderOptions::RenderModeOptions::FinalAndWireframe };

        ImGui::Begin("Render options || DebugSystem.cpp::Update(float dt)", NULL, ImGuiWindowFlags_NoFocusOnAppearing);

        ImGui::Text("Render mode");
        ImGui::SameLine();
        if (ImGui::BeginCombo("   ", current_item.c_str())) // The second parameter is the label previewed before opening the combo.
        {
            for (size_t n = 0; n < items.size(); n++)
            {
                bool is_selected = (current_item == items[n]); // You can store your selection however you want, outside or inside your objects
                if (ImGui::Selectable(items[n].c_str(), is_selected))
                {
                    current_item = items[n];
                    KiotoCore::GetRenderSettings().RenderMode = renderModeOptions[n];
                }
                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }

        ImGui::End();
    }

    void DebugSystem::Shutdown()
    {

    }
}