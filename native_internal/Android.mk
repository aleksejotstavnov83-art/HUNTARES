LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE := huntares_internal
LOCAL_SRC_FILES := \
  src/core/entry.cpp \
  src/ximira/core/il2cpp.cpp \
  src/ximira/core/memory.cpp \
  src/ximira/features/world_mod/world_mod.cpp \
  third_party/imgui/imgui.cpp \
  third_party/imgui/imgui_draw.cpp \
  third_party/imgui/imgui_tables.cpp \
  third_party/imgui/imgui_widgets.cpp \
  third_party/imgui/backends/imgui_impl_opengl3.cpp
LOCAL_C_INCLUDES := $(LOCAL_PATH)/src $(LOCAL_PATH)/src/ximira $(LOCAL_PATH)/third_party/imgui $(LOCAL_PATH)/third_party/imgui/backends
LOCAL_CPPFLAGS := -DIMGUI_IMPL_OPENGL_ES3 -std=c++17 -O2 -ffunction-sections -fdata-sections -fvisibility=hidden -Wall -Wextra
LOCAL_LDFLAGS := -Wl,--gc-sections
LOCAL_LDLIBS := -llog -lEGL -lGLESv3 -landroid -ldl
include $(BUILD_SHARED_LIBRARY)

# User-supplied BaseMenuAIDE: standalone Android overlay UI, not a game hook.
include $(CLEAR_VARS)
LOCAL_MODULE := huntaresmenu
LOCAL_SRC_FILES := src/overlay/main.cpp src/overlay/gfx.cpp src/overlay/menu.cpp src/overlay/content.cpp
LOCAL_C_INCLUDES := $(LOCAL_PATH)/src/overlay
LOCAL_LDLIBS := -lGLESv2 -llog
LOCAL_CPPFLAGS := -std=c++17 -fvisibility=hidden -O2
include $(BUILD_SHARED_LIBRARY)
