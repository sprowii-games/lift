# LIFT

Короткий психологический хоррор про побег из лаборатории на **raylib 5.5**.
Четыре этажа, лифт, записки и головоломки по физике.

## Скачать

Готовые сборки лежат в [Releases](https://github.com/sprowii-games/lift/releases):

| Платформа | Файл | Что внутри |
|---|---|---|
| Windows x64 | `LIFT-windows-x64.zip` | `LIFT.exe` + `textures/`, `sounds/`, шрифт |
| Web | `LIFT-web.zip` | `index.html`, `index.js`, `index.wasm`, `index.data` |

Веб-версию также можно запускать прямо в браузере: **https://sprowii-games.github.io/lift/**
(если страница ещё не открывается — включите GitHub Pages в настройках репозитория).

### Управление

- `A` / `D` — идти
- `Shift` — бежать
- `E` — действие / ответ
- `R` — рестарт этажа
- `Esc` — выход

## Сборка

### CMake (Windows / Linux / macOS)

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release --parallel
```

raylib 5.5 подтягивается автоматически через CMake `FetchContent`.
Чтобы получить папку, которую можно просто заархивировать:

```bash
cmake --install build --prefix dist
```

Запускать игру нужно из папки, где лежат `textures/`, `sounds/` и `handjet.ttf`
(либо из корня репозитория: `./build/LIFT`). Если ассеты не найдены, игра
попробует перейти в папку своего `.exe`.

### Web (Emscripten)

```bash
emcmake cmake -S . -B build-web -DCMAKE_BUILD_TYPE=Release -DPLATFORM=Web
cmake --build build-web --parallel
python3 -m http.server 8080 -d build-web
```

Откройте `http://localhost:8080`. Ассеты упаковываются в `index.data`,
поэтому веб-сборке не нужен отдельный веб-сервер для текстур и звуков.
Оболочка (`web/shell.html`) сама масштабирует canvas под окно и даёт кнопки
звука и фуллскрина.

### Visual Studio

Старый проект `raylib.sln` остался на месте и собирается через NuGet-пакет
raylib 5.5.0 (`raylib/packages.config`). CMake-сборка удобнее для CI и веба.

## Релизы и CI

Workflow [`.github/workflows/build-release.yml`](.github/workflows/build-release.yml)
собирает обе версии по тегу:

```bash
git tag v0.1.0
git push origin v0.1.0
```

Что произойдёт:

1. `windows-latest` соберёт `LIFT.exe` и упакует его с ассетами в zip.
2. `ubuntu-latest` соберёт веб-версию через Emscripten.
3. Создастся GitHub Release `LIFT v0.1.0` с двумя архивами и `SHA256SUMS.txt`.
4. Веб-сборка задеплоится на GitHub Pages (job `pages`).

Workflow можно запустить и вручную из вкладки **Actions → Build & Release → Run workflow**.

## Структура

```
raylib/            исходники игры (main, player, camera, уровни, ассеты)
textures/          спрайты и фоны
sounds/            музыка и SFX
web/shell.html     HTML-оболочка для веб-сборки
CMakeLists.txt     сборка desktop + web
raylib.sln         старый проект Visual Studio
utils.h            математические хелперы
dizdoc.md          дизайн-документ
```

## Известные мелочи

Код загружает два файла, которых в репозитории нет:

- `sounds/menu.ogg` — поэтому меню сейчас без музыки;
- `textures/terminal.png` — терминал на первом этаже рисуется без спрайта
  (подсказка `[E]` и логика работают).

Просто положите файлы с такими именами в `sounds/` и `textures/` — сборка
подхватит их автоматически.
