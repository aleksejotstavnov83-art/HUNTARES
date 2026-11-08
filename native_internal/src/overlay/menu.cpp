#include "menu.h"

#include <algorithm>
#include <cmath>
#include <cstdio>

#include "content.h"

static inline float clampf(float v, float a, float b) { return v < a ? a : (v > b ? b : v); }

Menu::Menu() { accent = rgba(72, 149, 255); watermark = "HUNTARES CHEAT"; }

Col Menu::accentL() const { return mix(accent, rgba(255, 255, 255), 0.38f); }

void Menu::recalc() {
    if (W_ <= 0) return;
    u_ = std::min(W_, H_) / 540.f * uiScale;
    lastScale_ = uiScale;
    ww_ = std::min(660 * u_, W_ - 24 * u_);
    wh_ = std::min(420 * u_, H_ - 24 * u_);
    if (!placed_) {
        wx_ = (W_ - ww_) / 2;
        wy_ = (H_ - wh_) / 2;
        placed_ = true;
    }
    wx_ = clampf(wx_, 0, std::max(0.f, W_ - ww_));
    wy_ = clampf(wy_, 0, std::max(0.f, H_ - wh_));
}

void Menu::resize(int w, int h) {
    std::lock_guard<std::mutex> l(mu_);
    W_ = w;
    H_ = h;
    placed_ = false;
    recalc();
}

void Menu::handleRect(float& x, float& y, float& w, float& h) const {
    w = 120 * u_;
    h = 10 * u_;
    x = (W_ - w) / 2;
    y = H_ * handleY - h / 2;
}

// ---------------------------------------------------------------- ввод

bool Menu::touch(int action, int pid, float x, float y) {
    std::lock_guard<std::mutex> l(mu_);
    if (W_ <= 0) return false;
    const float th = 46 * u_;

    if (action == 0) {
        if (capId_ != -1) return false;
        float hx, hy, hw, hh;
        handleRect(hx, hy, hw, hh);
        bool inH = x >= hx - 30 * u_ && x <= hx + hw + 30 * u_ && y >= hy - 24 * u_ && y <= hy + hh + 24 * u_;
        bool inW = open_ && anim_ > 0.5f && x >= wx_ && x <= wx_ + ww_ && y >= wy_ && y <= wy_ + wh_;
        if (!inH && !inW) return false; // всё остальное уходит игре
        capId_ = pid;
        down_ = pressed_ = true;
        moved_ = false;
        px_ = ix_ = x;
        py_ = iy_ = lastY_ = y;
        handleDown_ = inH;
        dragWin_ = closeDown_ = false;
        if (!inH && y <= wy_ + th) {
            if (x >= wx_ + ww_ - th) {
                closeDown_ = true;
            } else {
                dragWin_ = true;
                dragDX_ = x - wx_;
                dragDY_ = y - wy_;
            }
        }
        return true;
    }

    if (pid != capId_) return false;
    ix_ = x;
    iy_ = y;

    if (action == 2) {
        if (!moved_ && (std::fabs(x - px_) > 8 * u_ || std::fabs(y - py_) > 8 * u_)) moved_ = true;
        if (dragWin_) {
            wx_ = clampf(x - dragDX_, 0, std::max(0.f, W_ - ww_));
            wy_ = clampf(y - dragDY_, 0, std::max(0.f, H_ - wh_));
        }
        return true;
    }

    down_ = false;
    released_ = true;
    capId_ = -1;
    if (action == 1) {
        if (handleDown_ && !moved_) open_ = !open_;
        if (closeDown_ && x >= wx_ + ww_ - th && y <= wy_ + th) open_ = false;
    }
    handleDown_ = dragWin_ = closeDown_ = false;
    return true;
}

bool Menu::hit(float x, float y, float w, float h, float tx, float ty) const {
    return tx >= x && tx <= x + w && ty >= y && ty <= y + h && ty >= top_ && ty <= bottom_;
}
bool Menu::tapped(float x, float y, float w, float h) const {
    return inputOK_ && released_ && !moved_ && hit(x, y, w, h, px_, py_) && hit(x, y, w, h, ix_, iy_);
}
bool Menu::held(float x, float y, float w, float h) const {
    return inputOK_ && down_ && !moved_ && hit(x, y, w, h, px_, py_);
}
float Menu::animTo(int id, float target) {
    auto it = anims_.find(id);
    if (it == anims_.end()) it = anims_.emplace(id, target).first;
    float& a = it->second;
    a += (target - a) * std::min(1.f, dt_ * 16.f);
    return a;
}

// ---------------------------------------------------------------- кадр

