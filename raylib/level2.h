#pragma once
#include "level.h"
#include "../utils.h"
#include <cmath>

// ЭТАЖ 2 «Тишина» — А.У.Р.А. мертва, сортировка почти-отсортированных данных
// Рубильники: BUBBLE (быстро), MERGE (медленно), QUICK (очень медленно)
// На почти отсортированных данных Bubble — лучший выбор

struct Level2 {
    bool completed = false;
    bool badEnding = false;

    // --- Настройки этажа (легко менять) ---
    const int BAR_COUNT = 20;
    const float SORT_STEP_TIME = 0.1f;
    const float TIME_LIMIT = 4.0f;

    Vector2 doorPos;
    float notePosX;
    bool noteActive;

    int bars[20];
    int barsOriginal[20];

    enum State { WAITING, SORTING, SUCCESS, FAIL };
    State state;
    float stateTimer;

    int sortType; // 0=Bubble, 1=Merge, 2=Quick
    int sortOpsDone;
    int sortOpsNeeded;
    float sortTimer;
    float sortElapsed;
    int hoveredSwitch;

    void InitBars() {
        for (int i = 0; i < BAR_COUNT; i++) bars[i] = i + 1;
        // Меняем 2 элемента — массив почти отсортирован
        int tmp = bars[3]; bars[3] = bars[17]; bars[17] = tmp;
        for (int i = 0; i < BAR_COUNT; i++) barsOriginal[i] = bars[i];
    }

    int CountBubbleOps() const {
        int b[20]; for (int i=0;i<BAR_COUNT;i++) b[i]=barsOriginal[i];
        int ops = 0;
        for (int i = 0; i < BAR_COUNT-1; i++)
            for (int j = 0; j < BAR_COUNT-i-1; j++) {
                ops++;
                if (b[j] > b[j+1]) { int t=b[j]; b[j]=b[j+1]; b[j+1]=t; ops++; }
            }
        return ops;
    }

    bool BubbleStep() {
        bool swapped = false;
        for (int j = 0; j < BAR_COUNT-1; j++) {
            if (bars[j] > bars[j+1]) {
                int t = bars[j]; bars[j] = bars[j+1]; bars[j+1] = t;
                swapped = true;
            }
        }
        return !swapped;
    }

    void FullSort() {
        for (int i = 0; i < BAR_COUNT-1; i++)
            for (int j = 0; j < BAR_COUNT-i-1; j++)
                if (bars[j] > bars[j+1]) { int t=bars[j]; bars[j]=bars[j+1]; bars[j+1]=t; }
    }

    void Init(int screenW, int screenH) {
        completed = false;
        noteActive = false;
        state = WAITING;
        stateTimer = 0.0f;
        sortType = -1;
        sortOpsDone = 0;
        sortTimer = 0.0f;
        sortElapsed = 0.0f;
        hoveredSwitch = -1;
        doorPos = { screenW * 0.88f, screenH * 0.5f };
        notePosX = screenW * 0.08f;
        InitBars();
    }

