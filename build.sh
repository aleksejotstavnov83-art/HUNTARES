#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")"
for cmd in aapt javac zip apksigner keytool curl; do
  command -v "$cmd" >/dev/null || { echo "Missing: $cmd"; exit 1; }
done
: "${ANDROID_HOME:?Android SDK required}"
export ANDROID_JAR="${ANDROID_JAR:-$ANDROID_HOME/platforms/android-35/android.jar}"
[ -f "$ANDROID_JAR" ] || { echo "Android platform not found: $ANDROID_JAR"; exit 1; }
if command -v d8 >/dev/null; then DEX=d8; elif command -v dx >/dev/null; then DEX=dx; else echo 'Missing d8/dx'; exit 1; fi
if [ -n "${NDK_BUILD:-}" ] && [ -x "$NDK_BUILD" ]; then NDK="$NDK_BUILD";
elif command -v ndk-build >/dev/null; then NDK=$(command -v ndk-build);
else echo 'Android NDK required; use GitHub Actions to build on phone'; exit 1; fi

mkdir -p native_internal/third_party/zygisk
if [ ! -s native_internal/third_party/zygisk/zygisk.hpp ]; then
  curl -fLsS --retry 3 -o native_internal/third_party/zygisk/zygisk.hpp \
    https://raw.githubusercontent.com/topjohnwu/zygisk-module-sample/master/module/jni/zygisk.hpp
fi
mkdir -p build/dex build/gen build/classes build/module/zygisk
"$NDK" NDK_PROJECT_PATH="$PWD/native_internal" \
  APP_BUILD_SCRIPT="$PWD/native_internal/Android.mk" \
  NDK_APPLICATION_MK="$PWD/native_internal/Application.mk" \
  NDK_OUT="$PWD/build/native_obj" NDK_LIBS_OUT="$PWD/build/native_libs" -j2
LIB="$PWD/build/native_libs/arm64-v8a/libhuntares_internal.so"
[ -s "$LIB" ] || { echo 'Native module not built'; exit 1; }
cp "$LIB" build/module/zygisk/arm64-v8a.so
cat > build/module/module.prop <<'EOF'
id=huntares_internal
name=HUNTARES INTERNAL v3.0
version=v3.0-beta
versionCode=30
author=HUNTARES
 description=Zygisk in-process ImGui renderer for test on ARM64 Android
EOF
sed -i 's/^ description=/description=/' build/module/module.prop
(cd build/module && zip -q -r ../HUNTARES_INTERNAL_v30_MODULE.zip module.prop zygisk)

rm -rf build/gen/* build/classes/* build/dex/*
aapt package -f -m -J build/gen -M app/src/main/AndroidManifest.xml \
  -S app/src/main/res -A app/src/main/assets -I "$ANDROID_JAR"
find app/src/main/java build/gen -name '*.java' > build/sources.txt
javac -source 8 -target 8 -encoding UTF-8 -cp "$ANDROID_JAR" -d build/classes @build/sources.txt
if [ "$DEX" = d8 ]; then
  d8 --lib "$ANDROID_JAR" --output build/dex $(find build/classes -name '*.class')
else
  dx --dex --output=build/dex/classes.dex build/classes
fi
aapt package -f -M app/src/main/AndroidManifest.xml -S app/src/main/res \
  -A app/src/main/assets -I "$ANDROID_JAR" -F build/unsigned.apk
(cd build/dex && zip -q -u ../unsigned.apk classes.dex)
if [ ! -s build/huntares.keystore ]; then
  keytool -genkeypair -keystore build/huntares.keystore -alias huntares \
    -storepass android -keypass android -keyalg RSA -keysize 2048 -validity 3650 \
    -dname 'CN=Huntares Internal,O=Personal' -noprompt
fi
apksigner sign --ks build/huntares.keystore --ks-key-alias huntares \
  --ks-pass pass:android --key-pass pass:android \
  --out build/HUNTARES_INTERNAL_v30.apk build/unsigned.apk
apksigner verify build/HUNTARES_INTERNAL_v30.apk
printf 'DONE: %s\nDONE: %s\n' "$PWD/build/HUNTARES_INTERNAL_v30.apk" "$PWD/build/HUNTARES_INTERNAL_v30_MODULE.zip"
