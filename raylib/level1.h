#pragma once
#include "level.h"
#include "../utils.h"
#include <cmath>

// ============================================================
//  ЭТАЖ 1 — «Приветствие»
//  Терминал советской системы А.У.Р.А.
//  Товарищ Коротков проспал 43 821 час и должен пройти тест
//  по электробезопасности (закон Ома, параллельные резисторы)
// ============================================================

struct Level1 {
    bool completed = false;
    bool badEnding = false;

    // Позиции объектов
    Vector2 doorPos;
    Vector2 terminalPos;
    float notePosX;
    float terminalCenterX;   // X-центр терминала — куда «примораживается» игрок

    // Состояния экрана терминала
    enum State { WAITING, INTRO, QUIZ, FAIL, SOLVED };
    State state;
    float stateTimer;

    // Интро — построчное появление текста АУРА
    int introLineShown;      // сколько строк уже видно (0..4)

    // Квиз — вопросы по электробезопасности
    int currentQuestion;     // 0, 1, 2
    int selectedOption;      // 0..3 (курсор на варианте ответа)

    // Данные вопросов (заполняются в Init)
    const char* questionText[3];
    const char* optionText[3][4];
    int correctOption[3];

    // Сарказм при ошибке (3 варианта, выбирается случайно)
    const char* failMessages[3];
    int failMessageIndex;

    void Init(int screenW, int screenH) {
        completed = false;
        badEnding = false;
        state = WAITING;
        stateTimer = 0.0f;
        introLineShown = 0;
        currentQuestion = 0;
        selectedOption = 0;
        failMessageIndex = 0;

        doorPos = { screenW * 0.88f, screenH * 0.5f };
        terminalPos = { screenW * 0.08f, screenH * 0.10f };
        terminalCenterX = screenW * 0.5f;
        notePosX = screenW * 0.08f;

        // --- Вопрос 1: параллельное соединение 6 Ом и 3 Ом ---
        // 1/R = 1/6 + 1/3 = 1/6 + 2/6 = 3/6 = 1/2  →  R = 2 Ом
        questionText[0] = u8"Два резистора 6 Ом и 3 Ом соединены ПАРАЛЛЕЛЬНО. Общее сопротивление?";
        optionText[0][0] = u8"9 Ом";
        optionText[0][1] = u8"2 Ом";
        optionText[0][2] = u8"3 Ом";
        optionText[0][3] = u8"4.5 Ом";
        correctOption[0] = 1;

        // --- Вопрос 2: закон Ома ---
        questionText[1] = u8"Резистор 10 Ом подключён к источнику 20 В. Ток через резистор?";
        optionText[1][0] = u8"0.5 А";
        optionText[1][1] = u8"2 А";
        optionText[1][2] = u8"200 А";
        optionText[1][3] = u8"10 А";
        correctOption[1] = 0;

        // --- Вопрос 3: три резистора по 12 Ом параллельно ---
        // 1/R = 3/12 = 1/4  →  R = 4 Ом
        questionText[2] = u8"Три резистора по 12 Ом соединены ПАРАЛЛЕЛЬНО. Общее сопротивление?";
        optionText[2][0] = u8"36 Ом";
        optionText[2][1] = u8"6 Ом";
        optionText[2][2] = u8"4 Ом";
        optionText[2][3] = u8"24 Ом";
        correctOption[2] = 2;

        // --- Саркастические сообщения при ошибке ---
        failMessages[0] = u8"СИСТЕМА: Ошибка. Товарищ Коротков, ваше незнание закона Ома позорит весь институт.";
        failMessages[1] = u8"СИСТЕМА: Ошибка. Даже стажёр справился бы лучше.";
        failMessages[2] = u8"СИСТЕМА: Ошибка. Вы уверены, что вы физик?";
    }

