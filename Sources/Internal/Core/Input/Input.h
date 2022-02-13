#pragma once

#include <array>

#include "Core/CoreTypes.h"
#include "Core/Core.h"
#include "Math/Vector2.h"

namespace Kioto
{
enum class eKeyCode
{
    KeyCodeBackspace = 0x08,
    KeyCodeTab = 0x09,
    KeyCodeEnter = 0x0D,
    KeyCodeShift = 0x10,
    KeyCodeControl = 0x11,
    KeyCodeEscape = 0x1B,
    KeyCodeSpace = 0x20,
    KeyCodePageUp = 0x21,
    KeyCodePageDown = 0x22,
    KeyCodeEnd = 0x23,
    KeyCodeHome = 0x24,
    KeyCodeLeft = 0x25,
    KeyCodeUp = 0x26,
    KeyCodeRight = 0x27,
    KeyCodeDown = 0x28,
    KeyCodeInsert = 0x2D,
    KeyCodeDelete = 0x2E,
    KeyCode0 = 0x30,
    KeyCode1 = 0x31,
    KeyCode2 = 0x32, 
    KeyCode3 = 0x33,
    KeyCode4 = 0x34,
    KeyCode5 = 0x35,
    KeyCode6 = 0x36,
    KeyCode7 = 0x37,
    KeyCode8 = 0x38,
    KeyCode9 = 0x39,
    KeyCodeA = 0x41,
    KeyCodeB = 0x42,
    KeyCodeC = 0x43,
    KeyCodeD = 0x44,
    KeyCodeE = 0x45,
    KeyCodeF = 0x46,
    KeyCodeG = 0x47,
    KeyCodeH = 0x48,
    KeyCodeI = 0x49,
    KeyCodeJ = 0x4A,
    KeyCodeK = 0x4B,
    KeyCodeL = 0x4C,
    KeyCodeM = 0x4D,
    KeyCodeN = 0x4E,
    KeyCodeO = 0x4F,
    KeyCodeP = 0x50,
    KeyCodeQ = 0x51,
    KeyCodeR = 0x52,
    KeyCodeS = 0x53,
    KeyCodeT = 0x54,
    KeyCodeU = 0x55,
    KeyCodeV = 0x56,
    KeyCodeW = 0x57,
    KeyCodeX = 0x58,
    KeyCodeY = 0x59,
    KeyCodeZ = 0x5A,
    KeyCodeNum0 = 0x60,
    KeyCodeNum1 = 0x61,
    KeyCodeNum2 = 0x62,
    KeyCodeNum3 = 0x63,
    KeyCodeNum4 = 0x64,
    KeyCodeNum5 = 0x65,
    KeyCodeNum6 = 0x66,
    KeyCodeNum7 = 0x67,
    KeyCodeNum8 = 0x68,
    KeyCodeNum9 = 0x69,
    KeyCodeMul = 0x6A,
    KeyCodeAdd = 0x6B,
    KeyCodeSep = 0x6C,
    KeyCodeSubtr = 0x6D,
    KeyCodeDec = 0x6E,
    KeyCodeDiv = 0x6F,
    KeyCodeF1 = 0x70,
    KeyCodeF2 = 0x71,
    KeyCodeF3 = 0x72,
    KeyCodeF4 = 0x73,
    KeyCodeF5 = 0x74,
    KeyCodeF6 = 0x75,
    KeyCodeF7 = 0x76,
    KeyCodeF8 = 0x77,
    KeyCodeF9 = 0x78,
    KeyCodeF10 = 0x79,
    KeyCodeF11 = 0x7A,
    KeyCodeF12 = 0x7B,
    KeyCodeNumLock = 0x90,
    KeyCodeScrollLock = 0x91,
    KeyCodeLShift = 0xA0,
    KeyCodeRShift = 0xA1,
    KeyCodeLCtrl = 0xA2,
    KeyCodeRCtrl = 0xA3,
    Count
};

enum class eMouseCodes
{
    MouseLeft = 0,
    MouseRight = 1,
    MouseMiddle = 2,
    Count
};

enum class eMouseWheelScroll : uint32
{
    ScrollUp = 120,
    ScrollDown = 65416, // [a_vorontcov] Emperical way.
    ScrollNone = 0
};

class Input
{
public:
    KIOTO_API static bool GetButtonUp(eKeyCode keyCode);
    KIOTO_API static bool GetButtonDown(eKeyCode keyCode);
    KIOTO_API static bool GetIsButtonHeldDown(eKeyCode keyCode);

    KIOTO_API static bool GetMouseUp(eMouseCodes keyCode);
    KIOTO_API static bool GetMouseDown(eMouseCodes keyCode);
    KIOTO_API static bool GetMouseHeldDown(eMouseCodes keyCode);

    KIOTO_API static eMouseWheelScroll GetMouseWheel();
    KIOTO_API static Vector2i GetMouseRelativePosition();

    static void SetButtonUp(uint32 keyCode);
    static void SetButtonDown(uint32 keyCode);
    static void SetMouseMoveRelated(int32 x, int32 y);
    static void SetMouseWheel(int32 v);
    static void SetMouseFlags(uint32 flags);
    static void Update();

private:
    static constexpr uint32 MAX_INPUT_ARRAY_SIZE = 256; // [a_vorontcov] Yep, bit of wasting memory but whatever.
    static constexpr uint32 MAX_MOUSE_ARRAY_SIZE = 3;

    static std::array<bool, MAX_INPUT_ARRAY_SIZE> m_thisFrameInput; // [a_vorontcov] Up - false, down - true.
    static std::array<bool, MAX_INPUT_ARRAY_SIZE> m_prevFrameInput;
    static std::array<bool, MAX_INPUT_ARRAY_SIZE> m_prevPrevFrameInput;

    static std::array<bool, MAX_MOUSE_ARRAY_SIZE> m_thisFrameMouse; // [a_vorontcov] Up - false, down - true.
    static std::array<bool, MAX_MOUSE_ARRAY_SIZE> m_prevFrameMouse;
    static std::array<bool, MAX_MOUSE_ARRAY_SIZE> m_prevPrevFrameMouse;

    static Vector2i mThisFrameMousePosRelative;
    static Vector2i mMouseRelative;
    static int32 mThisFrameMouseWheel;
    static int32 mMouseWheel;
};
}