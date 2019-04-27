//
// Copyright (C) Aleksandr Vorontcov. 2019
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#include "stdafx.h"

#include "Core/Input/Input.h"

namespace Kioto
{
std::array<bool, Input::MAX_INPUT_ARRAY_SIZE> Input::m_thisFrameInput = {}; // [a_vorontcov] Up - false, down - true.
std::array<bool, Input::MAX_INPUT_ARRAY_SIZE> Input::m_prevFrameInput = {};
std::array<bool, Input::MAX_INPUT_ARRAY_SIZE> Input::m_prevPrevFrameInput = {};

std::array<bool, Input::MAX_MOUSE_ARRAY_SIZE> Input::m_thisFrameMouse = {};
std::array<bool, Input::MAX_MOUSE_ARRAY_SIZE> Input::m_prevFrameMouse = {};
std::array<bool, Input::MAX_MOUSE_ARRAY_SIZE> Input::m_prevPrevFrameMouse = {};

Vector2 Input::m_thisFrameMousePosAbsolute = Vector2(0.0f, 0.0f);
Vector2 Input::m_thisFrameMousePosRelative = Vector2(0.0f, 0.0f);
Vector2 Input::m_mouseRelative = Vector2(0.0f, 0.0f);
Vector2 Input::m_mouseAbsolute = Vector2(0.0f, 0.0f);

uint32 Input::m_thisFrameMouseWheel = 0;
uint32 Input::m_mouseWheel = 0;

void Input::SetButtonUp(uint32 keyCode)
{
    m_thisFrameInput[keyCode] = false;
}

void Input::SetButtonDown(uint32 keyCode)
{
    m_thisFrameInput[keyCode] = true;
}

void Input::SetMouseMoveRelated(uint32 x, uint32 y)
{
    m_thisFrameMousePosRelative = Vector2(static_cast<float32>(x), static_cast<float32>(y));
    std::stringstream s;
    s << "relative: " << x << "  " << y << std::endl;
}

void Input::SetMouseMoveAbsolute(uint32 x, uint32 y)
{
    m_thisFrameMousePosAbsolute = Vector2(static_cast<float32>(x), static_cast<float32>(y));
    std::stringstream s;
    s << "relative: " << x << "  " << y << std::endl;
}

void Input::SetMouseWheel(uint32 v)
{
    m_thisFrameMouseWheel = v;
    //OutputDebugStringA(std::to_string(v).c_str());
    //OutputDebugStringA("\n");
    //OutputDebugStringA("c:\\repos\\kiotoengine\\sources\\internal\\core\\input\\input.cpp(57) : super\n");
    //OutputDebugStringA(__FILE__);
}

void Input::SetMouseFlags(uint32 flags)
{
    if (flags & RI_MOUSE_LEFT_BUTTON_DOWN)
        m_thisFrameMouse[static_cast<uint32>(eMouseFlags::MouseLeft)] = true;
    if (flags & RI_MOUSE_LEFT_BUTTON_UP)
        m_thisFrameMouse[static_cast<uint32>(eMouseFlags::MouseLeft)] = false;
    if (flags & RI_MOUSE_RIGHT_BUTTON_DOWN)
        m_thisFrameMouse[static_cast<uint32>(eMouseFlags::MouseRight)] = true;
    if (flags & RI_MOUSE_RIGHT_BUTTON_UP)
        m_thisFrameMouse[static_cast<uint32>(eMouseFlags::MouseRight)] = false;
    if (flags & RI_MOUSE_MIDDLE_BUTTON_DOWN)
        m_thisFrameMouse[static_cast<uint32>(eMouseFlags::MouseMiddle)] = true;
    if (flags & RI_MOUSE_MIDDLE_BUTTON_UP)
        m_thisFrameMouse[static_cast<uint32>(eMouseFlags::MouseMiddle)] = false;
}

void Input::Update()
{
    const uint32 sizeInBytes = static_cast<uint32>(m_prevFrameInput.size() * sizeof(m_prevFrameInput[0]));
    memcpy(m_prevPrevFrameInput.data(), m_prevFrameInput.data(), sizeInBytes);
    memcpy(m_prevFrameInput.data(), m_thisFrameInput.data(), sizeInBytes);
    memset(m_thisFrameInput.data(), 0, sizeInBytes);
    m_mouseAbsolute = m_thisFrameMousePosAbsolute;
    m_mouseRelative = m_thisFrameMousePosRelative;
    m_thisFrameMousePosAbsolute = Vector2(0.0f, 0.0f);
    m_thisFrameMousePosAbsolute = Vector2(0.0f, 0.0f);
    m_mouseWheel = m_thisFrameMouseWheel;
    m_thisFrameMouseWheel = 0;
}

bool Input::GetButtonUp(eKeyCode keyCode)
{
    return !m_prevFrameInput[static_cast<uint32>(keyCode)];
}

bool Input::GetButtonDown(eKeyCode keyCode)
{
    return m_prevFrameInput[static_cast<uint32>(keyCode)];
}

bool Input::GetIsButtonHeldDown(eKeyCode keyCode)
{
    const uint32 index = static_cast<uint32>(keyCode);
    return m_prevFrameInput[index] && m_prevPrevFrameInput[index];
}

bool Input::GetButtonPressed(eKeyCode keyCode)
{
    const uint32 index = static_cast<uint32>(keyCode);
    return !m_prevFrameInput[index] && m_prevPrevFrameInput[index];
}
}