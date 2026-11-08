#pragma once
#ifdef __ANDROID__
#include <android/log.h>
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, "HUNTARES_MENU", __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, "HUNTARES_MENU", __VA_ARGS__)
#else
#include <cstdio>
#define LOGI(...) (std::printf(__VA_ARGS__), std::printf("\n"))
#define LOGE(...) (std::printf(__VA_ARGS__), std::printf("\n"))
#endif
