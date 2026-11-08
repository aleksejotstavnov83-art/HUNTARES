#pragma once
// In-process ImGui interface. Does not access game data when 1.1.0 profile is unverified.
#include <atomic>
#include "imgui.h"

static bool g_huntaresCrosshair = false;
static float g_huntaresReticleScale = 11.0f;

void renderMinimalMenu() {
    ImGuiIO &io = ImGui::GetIO();
    const float cx = io.DisplaySize.x * 0.5f, cy = io.DisplaySize.y * 0.5f;
    if (g_huntaresCrosshair) {
        ImDrawList* draw = ImGui::GetBackgroundDrawList();
        const ImU32 white = IM_COL32(245, 245, 245, 245);
        const float gap = g_huntaresReticleScale * .38f;
        draw->AddLine(ImVec2(cx - g_huntaresReticleScale, cy), ImVec2(cx - gap, cy), white, 1.7f);
        draw->AddLine(ImVec2(cx + gap, cy), ImVec2(cx + g_huntaresReticleScale, cy), white, 1.7f);
        draw->AddLine(ImVec2(cx, cy - g_huntaresReticleScale), ImVec2(cx, cy - gap), white, 1.7f);
        draw->AddLine(ImVec2(cx, cy + gap), ImVec2(cx, cy + g_huntaresReticleScale), white, 1.7f);
    }
    const bool opened = g_menuOpen.load(std::memory_order_relaxed);
    if (!opened) {
        ImGui::SetNextWindowPos(ImVec2(18, 18), ImGuiCond_Always);
        ImGui::SetNextWindowBgAlpha(0.55f);
        ImGui::Begin("##HuntMark", nullptr, ImGuiWindowFlags_AlwaysAutoResize |
                     ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove);
        if (ImGui::Button("HUNTARES")) g_menuOpen.store(true, std::memory_order_relaxed);
        ImGui::End();
        return;
    }
    ImGui::SetNextWindowSize(ImVec2(640, 420), ImGuiCond_FirstUseEver);
    if (ImGui::Begin("HUNTARES CHEAT | INTERNAL v3.0", nullptr)) {
        ImGui::TextColored(ImVec4(.94f,.94f,.94f,1.f), "HUNTARES  /  INTERNAL v3.0");
        ImGui::Separator();
        if (ImGui::BeginTabBar("huntares_tabs")) {
            if (ImGui::BeginTabItem("Visuals")) {
                ImGui::Checkbox("Center crosshair", &g_huntaresCrosshair);
                if (g_huntaresCrosshair)
                    ImGui::SliderFloat("Crosshair scale", &g_huntaresReticleScale, 5.f, 30.f);
                ImGui::Separator();
                ImGui::TextDisabled("ESP 2D Box: not enabled");
                ImGui::TextWrapped("The Ximira 1.0.0 offset profile is unverified on Standoff 2 1.1.0. ");
                ImGui::TextWrapped("Internal drawing is independent of the game's player-position data.");
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Chams / Skins")) {
                ImGui::TextDisabled("Chams: not integrated");
                ImGui::TextDisabled("Weapon skin replacement: not integrated");
                ImGui::TextWrapped("Existing HUNTARES skin preview remains available in the APK loader.");
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Diagnostics")) {
                ImGui::Text("Render FPS: %.1f", io.Framerate);
                ImGui::Text("Viewport: %.0f x %.0f", io.DisplaySize.x, io.DisplaySize.y);
                ImGui::Text("EGL swap slots: %d", g_swapSlots.load(std::memory_order_relaxed));
                ImGui::Text("EGL damage slots: %d", g_swapDamageSlots.load(std::memory_order_relaxed));
                ImGui::Text("Rendered frames: %llu", static_cast<unsigned long long>(g_renderedFrames.load(std::memory_order_relaxed)));
                ImGui::TextDisabled("Target: com.axlebolt.standoff2 / ARM64");
                ImGui::TextDisabled("Game profile: 1.1.0 pending validation");
                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }
        ImGui::Separator();
        if (ImGui::Button("Minimize menu")) g_menuOpen.store(false, std::memory_order_relaxed);
    }
    ImGui::End();
}