    void Update(Player& player) {
        float dt = GetFrameTime();

        // --- WAITING: ждём, пока игрок подойдёт к терминалу ---
        if (state == WAITING) {
            if (player.is_in_area(terminalCenterX) && IsKeyPressed(KEY_E)) {
                state = INTRO;
                stateTimer = 1.5f;
                introLineShown = 0;
            }
            return;
        }

        // Во время работы с терминалом «примораживаем» игрока
        if (state == INTRO || state == QUIZ || state == FAIL) {
            player.pos.x = terminalCenterX;
            player.direction = 0;
        }

        // --- INTRO: построчное появление текста АУРА ---
        if (state == INTRO) {
            stateTimer -= dt;
            if (stateTimer <= 0.0f) {
                introLineShown++;
                if (introLineShown >= 4) {
                    state = QUIZ;
                    currentQuestion = 0;
                    selectedOption = 0;
                } else {
                    stateTimer = 1.5f;
                }
            }
            return;
        }

        // --- QUIZ: выбор ответа ---
        if (state == QUIZ) {
            // Перемещение курсора между вариантами
            if (IsKeyPressed(KEY_A) || IsKeyPressed(KEY_LEFT)) {
                selectedOption--;
                if (selectedOption < 0) selectedOption = 3;
            }
            if (IsKeyPressed(KEY_D) || IsKeyPressed(KEY_RIGHT)) {
                selectedOption++;
                if (selectedOption > 3) selectedOption = 0;
            }

            // Подтверждение ответа
            if (IsKeyPressed(KEY_E)) {
                if (selectedOption == correctOption[currentQuestion]) {
                    // Правильно — следующий вопрос
                    currentQuestion++;
                    selectedOption = 0;
                    if (currentQuestion >= 3) {
                        state = SOLVED;
                    }
                } else {
                    // Неправильно — сарказм
                    failMessageIndex = RandI(0, 2);
                    state = FAIL;
                    stateTimer = 2.0f;
                }
            }
            return;
        }

        // --- FAIL: показываем сарказм 2 секунды ---
        if (state == FAIL) {
            stateTimer -= dt;
            if (stateTimer <= 0.0f) {
                state = QUIZ;
                selectedOption = 0;
            }
            return;
        }

        // --- SOLVED: идём к двери ---
        if (state == SOLVED) {
            if (player.is_in_area(doorPos.x) && IsKeyPressed(KEY_E)) {
                completed = true;
            }
            return;
        }
    }

    // Рисуем мигающий курсор терминала
    void DrawCursor(Assets& assets, int x, int y) {
        float t = (float)GetTime();
        if (fmodf(t, 1.0f) < 0.5f) {
            RuText(assets.font, u8">", x, y, 22, Color{ 0, 200, 100, 255 });
        }
    }

