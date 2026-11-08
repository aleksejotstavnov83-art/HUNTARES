#include "gfx.h"

#include <GLES2/gl2.h>

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstring>

#include "font.h"
#include "log.h"

namespace {

const char* kVS =
    "attribute vec2 aPos; attribute vec2 aUV; attribute vec4 aCol; attribute vec4 aExt;\n"
    "uniform vec2 uScreen;\n"
    "varying vec2 vUV; varying vec4 vCol; varying vec4 vExt;\n"
    "void main(){ vec2 p=aPos/uScreen*2.0-1.0; gl_Position=vec4(p.x,-p.y,0.0,1.0);"
    " vUV=aUV; vCol=aCol; vExt=aExt; }\n";

const char* kFS =
    "#ifdef GL_FRAGMENT_PRECISION_HIGH\nprecision highp float;\n#else\nprecision mediump float;\n#endif\n"
    "uniform sampler2D uTex;\n"
    "varying vec2 vUV; varying vec4 vCol; varying vec4 vExt;\n"
    "void main(){\n"
    "  if(vExt.x<0.0){ float a=texture2D(uTex,vUV).a; gl_FragColor=vec4(vCol.rgb,vCol.a*a); return; }\n"
    "  vec2 q=abs(vUV)-vExt.xy+vec2(vExt.z);\n"
    "  float d=length(max(q,0.0))+min(max(q.x,q.y),0.0)-vExt.z;\n"
    "  float a=clamp(0.5-d/vExt.w,0.0,1.0);\n"
    "  gl_FragColor=vec4(vCol.rgb,vCol.a*a);\n"
    "}\n";

GLuint compile(GLenum type, const char* src) {
    GLuint sh = glCreateShader(type);
    glShaderSource(sh, 1, &src, nullptr);
    glCompileShader(sh);
    GLint ok = 0;
    glGetShaderiv(sh, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        char log[512] = {0};
        glGetShaderInfoLog(sh, sizeof(log), nullptr, log);
        LOGE("shader error: %s", log);
        glDeleteShader(sh);
        return 0;
    }
    return sh;
}

// Атлас: ячейка 6x8 «пикселей шрифта», каждый пиксель увеличен в S раз и слегка
// размыт, поэтому текст выглядит гладко при любом масштабе.
const int GC = 16, GR = 6, S = 4, CW = 6 * S, CH = 8 * S;
const int AW = GC * CW, AH = GR * CH;

void buildAtlas(GLuint& tex) {
    std::vector<uint8_t> hi(AW * AH, 0), out(AW * AH, 0);
    for (int g = 0; g < 95; ++g) {
        int cx = (g % GC) * CW, cy = (g / GC) * CH;
        for (int c = 0; c < 5; ++c) {
            uint8_t bits = kFont5x7[g * 5 + c];
            for (int r = 0; r < 7; ++r) {
                if (!(bits & (1 << r))) continue;
                for (int dy = 0; dy < S; ++dy)
                    for (int dx = 0; dx < S; ++dx) hi[(cy + r * S + dy) * AW + cx + c * S + dx] = 255;
            }
        }
    }
    for (int y = 0; y < AH; ++y) {
        for (int x = 0; x < AW; ++x) {
            int s = 0;
            for (int dy = -1; dy <= 1; ++dy)
                for (int dx = -1; dx <= 1; ++dx) {
                    int xx = x + dx, yy = y + dy;
                    if (xx >= 0 && xx < AW && yy >= 0 && yy < AH) s += hi[yy * AW + xx];
                }
            out[y * AW + x] = (uint8_t)(s / 9);
        }
    }
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, AW, AH, 0, GL_ALPHA, GL_UNSIGNED_BYTE, out.data());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

const int kStride = 12; // floats на вершину: pos2 uv2 col4 ext4

} // namespace

bool Gfx::init() {
    GLuint vs = compile(GL_VERTEX_SHADER, kVS);
    GLuint fs = compile(GL_FRAGMENT_SHADER, kFS);
    if (!vs || !fs) return false;
    prog_ = glCreateProgram();
    glAttachShader(prog_, vs);
    glAttachShader(prog_, fs);
    glBindAttribLocation(prog_, 0, "aPos");
    glBindAttribLocation(prog_, 1, "aUV");
    glBindAttribLocation(prog_, 2, "aCol");
    glBindAttribLocation(prog_, 3, "aExt");
    glLinkProgram(prog_);
    GLint ok = 0;
    glGetProgramiv(prog_, GL_LINK_STATUS, &ok);
    if (!ok) {
        LOGE("program link failed");
        return false;
    }
    glDeleteShader(vs);
    glDeleteShader(fs);
    locScreen_ = glGetUniformLocation(prog_, "uScreen");
    locTex_ = glGetUniformLocation(prog_, "uTex");
    buildAtlas(tex_);
    buf_.reserve(kStride * 6 * 2048);
    LOGI("Gfx ready");
    return true;
}

