#pragma once
#include "level.h"
#include "../utils.h"
#include <cmath>

struct Level4 {
    bool completed = false;
    bool badEnding = false;

    Vector2 doorPos;
    float notePosX;
    bool noteActive;

    // Z-камера в центре комнаты
    Vector2 cameraPos;

    // Коробка Шрёдингера
    Vector2 boxPos;
    bool catAlive;

    // Электронная пушка и экран
    Vector2 gunPos;
    Vector2 screenPos;

    // Кнопки пушки
    Vector2 btnWavePos;
    Vector2 btnParticlePos;

    // Состояния
    enum State {
        WAITING,            // ждём, пока игрок подойдёт к коробке
        BOX_OPEN,           // коробка открыта, показываем результат 2 сек
        EXPERIMENTS_UNLOCKED, // эксперименты доступны
        SHOW_WAVES,         // показываем волновой паттерн 2 сек
        SHOW_PARTICLES,     // показываем частицы 2 сек
        QUIZ,               // викторина
        SOLVED              // загадка решена
    };
    State state;
    float stateTimer;

    bool waveSeen;
    bool particleSeen;

    // Варианты ответа в викторине: 0=Частица, 1=Волна, 2=И то и другое
    int quizAnswer;
    int quizHovered;

    void Init(int screenW, int screenH) {
        completed = false;
        badEnding = false;
        noteActive = false;

        state = WAITING;
        stateTimer = 0.0f;
        waveSeen = false;
        particleSeen = false;
        catAlive = false;
        quizAnswer = -1;
        quizHovered = -1;

        doorPos = { screenW * 0.88f, screenH * 0.5f };
        notePosX = screenW * 0.08f;

        // Z-камера в центре
        cameraPos = { screenW * 0.5f, screenH * 0.35f };

        // Коробка Шрёдингера слева от центра
        boxPos = { screenW * 0.25f, screenH * 0.55f };

        // Электронная пушка справа от центра
        gunPos = { screenW * 0.65f, screenH * 0.55f };

        // Экран-монитор выше пушки
        screenPos = { screenW * 0.65f, screenH * 0.2f };

        // Кнопки рядом с пушкой
        btnWavePos = { screenW * 0.55f, screenH * 0.68f };
        btnParticlePos = { screenW * 0.73f, screenH * 0.68f };
    }

    void Update(Player& player) {
        float dt = GetFrameTime();
        noteActive = player.is_in_area(notePosX);

        if (state == WAITING) {
            // Подошёл к коробке и нажал E
            if (player.is_in_area(boxPos.x) && IsKeyPressed(KEY_E)) {
                catAlive = (GetRandomValue(0, 1) == 1);
                state = BOX_OPEN;
                stateTimer = 2.0f;
            }
            return;
        }

        if (state == BOX_OPEN) {
            stateTimer -= dt;
            if (stateTimer <= 0.0f) {
                state = EXPERIMENTS_UNLOCKED;
            }
            return;
        }

        if (state == EXPERIMENTS_UNLOCKED) {
            // Кнопка «БЕЗ ДЕТЕКТОРА» — волны
            if (player.is_in_area(btnWavePos.x) && IsKeyPressed(KEY_E)) {
                state = SHOW_WAVES;
                stateTimer = 2.0f;
            }
            // Кнопка «С ДЕТЕКТОРОМ» — частицы
            if (player.is_in_area(btnParticlePos.x) && IsKeyPressed(KEY_E)) {
                state = SHOW_PARTICLES;
                stateTimer = 2.0f;
            }
            return;
        }

        if (state == SHOW_WAVES) {
            stateTimer -= dt;
            if (stateTimer <= 0.0f) {
                waveSeen = true;
                if (waveSeen && particleSeen) {
                    state = QUIZ;
                } else {
                    state = EXPERIMENTS_UNLOCKED;
                }
            }
            return;
        }

        if (state == SHOW_PARTICLES) {
            stateTimer -= dt;
            if (stateTimer <= 0.0f) {
                particleSeen = true;
                if (waveSeen && particleSeen) {
                    state = QUIZ;
                } else {
                    state = EXPERIMENTS_UNLOCKED;
                }
            }
            return;
        }

        if (state == QUIZ) {
            quizHovered = -1;
            // Три варианта ответа, проверяем наведение
            float quizX = GetScreenWidth() * 0.28f;
            float quizY = GetScreenHeight() * 0.55f;
            for (int i = 0; i < 3; i++) {
                float bx = quizX + i * 200.0f;
                if (player.is_in_area(bx)) {
                    quizHovered = i;
                    if (IsKeyPressed(KEY_E)) {
                        quizAnswer = i;
                        if (i == 2) {
                            // Правильный ответ: «И то и другое»
                            state = SOLVED;
                        }
                        // Неправильный — остаёмся в QUIZ
                    }
                }
            }
            return;
        }

        if (state == SOLVED) {
            if (player.is_in_area(doorPos.x) && IsKeyPressed(KEY_E)) {
                completed = true;
            }
        }
    }

