//
// Copyright (C) Alexandr Vorontsov. 2017
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#pragma once

#include <windows.h>

#include "Core\Core.h"

namespace Kioto
{
///
/// Entry point of Kioto Engine. Call this function from your WinMain.
///
KIOTO_API void KiotoMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int nCmdShow);
}