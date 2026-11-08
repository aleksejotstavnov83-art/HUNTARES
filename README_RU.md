# HUNTARES INJECTOR v3.3 — Android ARM64 APK source

## Что взято из присланного base (8).apk

Из пользовательского APK извлечён `assets/AndKittyInjector` — ARM64 ELF executable,
в котором обнаружены сообщения `ptrace`, `dlopen`, `--package`, `--libs`.
Это **не** библиотека чита. Точная логика вызова бинарника в стороннем APK
статически не установлена. Binary включён по просьбе пользователя как reference input.
Проект AndKittyInjector: https://github.com/MJx0/AndKittyInjector (MIT).

## Что сделано в HUNTARES

* Твой фон HUNTARES INJECTOR, прежняя кнопка INJECT и Skin Catalog.
* APK теперь извлекает `AndKittyInjector` и собранный `libhuntares_internal.so`
  и через root запускает инжектор **без установки Zygisk-модуля**.
* Нативный модуль экспортирует `JNI_OnLoad`, вызывающий `HuntaresInternalStart()`.
  В версиях v3.1/3.2 этой точки входа не было.
* Меню ImGui — HUNTARES CHEAT, нижняя BYCMD-полоска, один пункт Chams.
* Исходные 1.1.0 camera/transform reference offsets сохранены.

## Ограничения — прочитать обязательно

**Рабочая инъекция в Standoff 2 НЕ подтверждена.** Устройство может запретить
ptrace/exec/загрузку. Без запуска на ARM64 гарантировать результат нельзя.
**Chams пока только UI-переключатель** — в исходных файлах нет рабочего Unity
AssetBundle и привязки материалов к рендерам игроков 1.1.0. Скрипт ничего не
обходит автоматически и не делает нерабочую функцию рабочей.
Skin Catalog — локальная галерея, не реальный скинченджер.

## Сборка APK через GitHub

1. Создай ветку `injector-v33` в своём GitHub-репозитории (или новый репозиторий).
2. Загрузи **содержимое архива в корень**, включая `.github/workflows/build-apk.yml`.
3. GitHub Actions → `Build HUNTARES INJECTOR v3.3 APK` → проверь результат.
4. После успешной сборки скачай `HUNTARES-INJECTOR-v33-APK` (ZIP-артефакт),
   распакуй APK и установи. Доступ root потребуется при нажатии INJECT.

## Логи

Если INJECT завершился с ошибкой, смотри Android Logcat по тегам
`HUNTARES_LOADER` и `HUNTARES_INTERNAL`. Установи root-права приложению,
но не отключай защиту Android целиком ради теста. Нативный инжектор запускается
из `/data/local/tmp` только с root.

## Авторство

AndKittyInjector: MJx0 и участники, https://github.com/MJx0/AndKittyInjector.
Исходный бинарник извлечён из пользовательского reference APK; его исходный build
и неизменность по отношению к upstream **не подтверждены**.
SHA256: `a5c497c1221cb59541307a7f1f373a79747fe41813dd1d00510e7fe50b3a18e4`.
**Безопасность:** бинарник из стороннего APK не аудирован. Root-доступ даёт ему широкие права.
Для распространения рекомендуется заменить его собственной сборкой официального AndKittyInjector.
Остальные кодовые компоненты происходят из архивов HUNTARES/Ximira, присланных
пользователем ранее.
