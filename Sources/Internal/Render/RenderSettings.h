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
        Vector2i Resolution{ 1920, 1080 };

        static constexpr uint32 MaxRenderPassesCount = 128;
        static constexpr uint32 MaxRenderCommandsCount = 2048;
    };
}
