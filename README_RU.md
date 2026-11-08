# HUNTARES INJECTOR v3.5 — HUNTARES CHEAT native menu

## Changed

- Integrated BaseMenuAIDE rendering sources as independent GLES2 floating overlay.
- Native menu title is **HUNTARES CHEAT**; old **BASE MENU** title is removed.
- Removed the old non-interactive game-frame ImGui `CHAMS TEST` window and disabled drawing it into game EGL frames.
- Bottom floating button `HUNTARES CHEAT | МЕНЮ` opens the touch-capable Android window; X closes it.
- Retained Home / Chams / Settings pages with working native menu scale, watermark and accent settings.
- Removed the skin gallery, its Activity, assets, and button.
- Retained the previous injector loader and library as-is. **No game hacks were verified.**

## Chams — IMPORTANT

This release **does not contain working player Chams**. The supplied `chams.hpp` references unresolved
IL2CPP/Unity APIs and missing original `offset.hpp` members and a Dobby hook, and the shader bundle is
source code only rather than a compiled Unity AssetBundle compatible with Standoff 2 1.1.0.
The CHAMS page shows this status; it intentionally does not offer a misleading checkbox.

## Build GitHub Actions from Termux

1. Download `HUNTARES_INJECTOR_v35_HUNTARES_MENU_SOURCE.zip` to `Download`.
2. Run:

```sh
pkg install git gh unzip -y
termux-setup-storage
mkdir -p ~/HUNTARES-INJECTOR-V35
unzip -oq ~/storage/downloads/HUNTARES_INJECTOR_v35_HUNTARES_MENU_SOURCE.zip -d ~/HUNTARES-INJECTOR-V35
cd ~/HUNTARES-INJECTOR-V35
git init
git switch -C injector-v35
git add -A
git commit -m 'HUNTARES v3.5 native AIDE menu, no skins'
git remote get-url origin >/dev/null 2>&1 && git remote set-url origin https://github.com/aleksejotstavnov83-art/HUNTARES.git || git remote add origin https://github.com/aleksejotstavnov83-art/HUNTARES.git
gh auth setup-git
git push -u origin injector-v35
```

3. Open https://github.com/aleksejotstavnov83-art/HUNTARES/actions .
4. After a successful run, open Artifacts -> `HUNTARES-INJECTOR-v35-APK`, unzip it, install the APK.
5. Permit "Display over other apps" to show the Android touch menu. In-app menu is independent of injection success.

## Limitations / risks

The ZIP is **source code**, not a tested APK. Android SDK/NDK build and in-game touch behavior have not
been verified on an Android device. Overlay permission is required. A third-party `AndKittyInjector`
binary from a user-supplied APK is included as-is, has not been independently audited, and will run
with root privileges if INJECT is pressed. Review it before using. No SELinux bypass is implemented.
