// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers.
#endif

#include <windows.h>

#include <wrl.h>
#include <dxgi1_5.h>
#include <d3d12.h>
#include <D3Dcompiler.h>
#include "Sources/External/Dx12Helpers/d3dx12.h"
#include <DirectXMath.h>
#include <DirectXColors.h>
#ifdef _DEBUG
#include <DXGIDebug.h>
#endif


// TODO: reference additional headers your program requires here
