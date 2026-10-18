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

### Если веб-версия показывает чёрный экран

1. **Не открывайте `index.html` двойным кликом.** По протоколу `file://` браузер
   не отдаёт `.wasm` и `.data`, поэтому страница остаётся чёрной. Запускайте через
   сервер: `start-web.bat` (Windows) или `./start-web.sh` (Linux/macOS) из архива,
   либо вручную:
   ```bash
   python3 -m http.server 8080
   ```
   и открывайте `http://localhost:8080`.
2. Если экран всё равно чёрный — веб-оболочка (`web/shell.html`) показывает панель
   с текстом ошибки и последними строками лога Emscripten прямо на странице.
   Пришлите текст из этой панели — по нему видно причину.

Веб-версия — это **тот же самый код** `raylib/*.cpp`, `raylib/level*.h`, скомпилированный
Emscripten'ом. Никакой отдельной «веб-игры» нет: единственное отличие — HTML-оболочка
(`web/shell.html`) и отсутствие принудительного фуллскрина при старте.

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

### Мышь на вебе

В raylib/emscripten координаты мыши приходят в CSS-пикселях, а игра работает
в пространстве 1920x1080. Если растягивать `<canvas>` через `width/height`,
курсор начинает «уезжать»: подсказки срабатывают ниже и левее реальной точки.
Поэтому в `web/shell.html` canvas всегда имеет layout-размер ровно 1920x1080,
а вписывается в окно через `transform: scale()`. Дополнительно `main.cpp`
на вебе каждый кадр пересчитывает `SetMouseScale` по реальному размеру canvas.
Клавиша `F1` включает HUD с числами (буфер, layout, окно, экран игры, мышь) —
по нему видно, совпадают ли координаты.

## Известные мелочи

- `sounds/menu.ogg` в репозитории нет — меню сейчас без музыки. Положите файл
  с таким именем в `sounds/`, и сборка подхватит его автоматически.
- `textures/terminal.png` добавлен: терминал на первом этаже теперь рисуется
  спрайтом, по центру зоны взаимодействия и ровно на полу. Если файл убрать,
  включится простая заглушка-корпус, логика не сломается.
