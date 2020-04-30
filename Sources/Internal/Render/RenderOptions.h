#pragma once

#include "Math/Vector2.h"

namespace Kioto
{
    struct RenderOptions
    {
        enum class RenderModeOptions
        {
            Final,
            Wireframe,
            FinalAndWireframe
        };

        RenderModeOptions RenderMode = RenderModeOptions::Final;
        Vector2i Resolution{ 1366, 768 };

        static constexpr uint32 MaxRenderPassesCount = 128;
        static constexpr uint32 MaxRenderCommandsCount = 2048;
    };
}
