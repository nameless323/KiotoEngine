//
// Copyright (C) Aleksandr Vorontcov. 2019
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#include "stdafx.h"

#include "Core/Input/Input.h"

namespace Kioto
{
void Input::SetButtonUp(uint32 keyCode)
{
    m_thisFrameInput[keyCode] = false;
}

void Input::SetButtonDown(uint32 keyCode)
{
    m_thisFrameInput[keyCode] = true;
}

void Input::Update()
{
    uint32 sizeInBytes = m_prevFrameInput.size() * sizeof(m_prevFrameInput[0]);
    memcpy(m_prevPrevFrameInput.data(), m_prevFrameInput.data(), sizeInBytes);
    memcpy(m_prevFrameInput.data(), m_thisFrameInput.data(), sizeInBytes);
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
    uint32 index = static_cast<uint32>(keyCode);
    return m_prevFrameInput[index] && m_prevPrevFrameInput[index];
}

bool Input::GetButtonPressed(eKeyCode keyCode)
{
    uint32 index = static_cast<uint32>(keyCode);
    return !m_prevFrameInput[index] && m_prevPrevFrameInput[index];
}
}