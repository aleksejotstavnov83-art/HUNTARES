#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")"
for cmd in aapt javac zip apksigner zipalign keytool readelf; do
  command -v "$cmd" >/dev/null || { echo "Missing tool: $cmd" >&2; exit 1; }
done
: "${ANDROID_HOME:?Android SDK required}"
ANDROID_JAR="${ANDROID_JAR:-$ANDROID_HOME/platforms/android-35/android.jar}"
test -f "$ANDROID_JAR" || { echo "Android platform missing: $ANDROID_JAR" >&2; exit 1; }
if command -v d8 >/dev/null 2>&1; then DEX=d8;
elif command -v dx >/dev/null 2>&1; then DEX=dx;
else echo 'Missing d8/dx' >&2; exit 1; fi
if [ -n "${NDK_BUILD:-}" ] && [ -x "$NDK_BUILD" ]; then NDK="$NDK_BUILD";
elif command -v ndk-build >/dev/null 2>&1; then NDK="$(command -v ndk-build)";
else echo 'Android NDK required; use GitHub Actions' >&2; exit 1; fi
mkdir -p build/native_obj build/native_libs build/gen build/classes build/dex build/app_assets
"$NDK" NDK_PROJECT_PATH="$PWD/native_internal" \
 APP_BUILD_SCRIPT="$PWD/native_internal/Android.mk" \
 NDK_APPLICATION_MK="$PWD/native_internal/Application.mk" \
 NDK_OUT="$PWD/build/native_obj" NDK_LIBS_OUT="$PWD/build/native_libs" -j2
LIB="$PWD/build/native_libs/arm64-v8a/libhuntares_internal.so"
INJECTOR="$PWD/reference_injector/AndKittyInjector"
[ -s "$LIB" ] || { echo 'Native library was not built' >&2; exit 1; }
[ -s "$INJECTOR" ] || { echo 'Reference AndKittyInjector missing' >&2; exit 1; }
readelf -h "$LIB" | grep -q 'AArch64' || { echo 'Library is not ARM64' >&2; exit 1; }
readelf -h "$INJECTOR" | grep -q 'AArch64' || { echo 'Injector is not ARM64' >&2; exit 1; }
readelf -W --dyn-syms "$LIB" | grep -F JNI_OnLoad >/dev/null || { echo 'JNI_OnLoad export missing' >&2; exit 1; }
rm -rf build/app_assets build/gen build/classes build/dex
mkdir -p build/app_assets build/gen build/classes build/dex
cp -R app/src/main/assets/. build/app_assets/
cp "$LIB" build/app_assets/libhuntares_internal.so
cp "$INJECTOR" build/app_assets/AndKittyInjector
aapt package -f -m -J build/gen -M app/src/main/AndroidManifest.xml \
 -S app/src/main/res -A build/app_assets -I "$ANDROID_JAR"
find app/src/main/java build/gen -name '*.java' > build/sources.txt
javac -source 8 -target 8 -encoding UTF-8 -cp "$ANDROID_JAR" \
 -d build/classes @build/sources.txt
if [ "$DEX" = d8 ]; then
  d8 --lib "$ANDROID_JAR" --output build/dex $(find build/classes -name '*.class')
else
  dx --dex --output=build/dex/classes.dex build/classes
fi
aapt package -f -M app/src/main/AndroidManifest.xml -S app/src/main/res \
 -A build/app_assets -I "$ANDROID_JAR" -F build/unsigned.apk
(cd build/dex && zip -q -u ../unsigned.apk classes.dex)
zipalign -f 4 build/unsigned.apk build/aligned.apk
if [ ! -s build/huntares.keystore ]; then
  keytool -genkeypair -keystore build/huntares.keystore -alias huntares \
    -storepass android -keypass android -keyalg RSA -keysize 2048 -validity 3650 \
    -dname 'CN=Huntares Internal,O=Personal' -noprompt
fi
apksigner sign --ks build/huntares.keystore --ks-key-alias huntares \
 --ks-pass pass:android --key-pass pass:android \
 --out build/HUNTARES_INJECTOR_v33.apk build/aligned.apk
apksigner verify build/HUNTARES_INJECTOR_v33.apk
printf 'APK: %s\n' "$PWD/build/HUNTARES_INJECTOR_v33.apk"
