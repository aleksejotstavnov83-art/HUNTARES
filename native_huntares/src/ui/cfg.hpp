
// ================================================================================
// Origin: @imguisource_bot
// Community:
//  - ImGui Source: @ImGui_Source_FIXNEY
//  - shit src 🧨: @il2cpp_src
// ================================================================================

#pragma once
#include "imgui.h"

namespace cfg {

namespace esp {
    inline bool box = true;
    inline bool name = false;
    inline bool health = true;
    inline bool distance = false;
    inline int box_type = 0;
    inline float box_rounding = 0.f;
    // The initial body envelope is derived automatically from the player root.
    // Bone-based exact per-animation bounds are pending verification.
    inline constexpr float estimated_body_height = 1.67f;
    inline ImVec4 box_col = ImVec4(162/255.f, 144/255.f, 225/255.f, 1.f);
    inline ImVec4 name_col = ImVec4(1.f, 1.f, 1.f, 1.f);
    inline ImVec4 health_col = ImVec4(162/255.f, 144/255.f, 225/255.f, 1.f);
    inline ImVec4 distance_col = ImVec4(162/255.f, 144/255.f, 225/255.f, 1.f);
}

namespace overlay {
    // When true request SurfaceFlinger to exclude our layer from screenshots.
    // Rendering stays ON in the real-time display; availability is device-specific.
    inline bool capture_exclude = false;
    inline bool capture_supported = true;
}

}


