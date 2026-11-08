
// ================================================================================
// Bot: @imguisource_bot
// Channel #1: @ImGui_Source_FIXNEY | ImGui Source
// Channel #2: @il2cpp_src | shit src 🧨
// ================================================================================

#include "ximira/features/world_mod/world_mod.h"
#include "ximira/core/il2cpp.h"
#include "ximira/util/log.h"

#include <dlfcn.h>
#include <cstring>

namespace xim::features::world_mod {

static WorldCfg g_cfg;
WorldCfg& config() { return g_cfg; }

namespace {
using t_domain_get = void* (*)();
using t_domain_assembly_open = void* (*)(void*, const char*);
using t_assembly_get_image = void* (*)(void*);
using t_class_from_name = void* (*)(void*, const char*, const char*);
using t_class_get_method = void* (*)(void*, const char*, int);
using t_runtime_invoke = void* (*)(const void*, void*, void**, void**);

static t_domain_get p_domain_get = nullptr;
static t_domain_assembly_open p_domain_assembly_open = nullptr;
static t_assembly_get_image p_assembly_get_image = nullptr;
static t_class_from_name p_class_from_name = nullptr;
static t_class_get_method p_class_get_method = nullptr;
static t_runtime_invoke p_runtime_invoke = nullptr;
static void* g_render_settings = nullptr;
static void* g_set_fog = nullptr;
static void* g_set_fog_mode = nullptr;
static void* g_set_fog_density = nullptr;
static void* g_set_fog_color = nullptr;
static void* g_set_fog_start = nullptr;
static void* g_set_fog_end = nullptr;
static void* g_set_ambient_light = nullptr;
static void* g_set_ambient_intensity = nullptr;
static void* g_set_shadow_distance = nullptr;
static bool g_logged_missing = false;

struct UnityColor { float r, g, b, a; };

template <typename T>
static bool invoke_setter(void* method, T& value) {
    if (!method || !p_runtime_invoke) return false;
    void* args[] = { &value };
    void* exception = nullptr;
    p_runtime_invoke(method, nullptr, args, &exception);
    if (exception) {
        XWARN("world_mod: Unity setter raised managed exception");
        return false;
    }
    return true;
}

static bool resolve_render_settings() {
    // If the class was found but a property setter was not yet available, retry only
    // the missing methods instead of treating the partial lookup as permanently ready.
    if (g_render_settings && p_class_get_method) {
        auto method = [](const char* name, int argc) -> void* {
            return p_class_get_method ? p_class_get_method(g_render_settings, name, argc) : nullptr;
        };
        if (!g_set_fog) g_set_fog = method("set_fog", 1);
        if (!g_set_fog_mode) g_set_fog_mode = method("set_fogMode", 1);
        if (!g_set_fog_density) g_set_fog_density = method("set_fogDensity", 1);
        if (!g_set_fog_color) g_set_fog_color = method("set_fogColor", 1);
        if (!g_set_fog_start) g_set_fog_start = method("set_fogStartDistance", 1);
        if (!g_set_fog_end) g_set_fog_end = method("set_fogEndDistance", 1);
        if (!g_set_ambient_light) g_set_ambient_light = method("set_ambientLight", 1);
        if (!g_set_ambient_intensity) g_set_ambient_intensity = method("set_ambientIntensity", 1);
        if (!g_set_shadow_distance) g_set_shadow_distance = method("set_shadowDistance", 1);
        return g_set_fog || g_set_ambient_light || g_set_shadow_distance;
    }
    void* handle = dlopen("libil2cpp.so", RTLD_NOW | RTLD_NOLOAD);
    if (!handle) {
        XWARN("world_mod: libil2cpp.so is not loaded yet");
        return false;
    }
    p_domain_get = reinterpret_cast<t_domain_get>(dlsym(handle, "il2cpp_domain_get"));
    p_domain_assembly_open = reinterpret_cast<t_domain_assembly_open>(dlsym(handle, "il2cpp_domain_assembly_open"));
    p_assembly_get_image = reinterpret_cast<t_assembly_get_image>(dlsym(handle, "il2cpp_assembly_get_image"));
    p_class_from_name = reinterpret_cast<t_class_from_name>(dlsym(handle, "il2cpp_class_from_name"));
    p_class_get_method = reinterpret_cast<t_class_get_method>(dlsym(handle, "il2cpp_class_get_method_from_name"));
    p_runtime_invoke = reinterpret_cast<t_runtime_invoke>(dlsym(handle, "il2cpp_runtime_invoke"));
    if (!p_domain_get || !p_domain_assembly_open || !p_assembly_get_image ||
        !p_class_from_name || !p_class_get_method || !p_runtime_invoke) {
        XWARN("world_mod: required IL2CPP method APIs are not exported");
        return false;
    }

    void* domain = p_domain_get();
    if (!domain) return false;
    void* image = nullptr;
    const char* assemblies[] = { "UnityEngine.CoreModule.dll", "UnityEngine.CoreModule", "UnityEngine.dll", "UnityEngine" };
    for (const char* name : assemblies) {
        void* assembly = p_domain_assembly_open(domain, name);
        if (assembly) {
            image = p_assembly_get_image(assembly);
            if (image) break;
        }
    }
    if (!image) {
        XWARN("world_mod: UnityEngine image not found");
        return false;
    }

    g_render_settings = p_class_from_name(image, "UnityEngine", "RenderSettings");
    if (!g_render_settings) {
        XWARN("world_mod: UnityEngine.RenderSettings class not found");
        return false;
    }

    auto method = [](const char* name, int argc) -> void* {
        return p_class_get_method ? p_class_get_method(g_render_settings, name, argc) : nullptr;
    };
    g_set_fog = method("set_fog", 1);
    g_set_fog_mode = method("set_fogMode", 1);
    g_set_fog_density = method("set_fogDensity", 1);
    g_set_fog_color = method("set_fogColor", 1);
    g_set_fog_start = method("set_fogStartDistance", 1);
    g_set_fog_end = method("set_fogEndDistance", 1);
    g_set_ambient_light = method("set_ambientLight", 1);
    g_set_ambient_intensity = method("set_ambientIntensity", 1);
    g_set_shadow_distance = method("set_shadowDistance", 1);

    XLOG("world_mod: RenderSettings setters fog=%d mode=%d density=%d color=%d start=%d end=%d ambient=%d intensity=%d shadows=%d",
         !!g_set_fog, !!g_set_fog_mode, !!g_set_fog_density, !!g_set_fog_color,
         !!g_set_fog_start, !!g_set_fog_end, !!g_set_ambient_light,
         !!g_set_ambient_intensity, !!g_set_shadow_distance);
    return g_set_fog || g_set_ambient_light || g_set_shadow_distance;
}

static void apply_fog() {
    auto& f = g_cfg.fog;
    bool enabled = f.enabled;
    if (g_set_fog) invoke_setter(g_set_fog, enabled);
    if (!enabled) return;
    static bool missingFogSetterLogged = false;
    if ((!g_set_fog || !g_set_fog_mode || !g_set_fog_color) && !missingFogSetterLogged) {
        XWARN("world_mod: fog requested but one or more essential setters are missing (fog=%d mode=%d color=%d density=%d start=%d end=%d)",
              !!g_set_fog, !!g_set_fog_mode, !!g_set_fog_color, !!g_set_fog_density,
              !!g_set_fog_start, !!g_set_fog_end);
        missingFogSetterLogged = true;
    }

    // UnityEngine.FogMode: Linear=1, Exponential=2, ExponentialSquared=3.
    int mode = f.mode + 1;
    float density = f.density;
    float start = f.start;
    float end = f.end;
    UnityColor color{f.color[0], f.color[1], f.color[2], f.color[3]};
    if (g_set_fog_mode) invoke_setter(g_set_fog_mode, mode);
    if (g_set_fog_density) invoke_setter(g_set_fog_density, density);
    if (g_set_fog_start) invoke_setter(g_set_fog_start, start);
    if (g_set_fog_end) invoke_setter(g_set_fog_end, end);
    if (g_set_fog_color) invoke_setter(g_set_fog_color, color);
}

static void apply_ambient() {
    auto& a = g_cfg.ambient;
    float intensity = a.enabled ? a.intensity : 1.0f;
    if (g_set_ambient_intensity) invoke_setter(g_set_ambient_intensity, intensity);
    if (g_set_ambient_light) {
        UnityColor color = a.enabled
            ? UnityColor{a.color[0], a.color[1], a.color[2], a.color[3]}
            : UnityColor{0.21f, 0.21f, 0.21f, 1.0f};
        invoke_setter(g_set_ambient_light, color);
    }
}

static void apply_shadows() {
    if (!g_set_shadow_distance) return;
    float distance = g_cfg.culling.no_shadows ? 0.0f : 150.0f;
    invoke_setter(g_set_shadow_distance, distance);
}
} // namespace

bool init() { return resolve_render_settings(); }

void update() {
    if (!resolve_render_settings()) {
        if (!g_logged_missing) {
            XWARN("world_mod: update skipped until RenderSettings APIs resolve");
            g_logged_missing = true;
        }
        return;
    }
    apply_fog();
    apply_ambient();
    apply_shadows();
    // Skybox tint is intentionally not emulated by overwriting ambient light:
    // doing so made Skybox and Ambient fight over the same setting.
}

} // namespace xim::features::world_mod
