#!/data/data/com.termux/files/usr/bin/bash
set -euo pipefail
cd "$(dirname "$0")"
for tool in aapt javac zip apksigner keytool; do
  command -v "$tool" >/dev/null || { echo "Не найден $tool"; exit 1; }
done
ANDROID_JAR="${ANDROID_JAR:-${ANDROID_HOME:-$HOME/HunterInjector/sdk}/platforms/android-35/android.jar}"
if [ ! -f "$ANDROID_JAR" ] && [ -f "$HOME/HunterInjector/sdk/android-35/android.jar" ]; then ANDROID_JAR="$HOME/HunterInjector/sdk/android-35/android.jar"; fi
[ -f "$ANDROID_JAR" ] || { echo "Не найден android.jar: $ANDROID_JAR"; exit 1; }
if command -v d8 >/dev/null; then DEX=d8; elif command -v dx >/dev/null; then DEX=dx; else echo 'Нет d8/dx'; exit 1; fi
if [ -n "${NDK_BUILD:-}" ] && [ -x "$NDK_BUILD" ]; then NDK="$NDK_BUILD";
elif command -v ndk-build >/dev/null 2>&1; then NDK="$(command -v ndk-build)";
elif [ -n "${ANDROID_NDK_HOME:-}" ] && [ -x "$ANDROID_NDK_HOME/ndk-build" ]; then NDK="$ANDROID_NDK_HOME/ndk-build";
else echo 'Нет ndk-build. Для сборки с телефона используй GitHub Actions из файла .github/workflows/build-apk.yml'; exit 1; fi
mkdir -p app/src/main/assets build/gen build/classes build/dex
"$NDK" NDK_PROJECT_PATH="$PWD/native_huntares" APP_BUILD_SCRIPT="$PWD/native_huntares/jni/Android.mk" \
  NDK_OUT="$PWD/native_huntares/build/obj" NDK_LIBS_OUT="$PWD/native_huntares/build/libs" -j2
BIN="$PWD/native_huntares/build/libs/arm64-v8a/huntares_cheat"
[ -s "$BIN" ] || { echo "Не собран HUNTARES CHEAT: $BIN"; exit 1; }
cp "$BIN" app/src/main/assets/huntares-cheat-1.1.0
rm -rf build/gen/* build/classes/* build/dex/*
aapt package -f -m -J build/gen -M app/src/main/AndroidManifest.xml -S app/src/main/res \
    -A app/src/main/assets -I "$ANDROID_JAR"
find app/src/main/java build/gen -name '*.java' > build/sources.txt
javac -source 8 -target 8 -encoding UTF-8 -cp "$ANDROID_JAR" -d build/classes @build/sources.txt
if [ "$DEX" = d8 ]; then
  d8 --lib "$ANDROID_JAR" --output build/dex $(find build/classes -name '*.class')
else dx --dex --output=build/dex/classes.dex build/classes; fi
aapt package -f -M app/src/main/AndroidManifest.xml -S app/src/main/res \
    -A app/src/main/assets -I "$ANDROID_JAR" -F build/unsigned.apk
(cd build/dex && zip -q -u ../unsigned.apk classes.dex)
if [ ! -f build/huntares.keystore ]; then
 keytool -genkeypair -keystore build/huntares.keystore -alias huntares \
 -storepass android -keypass android -keyalg RSA -keysize 2048 -validity 3650 \
 -dname 'CN=Huntares,O=Personal' -noprompt
fi
apksigner sign --ks build/huntares.keystore --ks-key-alias huntares \
 --ks-pass pass:android --key-pass pass:android \
 --out build/HUNTARES_CHEAT_v19.apk build/unsigned.apk
apksigner verify build/HUNTARES_CHEAT_v19.apk
mkdir -p "$HOME/storage/downloads" 2>/dev/null || true
cp build/HUNTARES_CHEAT_v19.apk "$HOME/storage/downloads/" 2>/dev/null || true
echo "Готово: $PWD/build/HUNTARES_CHEAT_v19.apk"
