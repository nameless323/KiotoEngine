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

Vector2i Input::m_thisFrameMousePosRelative = Vector2i(0, 0);
Vector2i Input::m_mouseRelative = Vector2i(0, 0);

int32 Input::m_thisFrameMouseWheel = 0;
int32 Input::m_mouseWheel = 0;

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

bool Input::GetMouseUp(eMouseCodes keyCode)
{
    return !m_prevFrameMouse[static_cast<uint32>(keyCode)];
}

bool Input::GetMouseDown(eMouseCodes keyCode)
{
    return m_prevFrameMouse[static_cast<uint32>(keyCode)];
}

bool Input::GetIsMouseHeldDown(eMouseCodes keyCode)
{
    const uint32 index = static_cast<uint32>(keyCode);
    return m_prevFrameMouse[index] && m_prevPrevFrameMouse[index];
}

bool Input::GetMousePressed(eMouseCodes keyCode)
{
    const uint32 index = static_cast<uint32>(keyCode);
    return !m_prevFrameMouse[index] && m_prevPrevFrameMouse[index];
}

eMouseWheelScroll Input::GetMouseWheel()
{
    return static_cast<eMouseWheelScroll>(m_mouseWheel);
}

Vector2i Input::GetMouseRelativePosition()
{
    return m_mouseRelative;
}

void Input::SetButtonUp(uint32 keyCode)
{
    m_thisFrameInput[keyCode] = false;
}

void Input::SetButtonDown(uint32 keyCode)
{
    m_thisFrameInput[keyCode] = true;
}

void Input::SetMouseMoveRelated(int32 x, int32 y)
{
    m_thisFrameMousePosRelative = Vector2i(x, y);
}

void Input::SetMouseWheel(int32 v)
{
    m_thisFrameMouseWheel = v;
}

void Input::SetMouseFlags(uint32 flags)
{
    if (flags & RI_MOUSE_LEFT_BUTTON_DOWN)
        m_thisFrameMouse[static_cast<uint32>(eMouseCodes::MouseLeft)] = true;
    if (flags & RI_MOUSE_LEFT_BUTTON_UP)
        m_thisFrameMouse[static_cast<uint32>(eMouseCodes::MouseLeft)] = false;
    if (flags & RI_MOUSE_RIGHT_BUTTON_DOWN)
        m_thisFrameMouse[static_cast<uint32>(eMouseCodes::MouseRight)] = true;
    if (flags & RI_MOUSE_RIGHT_BUTTON_UP)
        m_thisFrameMouse[static_cast<uint32>(eMouseCodes::MouseRight)] = false;
    if (flags & RI_MOUSE_MIDDLE_BUTTON_DOWN)
        m_thisFrameMouse[static_cast<uint32>(eMouseCodes::MouseMiddle)] = true;
    if (flags & RI_MOUSE_MIDDLE_BUTTON_UP)
        m_thisFrameMouse[static_cast<uint32>(eMouseCodes::MouseMiddle)] = false;
}

void Input::Update()
{
    const uint32 sizeInBytes = static_cast<uint32>(m_prevFrameInput.size() * sizeof(m_prevFrameInput[0]));
    memcpy(m_prevPrevFrameInput.data(), m_prevFrameInput.data(), sizeInBytes);
    memcpy(m_prevFrameInput.data(), m_thisFrameInput.data(), sizeInBytes);
    memset(m_thisFrameInput.data(), 0, sizeInBytes);

    const uint32 sizeInBytesMouse = static_cast<uint32>(m_thisFrameMouse.size() * sizeof(m_thisFrameMouse[0]));
    memcpy(m_prevPrevFrameMouse.data(), m_prevFrameMouse.data(), sizeInBytesMouse);
    memcpy(m_prevFrameMouse.data(), m_thisFrameMouse.data(), sizeInBytesMouse);
    memset(m_thisFrameMouse.data(), 0, sizeInBytesMouse);

    m_mouseRelative = m_thisFrameMousePosRelative;
    m_thisFrameMousePosRelative = Vector2i::Zero;
    m_mouseWheel = m_thisFrameMouseWheel;
    m_thisFrameMouseWheel = 0;
}
}
