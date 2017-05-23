//
// Copyright (C) Alexandr Vorontsov. 2017
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#include "stdafx.h"

#include "Core\KiotoEngine.h"

#include <sstream>

namespace Kioto
{

void KiotoMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int nCmdShow)
{
    std::stringstream ss;
    ss << "In main in dll " << hInstance << " || " << prevInstance << " || " << cmdLine << " || " << nCmdShow << std::endl;
    OutputDebugStringA(ss.str().c_str());
}

}