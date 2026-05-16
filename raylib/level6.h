#pragma once
#include "level.h"
#include "../utils.h"
#include <cmath>

// ЭТАЖ 4 «Побег» — вентиляционный лабиринт, BFS vs DFS
// DFS блуждает по тупикам, BFS находит кратчайший путь

struct Level6 {
    bool completed = false;
    bool badEnding = false;

    // --- Настройки этажа (легко менять) ---
    static const int MAZE_W = 15;
    static const int MAZE_H = 11;

    Vector2 doorPos;
    float notePosX;
    bool noteActive;

    // Лабиринт: 0=стена, 1=проход
    int maze[11][15];

    // Путь от старта до финиша
    int bfsPath[165][2]; // (x,y) пары
    int bfsPathLen;
    int dfsPath[165][2];
    int dfsPathLen;

    // Состояние
    enum State { WAITING, CHOOSING, ANIMATING, SUCCESS, FAIL, CODE_LOCK };
    State state;
    float stateTimer;

    int chosenAlgo; // 0=DFS, 1=BFS
    int animStep;   // текущий шаг анимации
    float animTimer;
    int codeValue;  // вводимый код
    int hoveredBtn; // 0=DFS, 1=BFS

    // Инициализация лабиринта (захардкожен — легко заменить на свой)
    void InitMaze() {
        // 0=стена, 1=проход
        // DFS тут попадёт в длинные тупики, BFS найдёт короткий путь
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

    // BFS — находит кратчайший путь
    void ComputeBFS() {
        // Старт: (1,1), Финиш: (13,9)
        int visited[11][15] = {};
        int parent[11][15][2] = {}; // откуда пришли
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
        // Восстанавливаем путь
        bfsPathLen = 0;
        int cx=13, cy=9;
        while (cx != -1) {
            bfsPath[bfsPathLen][0] = cx; bfsPath[bfsPathLen][1] = cy;
            bfsPathLen++;
            int px = parent[cy][cx][0], py = parent[cy][cx][1];
            cx = px; cy = py;
        }
        // Реверс
        for (int i=0; i<bfsPathLen/2; i++) {
            int tx=bfsPath[i][0], ty=bfsPath[i][1];
            bfsPath[i][0]=bfsPath[bfsPathLen-1-i][0]; bfsPath[i][1]=bfsPath[bfsPathLen-1-i][1];
            bfsPath[bfsPathLen-1-i][0]=tx; bfsPath[bfsPathLen-1-i][1]=ty;
        }
    }

    // DFS — находит длинный путь (идёт глубоко в тупики)
    void ComputeDFS() {
        int visited[11][15] = {};
        dfsPathLen = 0;
        // Простой рекурсивный DFS (стек через рекурсию не подходит для инлайн,
        // поэтому делаем итеративный стек)
        int stack[165][2] = {}; int sTop=0;
        int parent[11][15][2] = {};
        for (int y=0;y<MAZE_H;y++) for(int x=0;x<MAZE_W;x++) { parent[y][x][0]=-1; parent[y][x][1]=-1; }

        stack[sTop][0]=1; stack[sTop][1]=1; sTop++;
        visited[1][1] = 1;

        int dx[4] = {0,-1,0,1}; //优先 идём вправо и вниз чтобы зайти в тупики
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
        // Восстанавливаем
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
        noteActive = false;
        state = WAITING;
        stateTimer = 0.0f;
        chosenAlgo = -1;
        animStep = 0;
        animTimer = 0.0f;
        codeValue = 0;
        hoveredBtn = -1;
        doorPos = { screenW*0.88f, screenH*0.5f };
        notePosX = screenW*0.08f;
        InitMaze();
        ComputeBFS();
        ComputeDFS();
    }

    void Update(Player& player) {
        noteActive = player.is_in_area(notePosX);
        float dt = GetFrameTime();

        if (state == WAITING) {
            hoveredBtn = -1;
            // Две кнопки на экране терминала
            float scrW = GetScreenWidth();
            if (player.is_in_area(scrW*0.35f)) { hoveredBtn = 0; } // DFS
            if (player.is_in_area(scrW*0.55f)) { hoveredBtn = 1; } // BFS
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
                if (chosenAlgo == 1) { // BFS — успех
                    state = SUCCESS;
                    stateTimer = 0.0f;
                } else { // DFS — провал
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
            stateTimer += dt;
            if (stateTimer > 2.0f) {
                state = CODE_LOCK;
                codeValue = 0;
            }
        }
        else if (state == CODE_LOCK) {
            // A/D меняют цифру, E подтверждает
            if (IsKeyPressed(KEY_A) && codeValue > 0) codeValue--;
            if (IsKeyPressed(KEY_D) && codeValue < 99) codeValue++;
            if (IsKeyPressed(KEY_E)) {
                // Правильный код = длина BFS пути
                if (codeValue == bfsPathLen) {
                    completed = true;
                }
            }
        }
    }

    void Draw(Player& player, Assets& assets) {
        int scrW = GetScreenWidth();
        int scrH = GetScreenHeight();
        ClearBackground(Color{ 15, 18, 22, 255 });

        RuText(assets.font, u8"ЭТАЖ 4 : ПОБЕГ", scrW/2-140, 30, 26, LIGHTGRAY);
        RuText(assets.font, u8"Система вентиляции. Нужен кратчайший путь.",
               scrW/2-250, 65, 16, GRAY);

        // Рисуем лабиринт
        float cellSize = 28.0f;
        float mazeOffX = scrW/2.0f - (MAZE_W*cellSize)/2.0f;
        float mazeOffY = scrH*0.18f;

        for (int y=0; y<MAZE_H; y++) {
            for (int x=0; x<MAZE_W; x++) {
                float cx = mazeOffX + x*cellSize;
                float cy = mazeOffY + y*cellSize;
                if (maze[y][x] == 0) {
                    DrawRectangle((int)cx,(int)cy,(int)cellSize,(int)cellSize, Color{25,28,35,255});
                } else {
                    DrawRectangle((int)cx,(int)cy,(int)cellSize,(int)cellSize, Color{50,55,65,255});
                }
            }
        }

        // Старт и финиш
        DrawRectangle((int)(mazeOffX+1*cellSize),(int)(mazeOffY+1*cellSize),(int)cellSize,(int)cellSize, GREEN);
        RuText(assets.font, "S", (int)(mazeOffX+1*cellSize+8), (int)(mazeOffY+1*cellSize+4), 16, BLACK);
        DrawRectangle((int)(mazeOffX+13*cellSize),(int)(mazeOffY+9*cellSize),(int)cellSize,(int)cellSize, RED);
        RuText(assets.font, "F", (int)(mazeOffX+13*cellSize+8), (int)(mazeOffY+9*cellSize+4), 16, WHITE);

        // Рисуем путь (анимация)
        if (state == ANIMATING || state == SUCCESS || state == FAIL || state == CODE_LOCK) {
            int (*path)[2] = (chosenAlgo==1) ? bfsPath : dfsPath;
            int steps = (state==ANIMATING) ? animStep : ((chosenAlgo==1)?bfsPathLen:dfsPathLen);
            Color trailCol = (chosenAlgo==1) ? Fade(GREEN,0.6f) : Fade(RED,0.6f);
            for (int i=0; i<steps && i<((chosenAlgo==1)?bfsPathLen:dfsPathLen); i++) {
                float cx = mazeOffX + path[i][0]*cellSize;
                float cy = mazeOffY + path[i][1]*cellSize;
                DrawRectangle((int)cx+2,(int)cy+2,(int)cellSize-4,(int)cellSize-4, trailCol);
            }
            // Дрон (точка на текущей позиции)
            if (state == ANIMATING && animStep > 0 && animStep <= ((chosenAlgo==1)?bfsPathLen:dfsPathLen)) {
                float dx = mazeOffX + path[animStep-1][0]*cellSize + cellSize/2;
                float dy = mazeOffY + path[animStep-1][1]*cellSize + cellSize/2;
                DrawCircle((int)dx,(int)dy, 6, YELLOW);
            }
        }

        // Кнопки DFS / BFS
        if (state == WAITING) {
            float bw = 200, bh = 60;
            Rectangle btnDFS = { scrW*0.35f - bw/2, scrH*0.72f, bw, bh };
            Rectangle btnBFS = { scrW*0.55f - bw/2, scrH*0.72f, bw, bh };
            Color dfsCol = (hoveredBtn==0) ? Fade(RED,0.6f) : Fade(RED,0.3f);
            Color bfsCol = (hoveredBtn==1) ? Fade(GREEN,0.6f) : Fade(GREEN,0.3f);
            DrawRectangleRec(btnDFS, dfsCol); DrawRectangleLinesEx(btnDFS,2,RAYWHITE);
            DrawRectangleRec(btnBFS, bfsCol); DrawRectangleLinesEx(btnBFS,2,RAYWHITE);
            RuText(assets.font, "DFS", (int)(btnDFS.x+70),(int)(btnDFS.y+18), 24, RAYWHITE);
            RuText(assets.font, "BFS", (int)(btnBFS.x+70),(int)(btnBFS.y+18), 24, RAYWHITE);
            if (hoveredBtn >= 0) RuText(assets.font, u8"[E]", scrW/2-10, (int)(scrH*0.72f+bh+5), 16, YELLOW);
        }

        // Результаты
        if (state == FAIL) {
            RuText(assets.font, u8"Путь слишком длинный. Кислорода не хватит.",
                   scrW/2-270, scrH*0.85f, 20, RED);
        }
        if (state == SUCCESS || state == CODE_LOCK) {
            RuText(assets.font, TextFormat("Кратчайший путь: %d клеток", bfsPathLen),
                   scrW/2-160, scrH*0.85f, 20, GREEN);
        }
        if (state == CODE_LOCK) {
            RuText(assets.font, u8"Введите длину пути:", scrW/2-140, scrH*0.78f, 18, RAYWHITE);
            RuText(assets.font, TextFormat("[ %d ]", codeValue), scrW/2-30, scrH*0.82f, 36, YELLOW);
            RuText(assets.font, u8"A/D — менять | E — подтвердить", scrW/2-180, scrH*0.90f, 14, GRAY);
            if (IsKeyPressed(KEY_E) && codeValue != bfsPathLen)
                RuText(assets.font, u8"Неверный код!", scrW/2-70, scrH*0.93f, 18, RED);
        }

        // Записка
        DrawRectangle((int)notePosX,(int)(scrH*0.70f), 36, 28, BROWN);
        DrawRectangleLines((int)notePosX,(int)(scrH*0.70f), 36, 28, DARKBROWN);
        RuText(assets.font, u8"записка", (int)notePosX-4, (int)(scrH*0.67f), 13, BEIGE);
        if (noteActive) {
            int nx=scrW/2-400, ny=scrH/2-150;
            DrawRectangle(nx,ny,800,300, Fade(BLACK,0.92f));
            DrawRectangleLines(nx,ny,800,300, GOLD);
            RuText(assets.font, u8"--- ЗАПИСКА ---", nx+30, ny+25, 26, GOLD);
            RuText(assets.font, u8"Мне надо наверх. Срочно.", nx+30, ny+75, 22, RAYWHITE);
            RuText(assets.font, u8"Воздуха не хватает.", nx+30, ny+110, 22, RAYWHITE);
            RuText(assets.font, u8"Как пройти систему вентиляции?", nx+30, ny+150, 22, YELLOW);
        }

        player.Draw();
    }
};
