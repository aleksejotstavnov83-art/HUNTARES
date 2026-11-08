#pragma once
// Значения, которыми управляет меню. Читай их из своего кода.
struct Config {
    bool enabled = false;
    bool optA = false;
    bool optB = true;
    bool optC = false;
    float valueA = 50.f;
    float valueB = 10.f;
    float valueC = 1.0f;
    int mode = 0;
};
extern Config cfg;
