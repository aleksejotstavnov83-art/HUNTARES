#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")"
if [ -n "${NDK_BUILD:-}" ] && [ -x "$NDK_BUILD" ]; then NDK="$NDK_BUILD"
elif command -v ndk-build >/dev/null 2>&1; then NDK="$(command -v ndk-build)"
else echo 'Android NDK required. Use GitHub Actions from your phone.' >&2; exit 1; fi
command -v curl >/dev/null || { echo 'curl required' >&2; exit 1; }
command -v zip >/dev/null || { echo 'zip required' >&2; exit 1; }
mkdir -p native_internal/third_party/zygisk build/module/zygisk
if [ ! -s native_internal/third_party/zygisk/zygisk.hpp ]; then
  curl -fLsS --retry 3 \
    -o native_internal/third_party/zygisk/zygisk.hpp \
    'https://raw.githubusercontent.com/topjohnwu/zygisk-module-sample/master/module/jni/zygisk.hpp'
fi
"$NDK" NDK_PROJECT_PATH="$PWD/native_internal" \
  APP_BUILD_SCRIPT="$PWD/native_internal/Android.mk" \
  NDK_APPLICATION_MK="$PWD/native_internal/Application.mk" \
  NDK_OUT="$PWD/build/native_obj" NDK_LIBS_OUT="$PWD/build/native_libs" -j2
LIB="$PWD/build/native_libs/arm64-v8a/libhuntares_internal.so"
[ -s "$LIB" ] || { echo 'Native library not created' >&2; exit 1; }
cp "$LIB" build/module/zygisk/arm64-v8a.so
cp "$LIB" build/libhuntares_internal.so
cat > build/module/module.prop <<'EOF'
id=huntares_internal
name=HUNTARES INTERNAL v3.2
version=v3.2-beta
versionCode=32
author=HUNTARES
description=Zygisk internal ImGui test for ARM64; gameplay offsets not validated
EOF
(cd build/module && zip -q -r ../HUNTARES_INTERNAL_CHAMS_v32_MODULE.zip module.prop zygisk)
cp run_huntares.sh build/run_huntares.sh
printf '%s\n' 'Build complete:' ' build/libhuntares_internal.so (NOT directly executable)' ' build/HUNTARES_INTERNAL_CHAMS_v32_MODULE.zip (install through root manager)' ' build/run_huntares.sh (launcher after installing module and rebooting)'