void Gfx::begin(int w, int h) {
    W_ = w;
    H_ = h;
    alpha = 1.f;
    ox = oy = 0.f;
    buf_.clear();
    glViewport(0, 0, w, h);
    glClearColor(0.f, 0.f, 0.f, 0.f);
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glDisable(GL_SCISSOR_TEST);
    glEnable(GL_BLEND);
    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    glUseProgram(prog_);
    glUniform2f(locScreen_, (float)w, (float)h);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex_);
    glUniform1i(locTex_, 0);
}

void Gfx::end() {
    flush();
    glDisable(GL_SCISSOR_TEST);
}

void Gfx::flush() {
    if (buf_.empty()) return;
    const float* p = buf_.data();
    const GLsizei stride = kStride * sizeof(float);
    for (int i = 0; i < 4; ++i) glEnableVertexAttribArray(i);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, p + 0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, p + 2);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, stride, p + 4);
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, stride, p + 8);
    glDrawArrays(GL_TRIANGLES, 0, (GLsizei)(buf_.size() / kStride));
    buf_.clear();
}

void Gfx::vtx(float x, float y, float u, float v, Col c, float e0, float e1, float e2, float e3) {
    const float d[kStride] = {x, y, u, v, c.r, c.g, c.b, c.a * alpha, e0, e1, e2, e3};
    buf_.insert(buf_.end(), d, d + kStride);
}

void Gfx::rectImpl(float x, float y, float w, float h, float r, Col tl, Col tr, Col bl, Col br, float soft) {
    if (w <= 0.f || h <= 0.f) return;
    r = std::min(r, std::min(w, h) * 0.5f);
    soft = std::max(soft, 1.f);
    x += ox;
    y += oy;
    const float pad = soft + 1.f;
    const float x0 = x - pad, x1 = x + w + pad, y0 = y - pad, y1 = y + h + pad;
    const float cx = x + w * 0.5f, cy = y + h * 0.5f, hw = w * 0.5f, hh = h * 0.5f;
    auto col = [&](float vx, float vy) {
        float fx = std::min(1.f, std::max(0.f, (vx - x) / w));
        float fy = std::min(1.f, std::max(0.f, (vy - y) / h));
        return mix(mix(tl, tr, fx), mix(bl, br, fx), fy);
    };
    Col c00 = col(x0, y0), c10 = col(x1, y0), c01 = col(x0, y1), c11 = col(x1, y1);
    vtx(x0, y0, x0 - cx, y0 - cy, c00, hw, hh, r, soft);
    vtx(x1, y0, x1 - cx, y0 - cy, c10, hw, hh, r, soft);
    vtx(x1, y1, x1 - cx, y1 - cy, c11, hw, hh, r, soft);
    vtx(x0, y0, x0 - cx, y0 - cy, c00, hw, hh, r, soft);
    vtx(x1, y1, x1 - cx, y1 - cy, c11, hw, hh, r, soft);
    vtx(x0, y1, x0 - cx, y1 - cy, c01, hw, hh, r, soft);
}

void Gfx::shadow(float x, float y, float w, float h, float r, float blur, Col c) {
    float g = blur * 0.25f;
    rectImpl(x - g, y - g, w + 2 * g, h + 2 * g, r + g, c, c, c, c, blur);
}

float Gfx::textWidth(const char* s, float size) const {
    int n = (int)std::strlen(s);
    if (n == 0) return 0.f;
    float k = size / 7.f;
    return (n * 6 - 1) * k;
}

void Gfx::text(float x, float y, const char* s, float size, Col c) {
    const float k = size / 7.f;
    x += ox;
    y += oy;
    for (; *s; ++s) {
        unsigned char ch = (unsigned char)*s;
        if (ch < 32 || ch > 126) ch = '?';
        if (ch != ' ') {
            int idx = ch - 32;
            float u0 = (float)((idx % GC) * CW) / AW, v0 = (float)((idx / GC) * CH) / AH;
            float u1 = u0 + (float)CW / AW, v1 = v0 + (float)CH / AH;
            float x1 = x + 6 * k, y1 = y + 8 * k;
            vtx(x, y, u0, v0, c, -1, 0, 0, 1);
            vtx(x1, y, u1, v0, c, -1, 0, 0, 1);
            vtx(x1, y1, u1, v1, c, -1, 0, 0, 1);
            vtx(x, y, u0, v0, c, -1, 0, 0, 1);
            vtx(x1, y1, u1, v1, c, -1, 0, 0, 1);
            vtx(x, y1, u0, v1, c, -1, 0, 0, 1);
        }
        x += 6 * k;
    }
}

void Gfx::clip(float x, float y, float w, float h) {
    flush();
    glEnable(GL_SCISSOR_TEST);
    int sx = (int)std::floor(x + ox), sy = (int)std::floor(H_ - (y + oy + h));
    glScissor(sx, std::max(0, sy), std::max(0, (int)std::ceil(w)), std::max(0, (int)std::ceil(h)));
}

void Gfx::unclip() {
    flush();
    glDisable(GL_SCISSOR_TEST);
}