    void Update(Player& player) {
        noteActive = player.is_in_area(notePosX);
        float dt = GetFrameTime();

        if (state == WAITING) {
            hoveredSwitch = -1;
            float sw = GetScreenWidth();
            float switchX[3] = { sw*0.25f, sw*0.45f, sw*0.65f };
            for (int i = 0; i < 3; i++) {
                if (player.is_in_area(switchX[i])) {
                    hoveredSwitch = i;
                    if (IsKeyPressed(KEY_E)) {
                        sortType = i;
                        sortOpsDone = 0;
                        sortTimer = 0.0f;
                        sortElapsed = 0.0f;
                        if (sortType == 0) sortOpsNeeded = CountBubbleOps();
                        else if (sortType == 1) sortOpsNeeded = BAR_COUNT * 5;
                        else sortOpsNeeded = BAR_COUNT * BAR_COUNT / 2;
                        state = SORTING;
                    }
                }
            }
        }
        else if (state == SORTING) {
            sortElapsed += dt;
            sortTimer += dt;
            while (sortTimer >= SORT_STEP_TIME) {
                sortTimer -= SORT_STEP_TIME;
                sortOpsDone++;
                if (sortType == 0) {
                    if (BubbleStep()) { state = SUCCESS; stateTimer = 0.0f; return; }
                } else {
                    if (sortOpsDone >= sortOpsNeeded) {
                        FullSort();
                        state = FAIL; stateTimer = 0.0f; return;
                    }
                }
                if (sortElapsed > TIME_LIMIT) { state = FAIL; stateTimer = 0.0f; return; }
            }
        }
        else if (state == FAIL) {
            stateTimer += dt;
            if (stateTimer > 3.0f) {
                for (int i = 0; i < BAR_COUNT; i++) bars[i] = barsOriginal[i];
                state = WAITING;
            }
        }
        else if (state == SUCCESS) {
            if (player.is_in_area(doorPos.x) && IsKeyPressed(KEY_E)) completed = true;
        }
    }