void Menu::render(Gfx& g, float dt) {
    std::lock_guard<std::mutex> l(mu_);
    if (W_ <= 0) return;
    g_ = &g;
    dt_ = clampf(dt, 0.f, 0.1f);
    time_ += dt_;
    if (dt > 0.0001f) fps_ += (1.f / dt - fps_) * 0.05f;
    if (uiScale != lastScale_) recalc();
    anim_ += ((open_ ? 1.f : 0.f) - anim_) * std::min(1.f, dt_ * 14.f);
    if (!open_ && anim_ < 0.01f) anim_ = 0.f;

    g.begin(W_, H_);
    drawWatermark();
    drawHandle();
    inputOK_ = false;
    if (anim_ > 0.01f) drawWindow();
    g.alpha = 1.f;
    g.oy = 0.f;
    g.end();

    pressed_ = released_ = false;
}

void Menu::drawWatermark() {
    if (!showWatermark) return;
    Gfx& g = *g_;
    char buf[96];
    std::snprintf(buf, sizeof(buf), "%s  |  %d FPS", watermark.c_str(), (int)(fps_ + 0.5f));
    const float ts = 12 * u_, padx = 14 * u_, h = 30 * u_;
    const float tw = g.textWidth(buf, ts);
    const float w = tw + padx * 2, x = W_ - 12 * u_ - w, y = 12 * u_;
    g.shadow(x, y + 2 * u_, w, h, 9 * u_, 14 * u_, rgba(0, 0, 0, 110));
    g.rect(x, y, w, h, 9 * u_, rgba(22, 24, 34, 215), rgba(14, 15, 22, 225));
    g.rectH(x + 8 * u_, y, w - 16 * u_, 2.f * u_, u_, accent, accentL());
    g.text(x + padx, y + (h - ts) / 2 + 1 * u_, buf, ts, rgba(235, 238, 250));
}

void Menu::drawHandle() {
    Gfx& g = *g_;
    float x, y, w, h;
    handleRect(x, y, w, h);
    float pulse = 0.5f + 0.5f * std::sin(time_ * 2.6f);
    float glow = open_ ? 0.18f : 0.22f + 0.16f * pulse;
    g.shadow(x - 4 * u_, y - 4 * u_, w + 8 * u_, h + 8 * u_, h, 18 * u_, withA(accent, glow));
    g.rectH(x, y, w, h, h / 2, accent, accentL());
}

