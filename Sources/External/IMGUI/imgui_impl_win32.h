// dear imgui: Platform Binding for Windows (standard windows API for 32 and 64 bits applications)
// This needs to be used along with a Renderer (e.g. DirectX11, OpenGL3, Vulkan..)

// Implemented features:
//  [X] Platform: Clipboard support (for Win32 this is actually part of core imgui)
//  [X] Platform: Mouse cursor shape and visibility. Disable with 'io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange'.
//  [X] Platform: Keyboard arrays indexed using VK_* Virtual Key Codes, e.g. ImGui::IsKeyPressed(VK_SPACE).
//  [X] Platform: Gamepad support. Enabled with 'io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad'.

#pragma once

#include "Sources/External/IMGUI/imgui.h"

namespace ImGui
{
IMGUI_IMPL_API bool     ImplWinInit(void* hwnd);
IMGUI_IMPL_API void     ImplWinShutdown();
IMGUI_IMPL_API void     ImplWinNewFrame();

// Handler for Win32 messages, update mouse/keyboard data.
// You may or not need this for your implementation, but it can serve as reference for handling inputs.
// Intentionally commented out to avoid dragging dependencies on <windows.h> types. You can COPY this line into your .cpp code instead.
/*
IMGUI_IMPL_API LRESULT  ImplWinWndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
*/

IMGUI_IMPL_API LRESULT  ImplWinWndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
}
