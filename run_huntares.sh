#!/system/bin/sh
# HUNTARES INTERNAL v3.2 launcher -- requires installed/active Zygisk module.
set -eu
PACKAGE=com.axlebolt.standoff2
MODULE_DIR=/data/adb/modules/huntares_internal
if [ "$(id -u)" != 0 ]; then
  echo "[HUNTARES] Root required: su -c sh /data/local/tmp/run_huntares.sh"
  exit 1
fi
if [ ! -f "$MODULE_DIR/module.prop" ] || [ ! -s "$MODULE_DIR/zygisk/arm64-v8a.so" ]; then
  echo "[HUNTARES] Internal module not installed. Install HUNTARES_INTERNAL_CHAMS_v32_MODULE.zip in KernelSU/Magisk and reboot."
  exit 2
fi
if [ -f "$MODULE_DIR/disable" ]; then
  echo "[HUNTARES] Module disabled in root manager. Enable and reboot."
  exit 3
fi
if ! pm path "$PACKAGE" >/dev/null 2>&1; then
  echo "[HUNTARES] Standoff 2 not installed."
  exit 4
fi
echo "[HUNTARES] Launching $PACKAGE; Zygisk must already be enabled."
am force-stop "$PACKAGE" >/dev/null 2>&1 || :
monkey -p "$PACKAGE" -c android.intent.category.LAUNCHER 1
printf '%s\n' '[HUNTARES] To check injection: su -c logcat -d -s HUNTARES_INTERNAL'