    // Рисуем волновую интерференцию на экране
    void DrawWavePattern(Assets& assets, float cx, float cy, float w, float h) {
        // Фон экрана
        DrawRectangle((int)(cx - w / 2), (int)(cy - h / 2), (int)w, (int)h, Color{ 5, 10, 5, 255 });

        // Интерференционная картина — синусоиды разной амплитуды
        float baseY = cy;
        for (int i = -6; i <= 6; i++) {
            float x = cx + i * (w / 14.0f);
            // Интенсивность: максимум в центре, убывает к краям
            float intensity = cosf(i * 0.35f);
            if (intensity < 0.0f) intensity = 0.0f;
            float barH = h * 0.8f * intensity;
            Color c = Color{
                (unsigned char)(30 * intensity),
                (unsigned char)(255 * intensity),
                (unsigned char)(100 * intensity),
                255
            };
            DrawLineEx({ x, baseY - barH / 2 }, { x, baseY + barH / 2 }, 3.0f, c);
        }

        // Подпись
        RuText(assets.font, u8"\u0418\u043d\u0442\u0435\u0440\u0444\u0435\u0440\u0435\u043d\u0446\u0438\u044f",
               (int)(cx - 60), (int)(cy + h / 2 + 5), 14, GREEN);
    }

    // Рисуем две полосы — частицы
    void DrawParticlePattern(Assets& assets, float cx, float cy, float w, float h) {
        DrawRectangle((int)(cx - w / 2), (int)(cy - h / 2), (int)w, (int)h, Color{ 5, 5, 10, 255 });

        // Две вертикальные полосы
        float gap = w * 0.25f;
        for (int s = 0; s < 2; s++) {
            float sx = cx - gap / 2 + s * gap;
            Color c = Color{ 60, 120, 255, 255 };
            DrawLineEx({ sx, cy - h * 0.35f }, { sx, cy + h * 0.35f }, 6.0f, c);
            // Точки-частицы вокруг полосы
            for (int d = 0; d < 5; d++) {
                float dx = RandF(-8, 8);
                float dy = RandF(-h * 0.3f, h * 0.3f);
                DrawCircleV({ sx + dx, cy + dy }, 2, Fade(c, 0.4f));
            }
        }

        RuText(assets.font, u8"\u0414\u0432\u0435 \u0449\u0435\u043b\u0438",
               (int)(cx - 40), (int)(cy + h / 2 + 5), 14, SKYBLUE);
    }

