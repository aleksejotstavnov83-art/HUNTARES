#include <jni.h>
#include <time.h>

#include "gfx.h"
#include "log.h"
#include "menu.h"

static Gfx gGfx;
static Menu gMenu;
static double gLast = 0.0;
static bool gReady = false;

static double nowSec() {
    timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec * 1e-9;
}

extern "C" {

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM*, void*) {
    LOGI("HUNTARES CHEAT native AIDE menu loaded");
    return JNI_VERSION_1_6;
}

JNIEXPORT void JNICALL Java_com_huntares_launcher_NativeMenu_onSurfaceCreated(JNIEnv*, jclass) {
    gGfx = Gfx(); // новый GL-контекст: старые объекты недействительны
    gReady = gGfx.init();
    gMenu.setOpen(true);
    gLast = 0.0;
}

JNIEXPORT void JNICALL Java_com_huntares_launcher_NativeMenu_onSurfaceChanged(JNIEnv*, jclass, jint w, jint h) {
    gMenu.resize(w, h);
}

JNIEXPORT void JNICALL Java_com_huntares_launcher_NativeMenu_onDrawFrame(JNIEnv*, jclass) {
    if (!gReady) return;
    double t = nowSec();
    float dt = gLast > 0.0 ? (float)(t - gLast) : 0.016f;
    gLast = t;
    gMenu.render(gGfx, dt);
}

JNIEXPORT jboolean JNICALL Java_com_huntares_launcher_NativeMenu_onTouch(JNIEnv*, jclass, jint action, jint pid, jfloat x, jfloat y) {
    return gMenu.touch(action, pid, x, y) ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jboolean JNICALL Java_com_huntares_launcher_NativeMenu_isOpen(JNIEnv*, jclass) {
    return gMenu.isOpen() ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT void JNICALL Java_com_huntares_launcher_NativeMenu_setOpen(JNIEnv*, jclass, jboolean open) {
    gMenu.setOpen(open == JNI_TRUE);
}

} // extern "C"
