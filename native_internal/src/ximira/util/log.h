
// ================================================================================
// Telegram bot: t.me/imguisource_bot
// Telegram channel (ImGui Source): t.me/ImGui_Source_FIXNEY
// Telegram channel (shit src 🧨): t.me/il2cpp_src
// ================================================================================

#pragma once
#include <android/log.h>
#define XIM_TAG "Ximira"
#define XERR(...)  __android_log_print(ANDROID_LOG_ERROR, XIM_TAG, __VA_ARGS__)
#define XLOG(...)  __android_log_print(ANDROID_LOG_INFO,  XIM_TAG, __VA_ARGS__)
#define XWARN(...) __android_log_print(ANDROID_LOG_WARN,  XIM_TAG, __VA_ARGS__)
