#pragma once
#include "raylib.h"

// ============================================================
//  АССЕТЫ — текстуры, звуки, музыка, шрифт
//  Чтобы добавить свой файл:
//    1. Положи файл в папку (текстуры в textures/, звуки в sounds/)
//    2. Добавь переменную в структуру ниже
//    3. Добавь LoadTexture / LoadSound / LoadMusicStream в LoadAssets()
//    4. Добавь UnloadTexture / UnloadSound / UnloadMusicStream в UnloadAssets()
//  Всё! Теперь можно использовать assets.имя в любом уровне
// ============================================================

struct Assets {
    // --- Шрифт ---
    Font font;

    // --- Текстуры (спрайты) ---
    Texture2D door_opened;                 // ================= совместимость =================
    Texture2D tex_floor;
    Texture2D tex_wall;
    Texture2D tex_elevator_closed;
    Texture2D tex_elevator_opened;
    Texture2D tex_note;
    Texture2D tex_terminal;
    Texture2D player_stand[2];
    Texture2D player_walk[4];
    Texture2D player_run[4];

    // --- Звуки (SFX, короткие) ---
    Sound sfx_elevator;

    // --- Музыка (фоновая, зацикленная) ---
    Music music_menu;
    Music music_beginning;
    Music music_first_two;
    Music music_second_two;
    Music music_end;
    Music music_floor1;                  // ================= совместимость =================
    Music music_floor2;

    // --- Настройки ---
    float sfxVolume   = 0.7f;             // громкость звуков  0.0 — 1.0
    float musicVolume = 0.5f;             // громкость музыки  0.0 — 1.0
};

inline void LoadAssets(Assets& a) {
    // --- Шрифт (с кириллицей) ---
    int codepoints[2048];
    for (int i = 0; i < 2048; i++) codepoints[i] = i + 32;
    a.font = LoadFontEx("handjet.ttf", 64, codepoints, 2048);
    if (a.font.texture.id == 0)
        a.font = LoadFontEx("C:/Windows/Fonts/segoeui.ttf", 64, codepoints, 2048);
    if (a.font.texture.id == 0)
        a.font = GetFontDefault();

    // --- Текстуры ---
    a.door_opened         = LoadTexture("textures/elevator_opened.png");
    a.tex_floor           = LoadTexture("textures/floor.png");
    a.tex_wall            = LoadTexture("textures/wall.png");
    a.tex_elevator_closed = LoadTexture("textures/elevator-closed.png");
    a.tex_elevator_opened = LoadTexture("textures/elevator-opened.png");
    a.tex_note            = LoadTexture("textures/note.png");
    a.tex_terminal        = LoadTexture("textures/terminal.png");

    // --- Спрайты игрока: стоит (2 кадра) ---
    a.player_stand[0] = LoadTexture("textures/player/idle/sprite_0.png");
    a.player_stand[1] = LoadTexture("textures/player/idle/sprite_1.png");

    // --- Спрайты игрока: идёт (4 кадра) ---
    a.player_walk[0] = LoadTexture("textures/player/walk/sprite_0.png");
    a.player_walk[1] = LoadTexture("textures/player/walk/sprite_1.png");
    a.player_walk[2] = LoadTexture("textures/player/walk/sprite_2.png");
    a.player_walk[3] = LoadTexture("textures/player/walk/sprite_3.png");

    // --- Спрайты игрока: бежит (4 кадра), кадр 0 имеет особое имя ---
    a.player_run[0] = LoadTexture("textures/player/run/sprite_0.png");
    a.player_run[1] = LoadTexture("textures/player/run/sprite_1.png");
    a.player_run[2] = LoadTexture("textures/player/run/sprite_2.png");
    a.player_run[3] = LoadTexture("textures/player/run/sprite_3.png");

    // --- Звуки (нужен InitAudioDevice() в main!) ---
    a.sfx_elevator = LoadSound("sounds/звук при езде на лифте.mp3");
    SetSoundVolume(a.sfx_elevator, a.sfxVolume);

    // --- Музыка (нужен InitAudioDevice() в main!) ---
    a.music_menu       = LoadMusicStream("sounds/menu.ogg");
    a.music_beginning  = LoadMusicStream("sounds/begining(first level).ogg");
    a.music_first_two  = LoadMusicStream("sounds/first two levels.ogg");
    a.music_second_two = LoadMusicStream("sounds/second two levels.ogg");
    a.music_end        = LoadMusicStream("sounds/end(last level).ogg");

    a.music_menu.looping       = true;
    a.music_beginning.looping  = true;
    a.music_first_two.looping  = true;
    a.music_second_two.looping = true;
    a.music_end.looping        = true;

    SetMusicVolume(a.music_menu,       a.musicVolume);
    SetMusicVolume(a.music_beginning,  a.musicVolume);
    SetMusicVolume(a.music_first_two,  a.musicVolume);
    SetMusicVolume(a.music_second_two, a.musicVolume);
    SetMusicVolume(a.music_end,        a.musicVolume);
}

inline void UnloadAssets(Assets& a) {
    UnloadFont(a.font);

    UnloadTexture(a.door_opened);
    UnloadTexture(a.tex_floor);
    UnloadTexture(a.tex_wall);
    UnloadTexture(a.tex_elevator_closed);
    UnloadTexture(a.tex_elevator_opened);
    UnloadTexture(a.tex_note);
    UnloadTexture(a.tex_terminal);

    UnloadTexture(a.player_stand[0]);
    UnloadTexture(a.player_stand[1]);

    UnloadTexture(a.player_walk[0]);
    UnloadTexture(a.player_walk[1]);
    UnloadTexture(a.player_walk[2]);
    UnloadTexture(a.player_walk[3]);

    UnloadTexture(a.player_run[0]);
    UnloadTexture(a.player_run[1]);
    UnloadTexture(a.player_run[2]);
    UnloadTexture(a.player_run[3]);

    UnloadSound(a.sfx_elevator);

    UnloadMusicStream(a.music_menu);
    UnloadMusicStream(a.music_beginning);
    UnloadMusicStream(a.music_first_two);
    UnloadMusicStream(a.music_second_two);
    UnloadMusicStream(a.music_end);
}

// ============================================================
//  Хелперы для текста
// ============================================================
inline void RuText(Font f, const char* text, int x, int y, int fontSize, Color color) {
    DrawTextEx(f, text, {(float)x, (float)y}, (float)fontSize, 1.0f, color);
}
inline int RuMeasure(Font f, const char* text, int fontSize) {
    return (int)MeasureTextEx(f, text, (float)fontSize, 1.0f).x;
}