    void Draw(Player& player, Assets& assets) {
        int scrW = GetScreenWidth();
        int scrH = GetScreenHeight();

        // Тёмный фон — заброшенная комната с терминалом
        ClearBackground(Color{ 10, 10, 15, 255 });

        // Пол — чуть светлее фона
        DrawRectangle(0, scrH * 0.82f, scrW, scrH * 0.18f, Color{ 18, 18, 22, 255 });

        // Заголовок этажа
        RuText(assets.font, u8"ЭТАЖ 1 — ПРИВЕТСТВИЕ", scrW / 2 - 220, 8, 22, Color{ 60, 100, 60, 255 });

        // === Терминал (ЭЛТ-монитор) ===
        int termX = (int)(scrW * 0.08f);
        int termY = (int)(scrH * 0.10f);
        int termW = (int)(scrW * 0.84f);
        int termH = (int)(scrH * 0.68f);

        // Корпус монитора (тёмный пластик)
        DrawRectangle(termX - 10, termY - 10, termW + 20, termH + 20, Color{ 18, 22, 18, 255 });
        // Экран (тёмно-зелёный, как старый ЭЛТ)
        DrawRectangle(termX, termY, termW, termH, Color{ 3, 12, 3, 255 });
        // Рамка — зелёно-бирюзовая
        DrawRectangleLines(termX, termY, termW, termH, Color{ 0, 130, 110, 255 });
        // CRT-эффект — внутренняя подсветка рамки
        DrawRectangleLines(termX + 3, termY + 3, termW - 6, termH - 6, Color{ 0, 80, 60, 120 });

        // Свечение экрана
        DrawGlow({ (float)(termX + termW / 2), (float)(termY + termH / 2) },
                 200.0f, Color{ 0, 40, 20, 30 }, Color{ 0, 40, 20, 0 }, 6);

        // Отступы для текста внутри терминала
        int tx = termX + 30;
        int ty = termY + 25;
        int lh = 36;   // высота строки

        // Мигающий курсор «>»
        DrawCursor(assets, tx - 22, ty);

        // === WAITING ===
        if (state == WAITING) {
            RuText(assets.font, u8"СИСТЕМА А.У.Р.А. v3.14", tx, ty, 20, Color{ 0, 180, 100, 255 });
            RuText(assets.font, u8"НАЖМИТЕ [E] ДЛЯ АВТОРИЗАЦИИ", tx, ty + lh * 2, 26, YELLOW);

            // Подсказка [E] мигает, когда игрок рядом
            if (player.is_in_area(terminalCenterX)) {
                float pulse = 0.5f + 0.5f * sinf((float)GetTime() * 5.0f);
                unsigned char a = (unsigned char)(100 + 155 * pulse);
                RuText(assets.font, u8"  [E]", tx + RuMeasure(assets.font, u8"НАЖМИТЕ [E] ДЛЯ АВТОРИЗАЦИИ", 26),
                       ty + lh * 2, 26, Color{ 255, 255, 100, a });
            }
        }

        // === INTRO ===
        if (state == INTRO) {
            RuText(assets.font, u8"СИСТЕМА А.У.Р.А. v3.14", tx, ty, 20, Color{ 0, 180, 100, 255 });

            const char* introLines[4] = {
                u8"СИСТЕМА: Товарищ Коротков.",
                u8"СИСТЕМА: Ваш сон на рабочем месте составил 43 821 час.",
                u8"СИСТЕМА: За это время комплекс был законсервирован. Лифт обесточен.",
                u8"СИСТЕМА: Для допуска к щитку пройдите тест по электробезопасности."
            };

            for (int i = 0; i < introLineShown; i++) {
                RuText(assets.font, introLines[i], tx, ty + lh * (i + 1), 20, Color{ 0, 200, 100, 255 });
            }
        }

        // === QUIZ ===
        if (state == QUIZ) {
            RuText(assets.font, u8"ТЕСТ ПО ЭЛЕКТРОБЕЗОПАСНОСТИ", tx, ty, 22, Color{ 0, 180, 100, 255 });
            RuText(assets.font, TextFormat(u8"Вопрос %d / 3", currentQuestion + 1), tx, ty + lh, 18, Color{ 0, 150, 80, 255 });

            // Текст вопроса
            RuText(assets.font, questionText[currentQuestion], tx, ty + lh * 2, 22, RAYWHITE);

            // Варианты ответа
            int optY = ty + lh * 4;
            for (int i = 0; i < 4; i++) {
                bool sel = (i == selectedOption);
                Color col = sel ? YELLOW : Color{ 0, 160, 80, 255 };
                const char* prefix = sel ? u8"  \u25ba " : u8"    ";
                char label[8];
                sprintf(label, "%c) ", 'A' + i);

                int ox = tx;
                RuText(assets.font, prefix, ox, optY + i * lh, 22, col);
                ox += RuMeasure(assets.font, prefix, 22);
                RuText(assets.font, label, ox, optY + i * lh, 22, col);
                ox += RuMeasure(assets.font, label, 22);
                RuText(assets.font, optionText[currentQuestion][i], ox, optY + i * lh, 22, col);
            }

            // Подсказка управления
            RuText(assets.font, u8"[A/D] Выбор   [E] Подтвердить", tx, termY + termH - 35, 16, Color{ 0, 100, 60, 255 });
        }

        // === FAIL ===
        if (state == FAIL) {
            RuText(assets.font, failMessages[failMessageIndex], tx, ty + lh, 24, Color{ 220, 50, 50, 255 });
        }

        // === SOLVED ===
        if (state == SOLVED) {
            RuText(assets.font, u8"СИСТЕМА: Допуск разрешён. Питание восстановлено. Удачной смены.",
                   tx, ty + lh, 22, Color{ 0, 230, 120, 255 });
        }

        // Дверь — появляется только после решения головоломки
        if (state == SOLVED) {
            DrawTexture(assets.door_opened, (int)doorPos.x, (int)doorPos.y, WHITE);
            if (player.is_in_area(doorPos.x)) {
                RuText(assets.font, u8"[E] ВОЙТИ В ЛИФТ", (int)doorPos.x - 30, (int)doorPos.y - 40, 15, GREEN);
            }
        }

        // Статус внизу экрана
        RuText(assets.font, u8"Товарищ Коротков  |  Этаж 1", 10, scrH - 28, 15, Color{ 50, 50, 50, 255 });

        // Игрок
        player.Draw();
    }
};
