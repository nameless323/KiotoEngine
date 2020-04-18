#pragma once

#include "Math/Vector2.h"

namespace Kioto
{
    struct RenderSettings
    {
        enum class RenderModeOptions
        {
            Final,
            Wireframe,
            FinalAndWireframe
        };

        RenderModeOptions RenderMode = RenderModeOptions::Final;
        Vector2i Resolution{ 1366, 768 };
    };
}
