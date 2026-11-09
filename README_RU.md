# HUNTARES TARGET LOADER v3.6 — ограниченная сборка

Содержимое: оригинальная библиотека `libneptuniumLib.so`, извлечённая из переданного пользователем файла `HUNTARES_target_library.so`; исходный `AndKittyInjector`; приложение с кнопкой INJECT. Нет отдельного Android-оверлейного меню.

**Что НЕ сделано:** ватермарка Neptunium внутри `libneptuniumLib.so` **не удалена**. Бинарник не изменялся, его работоспособность с этим APK и Android не проверена. Встроенный инжектор также является сторонним бинарником, которому потребуется root; его безопасность не аудирована. Этот проект **не готовый проверенный чит**.

## Сборка APK на GitHub

Распаковать архив в новую ветку репозитория HUNTARES и выполнить `git push`. В GitHub Actions появится Build HUNTARES TARGET LOADER v3.6 APK. Artifacts: `HUNTARES-TARGET-LOADER-v36-APK`.

В Termux, после скачивания ZIP в Download:

```sh
mkdir -p ~/HUNTARES-V36
unzip -oq ~/storage/downloads/HUNTARES_TARGET_LOADER_v36_SOURCE.zip -d ~/HUNTARES-V36
cd ~/HUNTARES-V36
git init
git switch -C target-v36
git add -A
git commit -m "HUNTARES v3.6 target loader"
git remote set-url origin https://github.com/aleksejotstavnov83-art/HUNTARES.git 2>/dev/null || git remote add origin https://github.com/aleksejotstavnov83-art/HUNTARES.git
gh auth setup-git
git push -u origin target-v36
```

Сборка сохраняет название приложения HUNTARES CHEAT, запускает AndKittyInjector по нажатию кнопки INJECT и пытается внедрить оригинальную библиотеку. Загрузка и успешная работа чужой библиотеки не подтверждены, возможны проверки/зависимости от исходного лоадера.