void Menu::drawWindow() {
    Gfx& g = *g_;
    const float e = anim_ * anim_ * (3.f - 2.f * anim_);
    g.alpha = e;
    g.oy = (1.f - e) * 28 * u_;
    inputOK_ = anim_ > 0.9f;

    const float x = wx_, y = wy_, w = ww_, h = wh_, rad = 16 * u_, th = 46 * u_;
    const Col ac = accent, al = accentL();

    g.shadow(x - 6 * u_, y, w + 12 * u_, h + 14 * u_, rad + 6 * u_, 38 * u_, rgba(0, 0, 0, 150));
    g.rect(x - 1, y - 1, w + 2, h + 2, rad + 1, withA(ac, 0.6f), rgba(70, 74, 96, 190));
    g.rect(x, y, w, h, rad, rgba(24, 26, 37, 246), rgba(14, 15, 21, 248));

    // заголовок
    const char* title = "HUNTARES CHEAT";
    g.text(x + 20 * u_, y + (th - 16 * u_) / 2, title, 16 * u_, rgba(240, 242, 255));
    g.text(x + 20 * u_ + g.textWidth(title, 16 * u_) + 10 * u_, y + (th - 10 * u_) / 2, "v3.5", 10 * u_, rgba(125, 131, 158));
    g.rectH(x + 16 * u_, y + th, w - 32 * u_, 2 * u_, u_, withA(ac, 0.95f), withA(ac, 0.f));
    {
        float tw = g.textWidth("X", 14 * u_);
        Col cc = closeDown_ ? rgba(255, 255, 255) : rgba(140, 146, 172);
        g.text(x + w - th + (th - tw) / 2, y + (th - 14 * u_) / 2, "X", 14 * u_, cc);
    }

    // боковая панель с вкладками
    top_ = -1e9f;
    bottom_ = 1e9f;
    const float sx = x + 12 * u_, sy = y + th + 14 * u_, sw = 136 * u_, sh = h - th - 26 * u_;
    g.rect(sx, sy, sw, sh, 12 * u_, rgba(255, 255, 255, 9));
    for (int i = 0; i < kTabCount; ++i) {
        float tx = sx + 8 * u_, ty = sy + 8 * u_ + i * 46 * u_, tw = sw - 16 * u_, thh = 40 * u_;
        bool sel = (i == tab_);
        if (sel) {
            g.rect(tx, ty, tw, thh, 10 * u_, withA(ac, 0.32f), withA(ac, 0.14f));
            g.rect(tx, ty + 9 * u_, 3 * u_, thh - 18 * u_, 1.5f * u_, al);
        } else if (held(tx, ty, tw, thh)) {
            g.rect(tx, ty, tw, thh, 10 * u_, rgba(255, 255, 255, 14));
        }
        g.text(tx + 18 * u_, ty + (thh - 13 * u_) / 2, kTabs[i], 13 * u_, sel ? rgba(255, 255, 255) : rgba(150, 156, 182));
        if (tapped(tx, ty, tw, thh) && !sel) {
            tab_ = i;
            scroll_ = 0;
        }
    }

    // область содержимого
    const float cpx = sx + sw + 10 * u_, cpy = sy, cpw = x + w - 12 * u_ - cpx, cph = sh;
    g.rect(cpx, cpy, cpw, cph, 12 * u_, rgba(255, 255, 255, 6));
    top_ = cpy + 4 * u_;
    bottom_ = cpy + cph - 4 * u_;
    cx_ = cpx + 12 * u_;
    cw_ = cpw - 24 * u_;

    bool pressInContent = px_ >= cpx && px_ <= cpx + cpw && py_ >= cpy && py_ <= cpy + cph;
    if (inputOK_ && down_ && moved_ && activeId_ == 0 && pressInContent && !dragWin_) scroll_ -= (iy_ - lastY_);
    lastY_ = iy_;

    const float startY = top_ + 8 * u_ - scroll_;
    cy_ = startY;
    idc_ = 0;
    g.clip(cpx, cpy + 2 * u_, cpw, cph - 4 * u_);
    buildContent(*this, tab_);
    g.unclip();

    contentH_ = cy_ - startY + 8 * u_;
    const float viewH = bottom_ - top_;
    const float maxScroll = std::max(0.f, contentH_ - viewH);
    scroll_ = clampf(scroll_, 0.f, maxScroll);
    if (maxScroll > 1.f) {
        float bh = std::max(24 * u_, viewH * viewH / contentH_);
        float by = top_ + (viewH - bh) * (scroll_ / maxScroll);
        g.rect(cpx + cpw - 6 * u_, by, 3 * u_, bh, 1.5f * u_, withA(al, 0.55f));
    }
}

// ---------------------------------------------------------------- виджеты

float Menu::rowY(float h) {
    float y = cy_;
    cy_ += h + 8 * u_;
    return y;
}

static void card(Gfx& g, float x, float y, float w, float h, float r, bool pressed) {
    g.rect(x, y, w, h, r, rgba(255, 255, 255, pressed ? 20 : 11));
}

bool Menu::toggle(const char* lbl, bool* v) {
    Gfx& g = *g_;
    const int id = tab_ * 1000 + (++idc_);
    const float h = 42 * u_, y = rowY(h), x = cx_, w = cw_;
    bool changed = false;
    if (tapped(x, y, w, h)) {
        *v = !*v;
        changed = true;
    }
    float t = animTo(id, *v ? 1.f : 0.f);
    if (y + h < top_ || y > bottom_) return changed;

    card(g, x, y, w, h, 10 * u_, held(x, y, w, h));
    g.text(x + 14 * u_, y + (h - 13 * u_) / 2, lbl, 13 * u_, rgba(228, 231, 245));
    const float sw = 46 * u_, sh = 24 * u_, sx = x + w - sw - 14 * u_, sy = y + (h - sh) / 2;
    g.rect(sx, sy, sw, sh, sh / 2, mix(rgba(74, 78, 98), accent, t), mix(rgba(60, 64, 82), accentL(), t));
    const float d = sh - 6 * u_;
    g.rect(sx + 3 * u_ + (sw - sh) * t, sy + 3 * u_, d, d, d / 2, rgba(255, 255, 255), rgba(222, 226, 240));
    return changed;
}

