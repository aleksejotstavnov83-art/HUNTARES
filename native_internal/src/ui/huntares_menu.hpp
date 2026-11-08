#pragma once
// HUNTARES v3.2: BYCMD-style bottom toggle and one Chams control.
// The Unity-renderer Chams implementation is NOT connected; see README_RU.md.
#include <atomic>
#include "imgui.h"

static bool g_chamsRequested = false;
static float g_chamsColor[3] = {0.22f, 1.0f, 0.47f};

void renderMinimalMenu() {
    ImGuiIO& io = ImGui::GetIO();
    const float ww = io.DisplaySize.x;
    const float hh = io.DisplaySize.y;

    // Bottom-center BYCMD-like bar remains available when the main menu is hidden.
    const ImVec2 barSize(218.f, 43.f);
    ImGui::SetNextWindowPos(ImVec2((ww - barSize.x)*.5f, hh - barSize.y - 14.f), ImGuiCond_Always);
    ImGui::SetNextWindowSize(barSize, ImGuiCond_Always);
    ImGui::SetNextWindowBgAlpha(.92f);
    const ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings;
    ImGui::Begin("##HuntaresBottomBar", nullptr, flags);
    if (ImGui::Button("HUNTARES  |  MENU", ImVec2(-1.f, 0.f))) {
        g_menuOpen.store(!g_menuOpen.load(std::memory_order_relaxed), std::memory_order_relaxed);
    }
    ImGui::End();

    if (!g_menuOpen.load(std::memory_order_relaxed)) return;

    ImGui::SetNextWindowPos(ImVec2(ww * .5f, hh * .43f), ImGuiCond_FirstUseEver, ImVec2(.5f,.5f));
    ImGui::SetNextWindowSize(ImVec2(570.f, 320.f), ImGuiCond_FirstUseEver);
    ImGui::Begin("HUNTARES CHEAT  |  CHAMS TEST v3.2", nullptr,
                 ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings);
    ImGui::TextUnformatted("VISUALS / CHAMS");
    ImGui::Separator();
    ImGui::Checkbox("Chams (experimental request)", &g_chamsRequested);
    ImGui::ColorEdit3("Color", g_chamsColor, ImGuiColorEditFlags_NoInputs);
    ImGui::Separator();
    ImGui::TextColored(ImVec4(1.f,.67f,.32f,1.f), "STATUS: GAME RENDERER NOT CONNECTED");
    ImGui::TextWrapped("This switch records the request but does not change enemy shaders. "
                       "The supplied chams.hpp requires missing Unity bindings, valid "
                       "1.1.0 player objects and a compiled shader AssetBundle.");
    ImGui::TextDisabled("Test target: Android ARM64 / Standoff 2 1.1.0");
    if (ImGui::Button("Hide menu")) g_menuOpen.store(false, std::memory_order_relaxed);
    ImGui::End();
}
