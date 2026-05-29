#pragma once
#include "level.h"
#include "../utils.h"
#include <cmath>

struct Level6 {
    bool completed = false;
    bool badEnding = false;

    static const int MAZE_W = 15;
    static const int MAZE_H = 11;

    Vector2 doorPos;
    float notePosX;
    bool noteNear;
    bool noteOpen;

    float floorY;
    float termX;

    int maze[11][15];

    int bfsPath[165][2];
    int bfsPathLen;
    int dfsPath[165][2];
    int dfsPathLen;

    enum State { WAITING, ANIMATING, SUCCESS, FAIL };
    State state;
    float stateTimer;

    int chosenAlgo;
    int animStep;
    float animTimer;
    int hoveredBtn;

    void InitMaze() {
        int m[11][15] = {
            {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
            {0,1,1,1,0,1,1,1,1,1,0,1,1,1,0},
            {0,1,0,1,0,1,0,0,0,1,0,1,0,1,0},
            {0,1,0,1,1,1,0,1,1,1,0,1,0,1,0},
            {0,1,0,0,0,0,0,1,0,0,0,1,0,1,0},
            {0,1,1,1,1,1,1,1,1,1,1,1,0,1,0},
            {0,0,0,0,0,0,0,0,0,0,0,1,0,1,0},
            {0,1,1,1,1,1,1,1,1,1,1,1,1,1,0},
            {0,1,0,0,0,0,1,0,0,0,0,0,0,0,0},
            {0,1,1,1,1,1,1,1,1,1,1,1,1,1,0},
            {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        };
        for (int y = 0; y < MAZE_H; y++)
            for (int x = 0; x < MAZE_W; x++)
                maze[y][x] = m[y][x];
    }

    void ComputeBFS() {
        int visited[11][15] = {};
        int parent[11][15][2] = {};
        for (int y=0;y<MAZE_H;y++) for(int x=0;x<MAZE_W;x++) { parent[y][x][0]=-1; parent[y][x][1]=-1; }

        int queue[165][2] = {}; int qHead=0, qTail=0;
        queue[qTail][0]=1; queue[qTail][1]=1; qTail++;
        visited[1][1] = 1;

        int dx[4] = {0,1,0,-1};
        int dy[4] = {-1,0,1,0};

        while (qHead < qTail) {
            int cx = queue[qHead][0], cy = queue[qHead][1]; qHead++;
            if (cx==13 && cy==9) break;
            for (int d=0; d<4; d++) {
                int nx = cx+dx[d], ny = cy+dy[d];
                if (nx>=0 && nx<MAZE_W && ny>=0 && ny<MAZE_H && maze[ny][nx]==1 && !visited[ny][nx]) {
                    visited[ny][nx] = 1;
                    parent[ny][nx][0] = cx; parent[ny][nx][1] = cy;
                    queue[qTail][0]=nx; queue[qTail][1]=ny; qTail++;
                }
            }
        }
        bfsPathLen = 0;
        int cx=13, cy=9;
        while (cx != -1) {
            bfsPath[bfsPathLen][0] = cx; bfsPath[bfsPathLen][1] = cy;
            bfsPathLen++;
            int px = parent[cy][cx][0], py = parent[cy][cx][1];
            cx = px; cy = py;
        }
        for (int i=0; i<bfsPathLen/2; i++) {
            int tx=bfsPath[i][0], ty=bfsPath[i][1];
            bfsPath[i][0]=bfsPath[bfsPathLen-1-i][0]; bfsPath[i][1]=bfsPath[bfsPathLen-1-i][1];
            bfsPath[bfsPathLen-1-i][0]=tx; bfsPath[bfsPathLen-1-i][1]=ty;
        }
    }

    void ComputeDFS() {
        int visited[11][15] = {};
        dfsPathLen = 0;
        int stack[165][2] = {}; int sTop=0;
        int parent[11][15][2] = {};
        for (int y=0;y<MAZE_H;y++) for(int x=0;x<MAZE_W;x++) { parent[y][x][0]=-1; parent[y][x][1]=-1; }

        stack[sTop][0]=1; stack[sTop][1]=1; sTop++;
        visited[1][1] = 1;

        int dx[4] = {0,-1,0,1};
        int dy[4] = {-1,0,1,0};

        while (sTop > 0) {
            sTop--;
            int cx = stack[sTop][0], cy = stack[sTop][1];
            if (cx==13 && cy==9) break;
            for (int d=0; d<4; d++) {
                int nx = cx+dx[d], ny = cy+dy[d];
                if (nx>=0 && nx<MAZE_W && ny>=0 && ny<MAZE_H && maze[ny][nx]==1 && !visited[ny][nx]) {
                    visited[ny][nx] = 1;
                    parent[ny][nx][0] = cx; parent[ny][nx][1] = cy;
                    stack[sTop][0]=nx; stack[sTop][1]=ny; sTop++;
                }
            }
        }
        dfsPathLen = 0;
        int cx=13, cy=9;
        while (cx != -1) {
            dfsPath[dfsPathLen][0] = cx; dfsPath[dfsPathLen][1] = cy;
            dfsPathLen++;
            int px = parent[cy][cx][0], py = parent[cy][cx][1];
            cx = px; cy = py;
        }
        for (int i=0; i<dfsPathLen/2; i++) {
            int tx=dfsPath[i][0], ty=dfsPath[i][1];
            dfsPath[i][0]=dfsPath[dfsPathLen-1-i][0]; dfsPath[i][1]=dfsPath[dfsPathLen-1-i][1];
            dfsPath[dfsPathLen-1-i][0]=tx; dfsPath[dfsPathLen-1-i][1]=ty;
        }
    }

    void Init(int screenW, int screenH) {
        completed = false;
        badEnding = false;
        noteNear = false;
        noteOpen = false;
        state = WAITING;
        stateTimer = 0.0f;
        chosenAlgo = -1;
        animStep = 0;
        animTimer = 0.0f;
        hoveredBtn = -1;

        floorY = screenH * 0.82f;
        doorPos = { screenW * 0.85f, floorY };
        notePosX = screenW * 0.20f;
        termX = screenW * 0.45f;

        InitMaze();
        ComputeBFS();
        ComputeDFS();
    }

    void Update(Player& player) {
        float dt = GetFrameTime();

        if (noteOpen) {
            player.stop_brother();
            if (IsKeyPressed(KEY_E) || IsKeyPressed(KEY_BACKSPACE)) {
                noteOpen = false;
                player.can_move = true;
            }
            return;
        }

        noteNear = player.is_in_area(notePosX);

        if (state == WAITING) {
            if (noteNear && IsKeyPressed(KEY_E)) {
                noteOpen = true;
                player.stop_brother();
                return;
            }

            hoveredBtn = -1;
            float dfsBtnX = termX - 80;
            float bfsBtnX = termX + 80;
            if (player.is_in_area(dfsBtnX)) hoveredBtn = 0;
            if (player.is_in_area(bfsBtnX)) hoveredBtn = 1;
            if (hoveredBtn >= 0 && IsKeyPressed(KEY_E)) {
                chosenAlgo = hoveredBtn;
                animStep = 0;
                animTimer = 0.0f;
                state = ANIMATING;
            }
        }
        else if (state == ANIMATING) {
            animTimer += dt;
            int totalSteps = (chosenAlgo==1) ? bfsPathLen : dfsPathLen;
            float stepTime = 0.05f;
            while (animTimer >= stepTime && animStep < totalSteps) {
                animTimer -= stepTime;
                animStep++;
            }
            if (animStep >= totalSteps) {
                if (chosenAlgo == 1) {
                    state = SUCCESS;
                    stateTimer = 0.0f;
                } else {
                    state = FAIL;
                    stateTimer = 0.0f;
                }
            }
        }
        else if (state == FAIL) {
            stateTimer += dt;
            if (stateTimer > 3.0f) state = WAITING;
        }
        else if (state == SUCCESS) {
            if (player.is_in_area(doorPos.x) && IsKeyPressed(KEY_E)) {
                completed = true;
            }
        }
    }

    void DrawLabTable(float cx, float surfY, float w, float surfH) {
        DrawRectangle((int)(cx - w / 2), (int)surfY, (int)w, (int)surfH, Color{ 45, 38, 30, 255 });
        DrawRectangleLines((int)(cx - w / 2), (int)surfY, (int)w, (int)surfH, Color{ 85, 70, 50, 255 });
        float legW = 6.0f;
        float legTop = surfY + surfH;
        float legH = floorY - legTop;
        if (legH > 0) {
            Color legCol = { 55, 48, 38, 255 };
            DrawRectangle((int)(cx - w / 2 + 5), (int)legTop, (int)legW, (int)legH, legCol);
            DrawRectangle((int)(cx + w / 2 - 5 - legW), (int)legTop, (int)legW, (int)legH, legCol);
        }
    }

    void Draw(Player& player, Assets& assets) {
        int scrW = GetScreenWidth();
        int scrH = GetScreenHeight();
        ClearBackground(Color{ 15, 18, 22, 255 });

        if (assets.tex_wall.width > 0) {
            int wallTileCount = scrW / assets.tex_wall.width + 2;
            for (int i = 0; i < wallTileCount; i++) {
                DrawTexture(assets.tex_wall, i * assets.tex_wall.width, (int)(floorY - assets.tex_wall.height), WHITE);
            }
        }

        if (assets.tex_floor.width > 0) {
            int tileCount = scrW / assets.tex_floor.width + 2;
            for (int i = 0; i < tileCount; i++) {
                DrawTexture(assets.tex_floor, i * assets.tex_floor.width, (int)floorY, WHITE);
            }
            DrawRectangle(0, (int)(floorY + assets.tex_floor.height), scrW, scrH - (int)(floorY + assets.tex_floor.height), Color{18, 18, 22, 255});
        }

        RuText(assets.font, u8"\u042d\u0422\u0410\u0416 4 : \u041f\u041e\u0411\u0415\u0413",
               (int)(termX - 75), (int)(floorY - 215), 18, LIGHTGRAY);
        RuText(assets.font, u8"\u0412\u0435\u043d\u0442\u0438\u043b\u044f\u0446\u0438\u044f. \u041d\u0443\u0436\u0435\u043d \u043a\u0440\u0430\u0442\u0447\u0430\u0439\u0448\u0438\u0439 \u043f\u0443\u0442\u044c.",
               (int)(termX - 115), (int)(floorY - 195), 11, GRAY);

        float surfaceY = floorY - 50;

        // === Записка на столе ===
        DrawLabTable(notePosX + 14, surfaceY, 40, 7);
        DrawTextureEx(assets.tex_note, { notePosX, surfaceY - 15.0f }, 0.0f, 0.45f, WHITE);
        if (noteNear && !noteOpen) {
            RuText(assets.font, u8"[E] \u041f\u0440\u043e\u0447\u0438\u0442\u0430\u0442\u044c",
                   (int)(notePosX - 10), (int)(floorY + 6), 11, YELLOW);
        }

        // === Терминальный стол ===
        DrawLabTable(termX, surfaceY, 250, 10);

        // === Схема вентиляции на стене ===
        float cellSize = 12.0f;
        float mazeW = MAZE_W * cellSize;
        float mazeH = MAZE_H * cellSize;
        float mazeOffX = termX - mazeW / 2;
        float mazeOffY = surfaceY - 15 - mazeH;

        DrawRectangle((int)(mazeOffX - 6), (int)(mazeOffY - 6), (int)(mazeW + 12), (int)(mazeH + 12), Color{ 20, 25, 20, 255 });
        DrawRectangleLines((int)(mazeOffX - 6), (int)(mazeOffY - 6), (int)(mazeW + 12), (int)(mazeH + 12), Color{ 50, 70, 50, 255 });

        DrawRectangle((int)(termX - 2), (int)(mazeOffY + mazeH), 4, (int)(surfaceY - mazeOffY - mazeH + 10), Color{ 50, 55, 50, 255 });

        RuText(assets.font, u8"\u0421\u0425\u0415\u041c\u0410 \u0412\u0415\u041d\u0422\u0418\u041b\u042f\u0426\u0418\u0418",
               (int)(termX - 48), (int)(mazeOffY - 20), 10, Color{ 80, 120, 80, 180 });

        for (int y = 0; y < MAZE_H; y++) {
            for (int x = 0; x < MAZE_W; x++) {
                float cx = mazeOffX + x * cellSize;
                float cy = mazeOffY + y * cellSize;
                Color c = (maze[y][x] == 0) ? Color{ 25, 28, 35, 255 } : Color{ 50, 55, 65, 255 };
                DrawRectangle((int)cx, (int)cy, (int)cellSize, (int)cellSize, c);
            }
        }

        DrawRectangle((int)(mazeOffX + 1 * cellSize), (int)(mazeOffY + 1 * cellSize), (int)cellSize, (int)cellSize, GREEN);
        RuText(assets.font, "S", (int)(mazeOffX + 1 * cellSize + 2), (int)(mazeOffY + 1 * cellSize), 9, BLACK);
        DrawRectangle((int)(mazeOffX + 13 * cellSize), (int)(mazeOffY + 9 * cellSize), (int)cellSize, (int)cellSize, RED);
        RuText(assets.font, "F", (int)(mazeOffX + 13 * cellSize + 2), (int)(mazeOffY + 9 * cellSize), 9, WHITE);

        if (state == ANIMATING || state == SUCCESS || state == FAIL) {
            int (*path)[2] = (chosenAlgo==1) ? bfsPath : dfsPath;
            int pathLen = (chosenAlgo==1) ? bfsPathLen : dfsPathLen;
            int steps = (state==ANIMATING) ? animStep : pathLen;
            Color trailCol = (chosenAlgo==1) ? Fade(GREEN, 0.6f) : Fade(RED, 0.6f);
            for (int i = 0; i < steps && i < pathLen; i++) {
                float cx = mazeOffX + path[i][0] * cellSize;
                float cy = mazeOffY + path[i][1] * cellSize;
                DrawRectangle((int)cx + 1, (int)cy + 1, (int)cellSize - 2, (int)cellSize - 2, trailCol);
            }
            if (state == ANIMATING && animStep > 0 && animStep <= pathLen) {
                float dx = mazeOffX + path[animStep-1][0] * cellSize + cellSize / 2;
                float dy = mazeOffY + path[animStep-1][1] * cellSize + cellSize / 2;
                DrawCircle((int)dx, (int)dy, 3, YELLOW);
            }
        }

        // Кнопки DFS / BFS на столе
        if (state == WAITING) {
            float dfsBtnX = termX - 80;
            float bfsBtnX = termX + 80;
            float btnY = surfaceY - 22;
            float btnW = 85, btnH = 20;

            Color dfsCol = (hoveredBtn==0) ? Fade(RED, 0.6f) : Fade(RED, 0.3f);
            Color bfsCol = (hoveredBtn==1) ? Fade(GREEN, 0.6f) : Fade(GREEN, 0.3f);

            DrawRectangle((int)(dfsBtnX - btnW/2), (int)btnY, (int)btnW, (int)btnH, dfsCol);
            DrawRectangleLines((int)(dfsBtnX - btnW/2), (int)btnY, (int)btnW, (int)btnH, RAYWHITE);
            RuText(assets.font, "DFS", (int)(dfsBtnX - 10), (int)(btnY + 3), 12, RAYWHITE);

            DrawRectangle((int)(bfsBtnX - btnW/2), (int)btnY, (int)btnW, (int)btnH, bfsCol);
            DrawRectangleLines((int)(bfsBtnX - btnW/2), (int)btnY, (int)btnW, (int)btnH, RAYWHITE);
            RuText(assets.font, "BFS", (int)(bfsBtnX - 10), (int)(btnY + 3), 12, RAYWHITE);

            if (hoveredBtn >= 0) {
                float hx = (hoveredBtn==0) ? dfsBtnX : bfsBtnX;
                RuText(assets.font, u8"[E]", (int)(hx - 5), (int)(btnY - 14), 11, YELLOW);
            }
        }

        // Результаты
        if (state == FAIL) {
            RuText(assets.font, u8"\u0414\u043b\u0438\u043d\u043d\u044b\u0439 \u043f\u0443\u0442\u044c. \u0412\u043e\u0437\u0434\u0443\u0445 \u043a\u043e\u043d\u0447\u0438\u043b\u0441\u044f \u0431\u044b \u043d\u0430 \u043f\u043e\u043b\u043f\u0443\u0442\u0438.",
                   (int)(termX - 100), (int)(floorY + 6), 11, RED);
        }
        if (state == SUCCESS) {
            RuText(assets.font, u8"BFS нашёл кратчайший маршрут. Лифт открыт — иди к двери.",
                   (int)(termX - 145), (int)(floorY + 6), 11, GREEN);
        }

        // Лифт
        float doorScale = 0.2f;
        int doorH = (int)(assets.tex_elevator_closed.height * doorScale);
        if (state == SUCCESS) {
            DrawTextureEx(assets.tex_elevator_opened, {doorPos.x, doorPos.y - doorH}, 0.0f, doorScale, WHITE);
            if (player.is_in_area(doorPos.x)) {
                RuText(assets.font, u8"[E] В ЛИФТ", (int)doorPos.x - 18, (int)(doorPos.y - doorH - 16), 12, GREEN);
            }
        } else {
            DrawTextureEx(assets.tex_elevator_closed, {doorPos.x, doorPos.y - doorH}, 0.0f, doorScale, WHITE);
        }

        player.DrawSprite(assets);
    }

    void DrawUI(Assets& assets) {
        if (!noteOpen) return;

        int scrW = GetScreenWidth();
        int scrH = GetScreenHeight();
        float nx = scrW / 2.0f - 112.0f;
        float ny = scrH / 2.0f - 71.0f;
        DrawRectangle((int)nx, (int)ny, 224, 142, Fade(BLACK, 0.92f));
        DrawRectangleLines((int)nx, (int)ny, 224, 142, GOLD);
        RuText(assets.font, u8"--- ЗАПИСКА ---",
               (int)(nx + 8), (int)(ny + 8), 16, GOLD);
        RuText(assets.font, u8"Мне надо наверх. Прямо сейчас.",
               (int)(nx + 8), (int)(ny + 35), 12, RAYWHITE);
        RuText(assets.font, u8"Тут дышать почти нечем.",
               (int)(nx + 8), (int)(ny + 55), 12, RAYWHITE);
        RuText(assets.font, u8"DFS уходит в тупики. BFS сразу",
               (int)(nx + 8), (int)(ny + 75), 12, YELLOW);
        RuText(assets.font, u8"ищет кратчайший маршрут к люку.",
               (int)(nx + 8), (int)(ny + 95), 12, YELLOW);
        RuText(assets.font, u8"[E] Закрыть",
               (int)(nx + 72), (int)(ny + 118), 11, GRAY);
    }

};