bool Menu::slider(const char* lbl, float* v, float mn, float mx, const char* fmt) {
    Gfx& g = *g_;
    const int id = tab_ * 1000 + (++idc_);
    const float h = 60 * u_, y = rowY(h), x = cx_, w = cw_;
    const float tx = x + 16 * u_, tw = w - 32 * u_, trh = 8 * u_, ty = y + h - 20 * u_;
    bool changed = false;

    if (inputOK_ && pressed_ && hit(x, y, w, h, px_, py_)) activeId_ = id;
    if (activeId_ == id) {
        if (down_ || released_) {
            float f = clampf((ix_ - tx) / tw, 0.f, 1.f);
            float nv = mn + (mx - mn) * f;
            if (nv != *v) {
                *v = nv;
                changed = true;
            }
        }
        if (released_) activeId_ = 0;
    }
    if (y + h < top_ || y > bottom_) return changed;

    const bool active = activeId_ == id;
    card(g, x, y, w, h, 10 * u_, active);
    g.text(x + 14 * u_, y + 11 * u_, lbl, 13 * u_, rgba(228, 231, 245));
    char vb[32];
    std::snprintf(vb, sizeof(vb), fmt, *v);
    g.text(x + w - 14 * u_ - g.textWidth(vb, 13 * u_), y + 11 * u_, vb, 13 * u_, accentL());

    float f = clampf((*v - mn) / (mx - mn), 0.f, 1.f);
    g.rect(tx, ty, tw, trh, trh / 2, rgba(255, 255, 255, 24));
    g.rectH(tx, ty, std::max(trh, tw * f), trh, trh / 2, accent, accentL());
    const float kr = (active ? 12 : 10) * u_, kx = tx + tw * f, ky = ty + trh / 2;
    g.shadow(kx - kr, ky - kr, kr * 2, kr * 2, kr, 12 * u_, withA(accent, active ? 0.55f : 0.3f));
    g.rect(kx - kr, ky - kr, kr * 2, kr * 2, kr, rgba(255, 255, 255), rgba(224, 228, 242));
    return changed;
}

bool Menu::choice(const char* lbl, int* v, const char* const* opts, int n) {
    Gfx& g = *g_;
    ++idc_;
    const float h = 42 * u_, y = rowY(h), x = cx_, w = cw_;
    const float cw = 190 * u_, cl = x + w - cw - 10 * u_;
    bool changed = false;
    if (n > 0 && tapped(cl, y, cw, h)) {
        *v = (ix_ < cl + cw / 2) ? (*v + n - 1) % n : (*v + 1) % n;
        changed = true;
    }
    if (y + h < top_ || y > bottom_) return changed;

    card(g, x, y, w, h, 10 * u_, false);
    g.text(x + 14 * u_, y + (h - 13 * u_) / 2, lbl, 13 * u_, rgba(228, 231, 245));
    g.rect(cl, y + 6 * u_, cw, h - 12 * u_, 8 * u_, rgba(0, 0, 0, 70));
    const char* name = (n > 0) ? opts[*v] : "-";
    g.text(cl + (cw - g.textWidth(name, 12 * u_)) / 2, y + (h - 12 * u_) / 2, name, 12 * u_, rgba(240, 242, 255));
    g.text(cl + 12 * u_, y + (h - 13 * u_) / 2, "<", 13 * u_, accentL());
    g.text(cl + cw - 12 * u_ - g.textWidth(">", 13 * u_), y + (h - 13 * u_) / 2, ">", 13 * u_, accentL());
    return changed;
}

bool Menu::button(const char* lbl) {
    Gfx& g = *g_;
    ++idc_;
    const float h = 40 * u_, y = rowY(h), x = cx_, w = cw_;
    const bool clicked = tapped(x, y, w, h);
    if (y + h < top_ || y > bottom_) return clicked;
    const bool hd = held(x, y, w, h);
    Col a = hd ? mix(accent, rgba(0, 0, 0), 0.25f) : accent;
    Col b = hd ? mix(accentL(), rgba(0, 0, 0), 0.25f) : accentL();
    g.rect(x, y, w, h, 10 * u_, a, b);
    g.text(x + (w - g.textWidth(lbl, 13 * u_)) / 2, y + (h - 13 * u_) / 2, lbl, 13 * u_, rgba(255, 255, 255));
    return clicked;
}

void Menu::label(const char* text) {
    Gfx& g = *g_;
    ++idc_;
    const float h = 20 * u_, y = rowY(h);
    if (y + h < top_ || y > bottom_) return;
    g.text(cx_ + 4 * u_, y + (h - 12 * u_) / 2, text, 12 * u_, rgba(130, 136, 162));
}

void Menu::section(const char* title) {
    Gfx& g = *g_;
    ++idc_;
    const float h = 24 * u_, y = rowY(h);
    if (y + h < top_ || y > bottom_) return;
    const float ts = 11 * u_, tw = g.textWidth(title, ts);
    g.text(cx_ + 4 * u_, y + (h - ts) / 2, title, ts, accentL());
    g.rect(cx_ + tw + 16 * u_, y + h / 2, cw_ - tw - 20 * u_, 1.f * u_, 0.f, rgba(255, 255, 255, 22));
}
