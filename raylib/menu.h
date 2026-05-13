#pragma once
#include <raylib.h>

struct Button {
    Rectangle rect;
    const char* text;
    Color bg = WHITE;
    bool active = true;

    bool draw() {
        if (!active) return false;

        bool hovered = CheckCollisionPointRec(GetMousePosition(), rect);
        DrawRectangleRec(rect, bg);
        DrawRectangleLinesEx(rect, 2, hovered ? BLUE : ORANGE);

        int textW = MeasureText(text, 24);
        int textX = (int)(rect.x + (rect.width - (float)textW) / 2.0f);
        int textY = (int)(rect.y + (rect.height - 24.0f) / 2.0f);
        DrawText(text, textX, textY, 24, BLACK);

        return hovered && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
    }
};

enum GameState { MENU, PLAYING, EXIT };

struct Menu {
    Button btnPlay;
    Button btnQuit;
    Button btnSecret;
    
    bool showSecret = false;
    bool showQuitConfirm = false;
    GameState state = MENU; // MENU, PLAYING, EXIT

    void Init() {
        float x = 800.0f;
        float w = 300.0f;
        float h = 50.0f;
        
        btnPlay = { {x, 200.0f, w, h}, "PLAY", WHITE, true };
        btnQuit = { {x, 270.0f, w, h}, "EXIT", WHITE, true };
        btnSecret = { {x, 520.0f, w, h}, "DON'T CLICK", WHITE, true };
        showSecret = false;
        showQuitConfirm = false;
        state = MENU;
    }

    void Update() {
        if (btnSecret.draw()) {
            TraceLog(LOG_INFO, "Secret clicked!");
            btnSecret.active = false;
            showSecret = true;
        }

        if (btnPlay.draw()) {
            TraceLog(LOG_INFO, "Play clicked!");
            state = PLAYING;
        }

        if (btnQuit.draw()) {
            showQuitConfirm = true;
        }

        if (showQuitConfirm && btnQuit.draw()) {
            state = EXIT;
        }
    }

    void Draw() {
        ClearBackground(WHITE);
        DrawText("WELCOME TO THE GAME", 600, 24, 50, BLACK);
        DrawText("LIFT", 850, 80, 50, BLACK);

        btnPlay.draw();
        btnQuit.draw();
        btnSecret.draw();

        if (showSecret) {
            DrawText("ooo.mogaqu", 800, 520, 40, BLACK);
        }
    }
};