    void Draw(Player& player, Assets& assets) {
        int scrW = GetScreenWidth();
        int scrH = GetScreenHeight();
        ClearBackground(Color{ 12, 8, 10, 255 });

        float floorY = scrH * 0.82f;
        if (assets.tex_floor.width > 0) {
            int tileCount = scrW / assets.tex_floor.width + 2;
            for (int i = 0; i < tileCount; i++) {
                DrawTexture(assets.tex_floor, i * assets.tex_floor.width, (int)floorY, WHITE);
            }
            DrawRectangle(0, (int)(floorY + assets.tex_floor.height), scrW, scrH - (int)(floorY + assets.tex_floor.height), Color{18, 18, 22, 255});
        }

        if (assets.tex_wall.width > 0) {
            int wallTileCount = scrW / assets.tex_wall.width + 2;
            for (int i = 0; i < wallTileCount; i++) {
                DrawTexture(assets.tex_wall, i * assets.tex_wall.width, (int)(floorY - assets.tex_wall.height), WHITE);
            }
        }

        RuText(assets.font, u8"ЭТАЖ 2 : ТИШИНА", scrW/2 - 130, 30, 26, Color{180,60,60,255});
        RuText(assets.font, u8"А.У.Р.А. — нет сигнала", scrW/2 - 130, 65, 18, Color{120,50,50,255});

        // Сломанный терминал
        DrawRectangle(scrW/2-150, 100, 300, 180, Color{15,10,12,255});
        DrawRectangleLines(scrW/2-150, 100, 300, 180, Color{60,30,30,255});
        DrawLine(scrW/2-50, 100, scrW/2+30, 280, Color{40,25,25,255});
        DrawLine(scrW/2+80, 120, scrW/2-20, 250, Color{40,25,25,255});

        // Монитор с данными
        float monX = scrW*0.20f, monY = scrH*0.28f;
        float monW = scrW*0.60f, monH = scrH*0.20f;
        DrawRectangle((int)monX,(int)monY,(int)monW,(int)monH, Color{10,10,15,255});
        DrawRectangleLines((int)monX,(int)monY,(int)monW,(int)monH, Color{60,60,80,255});
        RuText(assets.font, u8"Данные почти отсортированы!", (int)(monX+10), (int)(monY-20), 14, YELLOW);

        // Столбики
        float barW = (monW - 40) / BAR_COUNT - 2;
        for (int i = 0; i < BAR_COUNT; i++) {
            float h = (bars[i]/(float)BAR_COUNT) * (monH-20);
            float x = monX + 20 + i*(barW+2);
            float y = monY + monH - 10 - h;
            Color c = (state==SUCCESS) ? GREEN : SKYBLUE;
            DrawRectangle((int)x,(int)y,(int)barW,(int)h, c);
        }

        // Три рубильника
        float switchX[3] = { scrW*0.25f, scrW*0.45f, scrW*0.65f };
        float switchY = scrH * 0.55f;
        const char* switchLabels[3] = { "BUBBLE", "MERGE", "QUICK" };
        Color switchColors[3] = { {60,180,60,255}, {180,180,60,255}, {180,60,60,255} };

        for (int i = 0; i < 3; i++) {
            Color box = switchColors[i];
            if (hoveredSwitch == i) box = Fade(box, 0.7f);
            DrawRectangle((int)switchX[i], (int)switchY, 120, 80, box);
            DrawRectangleLines((int)switchX[i], (int)switchY, 120, 80, RAYWHITE);
            float ly = (state==SORTING && sortType==i) ? switchY+15 : switchY+45;
            DrawRectangle((int)(switchX[i]+50),(int)ly, 20, 30, GRAY);
            DrawCircle((int)(switchX[i]+60),(int)(ly+15), 8, BLACK);
            int lw = RuMeasure(assets.font, switchLabels[i], 16);
            RuText(assets.font, switchLabels[i], (int)(switchX[i]+60-lw/2), (int)(switchY+85), 16, RAYWHITE);
            if (hoveredSwitch == i && state == WAITING)
                RuText(assets.font, u8"[E]", (int)(switchX[i]+45), (int)(switchY-22), 14, YELLOW);
        }

        // Сообщения
        if (state == SORTING) {
            RuText(assets.font, TextFormat("Сортировка %s... %.1fs", switchLabels[sortType], sortElapsed),
                   scrW/2-180, scrH*0.72f, 20, RAYWHITE);
        } else if (state == FAIL) {
            RuText(assets.font, u8"Слишком долго. Данные не вывезли.", scrW/2-220, scrH*0.72f, 22, RED);
            RuText(assets.font, u8"Попробуй другой алгоритм.", scrW/2-160, scrH*0.78f, 16, GRAY);
        } else if (state == SUCCESS) {
            RuText(assets.font, u8"ОТЛИЧНО! Путь открыт.", scrW/2-160, scrH*0.72f, 24, GREEN);
            float doorScale = 1.5f;
            int doorH = (int)(assets.tex_elevator_opened.height * doorScale);
            DrawTextureEx(assets.tex_elevator_opened, {doorPos.x, doorPos.y - doorH}, 0.0f, doorScale, WHITE);
            RuText(assets.font, u8"[E] ВЫЙТИ", (int)doorPos.x-20, (int)doorPos.y-40, 15, GREEN);
        }

        if (state != SUCCESS) {
            float doorScale = 1.5f;
            int doorH = (int)(assets.tex_elevator_closed.height * doorScale);
            DrawTextureEx(assets.tex_elevator_closed, {doorPos.x, doorPos.y - doorH}, 0.0f, doorScale, WHITE);
        }

        // Записка
        DrawTextureEx(assets.tex_note, { notePosX, scrH * 0.715f }, 0.0f, 0.15f, WHITE);
        if (noteActive) {
            int nx = scrW/2-420, ny = scrH/2-220;
            DrawRectangle(nx, ny, 840, 440, Fade(BLACK, 0.92f));
            DrawRectangleLines(nx, ny, 840, 440, GOLD);
            RuText(assets.font, u8"--- ЗАПИСКА ---", nx+40, ny+35, 28, GOLD);
            RuText(assets.font, u8"Центральный кластер завис.", nx+40, ny+95, 24, RAYWHITE);
            RuText(assets.font, u8"Я ж сисадмин... в смысле, физик.", nx+40, ny+135, 24, RAYWHITE);
            RuText(assets.font, u8"Ладно, починю. Тут делов на пять минут.", nx+40, ny+175, 24, RAYWHITE);
            RuText(assets.font, u8"Хотя откуда я вообще это знаю?..", nx+40, ny+225, 24, Color{200,180,80,255});
        }

        player.DrawSprite(assets);
    }
};
