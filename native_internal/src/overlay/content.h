#pragma once
class Menu;

extern const char* const kTabs[];
extern const int kTabCount;

// Здесь описывается содержимое вкладок.
void buildContent(Menu& m, int tab);
