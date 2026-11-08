# HUNTARES INTERNAL — CHAMS TEST v3.2

**Статус: экспериментальное меню, не рабочие Chams.**

Сохранён предыдущий код HUNTARES INTERNAL v3.1 с EGL/ImGui и Zygisk. Меню изменено: прежняя нижняя центральная полоска (BYCMD-подобная кнопка `HUNTARES | MENU`) открывает/закрывает единственный раздел `Chams` и выбор цвета. При переключении появляется сообщение в logcat. **Подсветки игроков через стены пока нет.**

Почему: `reference_chams/chams_original.hpp` требует заголовки IL2CPP, Dobby, файл offset.hpp (его нет в предоставленном модуле), загрузку Unity AssetBundle и рабочее определение мешей игроков 1.1.0. `CharacterMultipass.shader` — текстовый Unity shader, а **не** готовый Android AssetBundle. В оригинале неправильно вычисляется база `libil2cpp.so`, создаётся MeshRenderer без геометрии. Подключение как есть вызовет ошибки/сбой, а не работающую подсветку.

## Сборка без APK

Содержимое ZIP загрузить в отдельную ветку GitHub. Запустить workflow `Build HUNTARES INTERNAL v3.2 Chams Menu (no APK)` в Actions. После успешной сборки артефакт `HUNTARES-INTERNAL-CHAMS-v32` содержит:
- `libhuntares_internal.so` — **библиотека**, отдельно не исполняется;
- `HUNTARES_INTERNAL_CHAMS_v32_MODULE.zip` — Zygisk-модуль;
- `run_huntares.sh` — скрипт запуска после установки модуля.

На Android нужен root и совместимый Zygisk. Установить ZIP-модуль через менеджер root, перезагрузиться, затем в Termux:

```sh
su -c 'sh /data/local/tmp/run_huntares.sh'
su -c 'logcat -d -s HUNTARES_INTERNAL'
```

Этот ZIP содержит только **исходники**, потому что Android NDK в среде подготовки отсутствует. Сборка APK здесь не требуется. `chmod 777` не нужен — достаточно 700, если потребуется.

**Тест**: появляется меню? Нижняя полоска работает? Переключение Chams выводит лог `NO EFFECT applied`? Это подтверждает загрузку и кнопки, но не рендер шейдера на игроке.
