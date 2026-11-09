#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")"
for cmd in aapt javac zip apksigner zipalign keytool readelf; do
  command -v "$cmd" >/dev/null || { echo "Missing tool: $cmd" >&2; exit 1; }
done
: "${ANDROID_HOME:?Android SDK required}"
ANDROID_JAR="${ANDROID_JAR:-$ANDROID_HOME/platforms/android-35/android.jar}"
test -f "$ANDROID_JAR"
if command -v d8 >/dev/null 2>&1; then DEX=d8;
elif command -v dx >/dev/null 2>&1; then DEX=dx;
else echo 'Missing d8/dx' >&2; exit 1; fi
mkdir -p build/gen build/classes build/dex build/app_assets
INJECTOR="$PWD/reference_injector/AndKittyInjector"
MODULE="$PWD/app/src/main/assets/libneptuniumLib.so"
[ -s "$INJECTOR" ] && [ -s "$MODULE" ]
readelf -h "$INJECTOR" | grep -q AArch64
readelf -h "$MODULE" | grep -q AArch64
readelf -W --dyn-syms "$MODULE" | grep -F JNI_OnLoad >/dev/null
rm -rf build/gen build/classes build/dex build/app_assets
mkdir -p build/gen build/classes build/dex build/app_assets
cp "$MODULE" build/app_assets/libneptuniumLib.so
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
    --out build/HUNTARES_TARGET_LOADER_v36.apk build/aligned.apk
apksigner verify build/HUNTARES_TARGET_LOADER_v36.apk
printf 'APK: %s\n' "$PWD/build/HUNTARES_TARGET_LOADER_v36.apk"
