//
// Copyright (C) Alexandr Vorontsov. 2017
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#pragma once

#define WIN32_LEAN_AND_MEAN

#include <string>
#include <windows.h>

#include "Core/Core.h"
#include "Core/CoreTypes.h"

namespace Kioto
{

///
/// Entry point of Kioto Engine. Call this function from your WinMain.
///
KIOTO_API void KiotoMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int nCmdShow, std::wstring capture);

namespace KiotoCore
{

struct
{
    HINSTANCE HInstance = {};
    HINSTANCE PrevInstance = {};
    PSTR CmdLine = {};
    int NCmdShow = {};
    std::wstring WindowCapture;
} ApplicationInfo;

void Init();
void Update();
void Shutdown();
void ChangeFullscreenMode(bool fullScreen);
void Resize(uint16 width, uint16 height, bool minimized);

}

}