#include "Core/Input/Input.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers.
#endif

#include <Windows.h>

namespace Kioto
{
std::array<bool, Input::MAX_INPUT_ARRAY_SIZE> Input::m_thisFrameInput = {}; // [a_vorontcov] Up - false, down - true.
std::array<bool, Input::MAX_INPUT_ARRAY_SIZE> Input::m_prevFrameInput = {};
std::array<bool, Input::MAX_INPUT_ARRAY_SIZE> Input::m_prevPrevFrameInput = {};

std::array<bool, Input::MAX_MOUSE_ARRAY_SIZE> Input::m_thisFrameMouse = {};
std::array<bool, Input::MAX_MOUSE_ARRAY_SIZE> Input::m_prevFrameMouse = {};
std::array<bool, Input::MAX_MOUSE_ARRAY_SIZE> Input::m_prevPrevFrameMouse = {};

Vector2i Input::mThisFrameMousePosRelative = Vector2i(0, 0);
Vector2i Input::mMouseRelative = Vector2i(0, 0);

int32 Input::mThisFrameMouseWheel = 0;
int32 Input::mMouseWheel = 0;

bool Input::GetButtonUp(eKeyCode keyCode)
{
    const uint32 index = static_cast<uint32>(keyCode);
    return !m_prevFrameInput[index] && m_prevPrevFrameInput[index];
}

bool Input::GetButtonDown(eKeyCode keyCode)
{
    const uint32 index = static_cast<uint32>(keyCode);
    return m_prevFrameInput[index] && !m_prevPrevFrameInput[index];
}

bool Input::GetIsButtonHeldDown(eKeyCode keyCode)
{
    return m_prevFrameInput[static_cast<uint32>(keyCode)];
}

bool Input::GetMouseUp(eMouseCodes keyCode)
{
    const uint32 index = static_cast<uint32>(keyCode);
    return !m_prevFrameMouse[index] && m_prevPrevFrameMouse[index];
}

bool Input::GetMouseDown(eMouseCodes keyCode)
{
    const uint32 index = static_cast<uint32>(keyCode);
    return m_prevFrameMouse[index] && !m_prevPrevFrameMouse[index];
}

bool Input::GetMouseHeldDown(eMouseCodes keyCode)
{
    return m_prevFrameMouse[static_cast<uint32>(keyCode)];
}

eMouseWheelScroll Input::GetMouseWheel()
{
    return static_cast<eMouseWheelScroll>(mMouseWheel);
}

Vector2i Input::GetMouseRelativePosition()
{
    return mMouseRelative;
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
    mThisFrameMousePosRelative = Vector2i(x, y);
}

void Input::SetMouseWheel(int32 v)
{
    mThisFrameMouseWheel = v;
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

    const uint32 sizeInBytesMouse = static_cast<uint32>(m_thisFrameMouse.size() * sizeof(m_thisFrameMouse[0]));
    memcpy(m_prevPrevFrameMouse.data(), m_prevFrameMouse.data(), sizeInBytesMouse);
    memcpy(m_prevFrameMouse.data(), m_thisFrameMouse.data(), sizeInBytesMouse);

    mMouseRelative = mThisFrameMousePosRelative;
    mThisFrameMousePosRelative = Vector2i::Zero;
    mMouseWheel = mThisFrameMouseWheel;
    mThisFrameMouseWheel = 0;
}
}
