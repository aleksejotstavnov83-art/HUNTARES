#include "content.h"
#include "config.h"
#include "menu.h"

Config cfg;

// Adapted from the user's BaseMenuAIDE controls. This is an independent Android overlay.
// There is intentionally NO fake Chams toggle: renderer/material integration is missing.
const char* const kTabs[] = {"HOME", "CHAMS", "SETTINGS"};
const int kTabCount = 3;
static const char* const kAccentNames[] = {"Blue", "Purple", "Green", "Red", "Orange"};
static int accentIndex = 0;

void buildContent(Menu& m, int tab) {
    switch (tab) {
    case 0:
        m.section("HUNTARES CHEAT");
        m.label("Native touch overlay: active");
        m.label("Tap X to close the window");
        m.label("The bottom handle opens this menu");
        m.label("Use INJECT in the launcher");
        break;
    case 1:
        m.section("CHAMS / CURRENT STATUS");
        m.label("Status: NOT CONNECTED");
        m.label("No verified player renderers");
        m.label("No compiled Unity shader bundle");
        m.label("No in-game materials modified");
        m.label("This is not a working Chams build");
        break;
    case 2: {
        static const Col presets[5] = {rgba(72,149,255),rgba(124,92,255),rgba(46,204,130),rgba(255,82,96),rgba(255,150,50)};
        m.section("MENU SETTINGS");
        m.toggle("Show watermark", &m.showWatermark);
        m.slider("Menu scale", &m.uiScale, 0.75f, 1.25f, "%.2f");
        if (m.choice("Accent", &accentIndex, kAccentNames, 5)) m.accent = presets[accentIndex];
        break;
    }
    }
}