    void Draw(Player& player, Assets& assets) {
        int scrW = GetScreenWidth();
        int scrH = GetScreenHeight();

        // Фон — тёмный с зелёным оттенком
        ClearBackground(Color{ 10, 15, 10, 255 });

        // Заголовок
        RuText(assets.font, u8"\u042d\u0422\u0410\u0416 3 : \u041b\u0410\u0411\u041e\u0420\u0410\u0422\u041e\u0420\u0418\u042f",
               scrW / 2 - 220, 15, 26, LIGHTGRAY);
        RuText(assets.font, u8"Z-\u0438\u0437\u043b\u0443\u0447\u0435\u043d\u0438\u0435 \u00b7 \u043a\u0432\u0430\u043d\u0442\u043e\u0432\u044b\u0435 \u044d\u043a\u0441\u043f\u0435\u0440\u0438\u043c\u0435\u043d\u0442\u044b",
               scrW / 2 - 280, 48, 16, GRAY);

        // Z-камера в центре — светящаяся машина
        DrawGlow(cameraPos, 50.0f, { 100, 255, 100, 140 }, { 50, 200, 50, 0 }, 8);
        DrawRectangle((int)(cameraPos.x - 40), (int)(cameraPos.y - 30), 80, 60, Color{ 30, 50, 30, 255 });
        DrawRectangleLines((int)(cameraPos.x - 40), (int)(cameraPos.y - 30), 80, 60, GREEN);
        RuText(assets.font, "Z", (int)(cameraPos.x - 6), (int)(cameraPos.y - 12), 22, GREEN);
        RuText(assets.font, u8"Z-\u043a\u0430\u043c\u0435\u0440\u0430", (int)(cameraPos.x - 45), (int)(cameraPos.y - 50), 13, Color{ 100, 255, 100, 200 });

        // Столы с записками (просто прямоугольники)
        DrawRectangle((int)(scrW * 0.12f), (int)(scrH * 0.42f), 50, 30, Color{ 40, 35, 25, 255 });
        DrawRectangleLines((int)(scrW * 0.12f), (int)(scrH * 0.42f), 50, 30, Color{ 80, 70, 50, 255 });
        DrawRectangle((int)(scrW * 0.38f), (int)(scrH * 0.78f), 50, 30, Color{ 40, 35, 25, 255 });
        DrawRectangleLines((int)(scrW * 0.38f), (int)(scrH * 0.78f), 50, 30, Color{ 80, 70, 50, 255 });

        // Коробка Шрёдингера
        DrawRectangle((int)(boxPos.x - 35), (int)(boxPos.y - 30), 70, 60, Color{ 50, 40, 60, 255 });
        DrawRectangleLines((int)(boxPos.x - 35), (int)(boxPos.y - 30), 70, 60, PURPLE);
        if (state == WAITING || state == BOX_OPEN) {
            RuText(assets.font, u8"\u041a\u041e\u0420\u041e\u0411\u041a\u0410 \u0428\u0420\u0401\u0414\u0418\u041d\u0413\u0415\u0420\u0410",
                   (int)(boxPos.x - 80), (int)(boxPos.y - 55), 14, PURPLE);
        }
        if (state == WAITING && player.is_in_area(boxPos.x)) {
            RuText(assets.font, u8"[E] \u041e\u0442\u043a\u0440\u044b\u0442\u044c",
                   (int)(boxPos.x - 35), (int)(boxPos.y + 35), 14, YELLOW);
        }

        // Результат коробки
        if (state == BOX_OPEN) {
            Color catCol = catAlive ? GREEN : RED;
            const char* catText = catAlive
                ? u8"\u0421\u0443\u043f\u0435\u0440\u043f\u043e\u0437\u0438\u0446\u0438\u044f \u0440\u0443\u0445\u043d\u0443\u043b\u0430. \u041a\u043e\u0442: \u0416\u0418\u0412"
                : u8"\u0421\u0443\u043f\u0435\u0440\u043f\u043e\u0437\u0438\u0446\u0438\u044f \u0440\u0443\u0445\u043d\u0443\u043b\u0430. \u041a\u043e\u0442: \u041c\u0401\u0420\u0422\u0412";
            int tw = RuMeasure(assets.font, catText, 18);
            DrawRectangle(scrW / 2 - tw / 2 - 20, scrH / 2 - 30, tw + 40, 50, Fade(BLACK, 0.85f));
            RuText(assets.font, catText, scrW / 2 - tw / 2, scrH / 2 - 20, 18, catCol);
        }

        // Электронная пушка
        DrawRectangle((int)(gunPos.x - 30), (int)(gunPos.y - 20), 60, 40, Color{ 30, 30, 50, 255 });
        DrawRectangleLines((int)(gunPos.x - 30), (int)(gunPos.y - 20), 60, 40, SKYBLUE);
        RuText(assets.font, u8"e\u207b \u043f\u0443\u0448\u043a\u0430",
               (int)(gunPos.x - 30), (int)(gunPos.y - 38), 13, SKYBLUE);

        // Экран-монитор
        DrawRectangle((int)(screenPos.x - 80), (int)(screenPos.y - 50), 160, 100, Color{ 15, 20, 15, 255 });
        DrawRectangleLines((int)(screenPos.x - 80), (int)(screenPos.y - 50), 160, 100, Color{ 60, 100, 60, 255 });
        RuText(assets.font, u8"\u042d\u043a\u0440\u0430\u043d",
               (int)(screenPos.x - 25), (int)(screenPos.y - 65), 13, Color{ 60, 100, 60, 200 });

        // Рисуем паттерн на экране
        if (state == SHOW_WAVES) {
            DrawWavePattern(assets, screenPos.x, screenPos.y, 140.0f, 80.0f);
        } else if (state == SHOW_PARTICLES) {
            DrawParticlePattern(assets, screenPos.x, screenPos.y, 140.0f, 80.0f);
        } else if (state == EXPERIMENTS_UNLOCKED || state == QUIZ) {
            // Пустой экран
            RuText(assets.font, "...", (int)(screenPos.x - 10), (int)(screenPos.y - 8), 16, DARKGREEN);
        }

        // Кнопки пушки (только если эксперименты разблокированы)
        if (state == EXPERIMENTS_UNLOCKED) {
            // Кнопка «БЕЗ ДЕТЕКТОРА»
            Color btn1Col = player.is_in_area(btnWavePos.x)
                ? Color{ 40, 80, 40, 255 } : Color{ 25, 40, 25, 255 };
            DrawRectangle((int)(btnWavePos.x - 70), (int)(btnWavePos.y - 15), 140, 30, btn1Col);
            DrawRectangleLines((int)(btnWavePos.x - 70), (int)(btnWavePos.y - 15), 140, 30, GREEN);
            RuText(assets.font, u8"[\u0411\u0415\u0417 \u0414\u0415\u0422\u0415\u041a\u0422\u041e\u0420\u0410]",
                   (int)(btnWavePos.x - 65), (int)(btnWavePos.y - 10), 13, GREEN);
            if (player.is_in_area(btnWavePos.x)) {
                RuText(assets.font, u8"[E]", (int)(btnWavePos.x - 10), (int)(btnWavePos.y - 35), 14, YELLOW);
            }

            // Кнопка «С ДЕТЕКТОРОМ»
            Color btn2Col = player.is_in_area(btnParticlePos.x)
                ? Color{ 40, 40, 80, 255 } : Color{ 25, 25, 40, 255 };
            DrawRectangle((int)(btnParticlePos.x - 65), (int)(btnParticlePos.y - 15), 130, 30, btn2Col);
            DrawRectangleLines((int)(btnParticlePos.x - 65), (int)(btnParticlePos.y - 15), 130, 30, SKYBLUE);
            RuText(assets.font, u8"[\u0421 \u0414\u0415\u0422\u0415\u041a\u0422\u041e\u0420\u041e\u041c]",
                   (int)(btnParticlePos.x - 60), (int)(btnParticlePos.y - 10), 13, SKYBLUE);
            if (player.is_in_area(btnParticlePos.x)) {
                RuText(assets.font, u8"[E]", (int)(btnParticlePos.x - 10), (int)(btnParticlePos.y - 35), 14, YELLOW);
            }

            // Подсказка что видели
            if (waveSeen) {
                RuText(assets.font, u8"\u2713 \u0412\u043e\u043b\u043d\u044b",
                       (int)(btnWavePos.x - 40), (int)(btnWavePos.y + 22), 13, GREEN);
            }
            if (particleSeen) {
                RuText(assets.font, u8"\u2713 \u0427\u0430\u0441\u0442\u0438\u0446\u044b",
                       (int)(btnParticlePos.x - 40), (int)(btnParticlePos.y + 22), 13, SKYBLUE);
            }
        }

        // Викторина
        if (state == QUIZ) {
            float qx = scrW * 0.28f;
            float qy = scrH * 0.55f;

            // Фон вопроса
            int tw = RuMeasure(assets.font,
                u8"\u0427\u0442\u043e \u0432\u044b \u043d\u0430\u0431\u043b\u044e\u0434\u0430\u043b\u0438? \u042d\u043b\u0435\u043a\u0442\u0440\u043e\u043d \u2014 \u044d\u0442\u043e...", 18);
            DrawRectangle(scrW / 2 - tw / 2 - 20, (int)(qy - 70), tw + 40, 40, Fade(BLACK, 0.9f));
            RuText(assets.font,
                   u8"\u0427\u0442\u043e \u0432\u044b \u043d\u0430\u0431\u043b\u044e\u0434\u0430\u043b\u0438? \u042d\u043b\u0435\u043a\u0442\u0440\u043e\u043d \u2014 \u044d\u0442\u043e...",
                   scrW / 2 - tw / 2, (int)(qy - 60), 18, RAYWHITE);

            // Три варианта
            const char* options[3] = {
                u8"\u0427\u0430\u0441\u0442\u0438\u0446\u0430",
                u8"\u0412\u043e\u043b\u043d\u0430",
                u8"\u0418 \u0442\u043e \u0438 \u0434\u0440\u0443\u0433\u043e\u0435"
            };
            Color optColors[3] = { RED, GREEN, GOLD };

            for (int i = 0; i < 3; i++) {
                float bx = qx + i * 200.0f;
                bool hovered = (quizHovered == i);
                Color bg = hovered ? Fade(optColors[i], 0.3f) : Fade(BLACK, 0.7f);
                DrawRectangle((int)bx, (int)qy, 180, 40, bg);
                DrawRectangleLines((int)bx, (int)qy, 180, 40, optColors[i]);
                RuText(assets.font, options[i], (int)(bx + 10), (int)(qy + 10), 16, optColors[i]);
                if (hovered) {
                    RuText(assets.font, u8"[E]", (int)(bx + 75), (int)(qy - 20), 14, YELLOW);
                }
            }

            // Неправильный ответ — сообщение
            if (quizAnswer != -1 && quizAnswer != 2) {
                RuText(assets.font, u8"\u041d\u0435\u043f\u0440\u0430\u0432\u0438\u043b\u044c\u043d\u043e! \u041f\u043e\u0434\u0443\u043c\u0430\u0439\u0442\u0435 \u0435\u0449\u0451...",
                       scrW / 2 - 180, (int)(qy + 60), 16, RED);
                quizAnswer = -1;
            }
        }

        // Записка на столе (левая)
        DrawRectangle((int)notePosX, (int)(scrH * 0.70f), 36, 28, BROWN);
        DrawRectangleLines((int)notePosX, (int)(scrH * 0.70f), 36, 28, DARKBROWN);
        RuText(assets.font, u8"\u0437\u0430\u043f\u0438\u0441\u043a\u0430",
               (int)notePosX - 4, (int)(scrH * 0.67f), 13, BEIGE);

        if (noteActive) {
            int nx = scrW / 2 - 400;
            int ny = scrH / 2 - 250;
            DrawRectangle(nx, ny, 800, 500, Fade(BLACK, 0.92f));
            DrawRectangleLines(nx, ny, 800, 500, GOLD);

            RuText(assets.font, u8"--- \u0417\u0410\u041f\u0418\u0421\u041a\u0410 #7 ---", nx + 30, ny + 25, 26, GOLD);

            // Текст записки — иногда «глючит»
            float glitch = sinf(GetTime() * 7.0f);
            Color textColor = (glitch > 0.9f)
                ? Color{ 0, 255, 0, 255 } : RAYWHITE;

            RuText(assets.font,
                   u8"\u0421\u0443\u0431\u044a\u0435\u043a\u0442 \u0414.\u041a. \u043d\u0430\u0434\u0435\u043b \u0445\u0430\u043b\u0430\u0442 \u043f\u0435\u0440\u0435\u0434 \u043e\u0431\u043b\u0443\u0447\u0435\u043d\u0438\u0435\u043c.",
                   nx + 30, ny + 75, 20, textColor);
            RuText(assets.font,
                   u8"\u041a\u043e\u043d\u0444\u0430\u0431\u0443\u043b\u044f\u0446\u0438\u044f 100%.",
                   nx + 30, ny + 105, 20, textColor);
            RuText(assets.font,
                   u8"\u041e\u043d \u0432\u0435\u0440\u0438\u0442, \u0447\u0442\u043e \u043e\u043d \u0441\u043e\u0442\u0440\u0443\u0434\u043d\u0438\u043a.",
                   nx + 30, ny + 135, 20, textColor);

            // Глючная строка
            if (glitch > 0.8f) {
                RuText(assets.font,
                       u8"\u041e\u043d \u043d\u0415 \u0441\u043e\u0442\u0440\u0443\u0434\u043d\u0438\u043a. \u041e\u043d \u043d\u0415 \u0441\u043e\u0442\u0440\u0443\u0434\u043d\u0438\u043a.",
                       nx + 30, ny + 185, 20, Color{ 255, 0, 80, 200 });
            }
        }

        // Дверь — когда загадка решена
        if (state == SOLVED) {
            DrawTexture(assets.door_opened, (int)doorPos.x, (int)doorPos.y, WHITE);
            RuText(assets.font, u8"[E] \u0412\u041e\u0419\u0422\u0418 \u0412 \u041b\u0418\u0424\u0422",
                   (int)doorPos.x - 30, (int)doorPos.y - 40, 15, GREEN);
        }

        player.Draw();
    }
};
