#pragma once
#include <vector>

struct Col {
    float r, g, b, a;
};
inline Col rgba(int r, int g, int b, int a = 255) { return Col{r / 255.f, g / 255.f, b / 255.f, a / 255.f}; }
inline Col withA(Col c, float a) { c.a *= a; return c; }
inline Col mix(Col a, Col b, float t) {
    return Col{a.r + (b.r - a.r) * t, a.g + (b.g - a.g) * t, a.b + (b.b - a.b) * t, a.a + (b.a - a.a) * t};
}

// Свой 2D-рендерер на OpenGL ES 2.0: все примитивы батчатся в один буфер.
// Скруглённые прямоугольники, тени и свечение считаются в шейдере (SDF).
class Gfx {
public:
    float alpha = 1.f;       // общий множитель прозрачности
    float ox = 0.f, oy = 0.f; // общий сдвиг (для анимаций)

    bool init();
    void begin(int w, int h);
    void end();

    void rect(float x, float y, float w, float h, float r, Col c, float soft = 1.f) { rectImpl(x, y, w, h, r, c, c, c, c, soft); }
    void rect(float x, float y, float w, float h, float r, Col top, Col bot, float soft = 1.f) { rectImpl(x, y, w, h, r, top, top, bot, bot, soft); }
    void rectH(float x, float y, float w, float h, float r, Col left, Col right, float soft = 1.f) { rectImpl(x, y, w, h, r, left, right, left, right, soft); }
    void shadow(float x, float y, float w, float h, float r, float blur, Col c);

    // size = высота заглавной буквы в пикселях
    void text(float x, float y, const char* s, float size, Col c);
    float textWidth(const char* s, float size) const;

    void clip(float x, float y, float w, float h);
    void unclip();

private:
    void rectImpl(float x, float y, float w, float h, float r, Col tl, Col tr, Col bl, Col br, float soft);
    void vtx(float x, float y, float u, float v, Col c, float e0, float e1, float e2, float e3);
    void flush();

    unsigned prog_ = 0, tex_ = 0;
    int locScreen_ = -1, locTex_ = -1;
    int W_ = 0, H_ = 0;
    std::vector<float> buf_;
};
