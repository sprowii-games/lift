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
    Texture2D door_opened;
    // Texture2D player_sprite;    // раскомментируй когда будет спрайт игрока
    // Texture2D bg_floor1;        // раскомментируй когда будет фон 1 этажа
    // Texture2D aura_terminal;    // раскомментируй когда будет спрайт терминала

    // --- Звуки (SFX, короткие) ---
    // Sound sfx_click;            // раскомментируй когда будет звук
    // Sound sfx_error;
    // Sound sfx_success;
    // Sound sfx_door;
    // Sound sfx_step;

    // --- Музыка (фоновая, зацикленная) ---
    // Music music_menu;           // раскомментируй когда будет музыка
    // Music music_floor1;
    // Music music_floor2;

    // --- Настройки ---
    float sfxVolume   = 0.7f;     // громкость звуков  0.0 — 1.0
    float musicVolume = 0.5f;     // громкость музыки  0.0 — 1.0
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
    a.door_opened = LoadTexture("textures/elevator_opened.png");
    // a.player_sprite = LoadTexture("textures/player.png");
    // a.bg_floor1     = LoadTexture("textures/floor1_bg.png");
    // a.aura_terminal = LoadTexture("textures/aura.png");

    // --- Звуки (нужен InitAudioDevice() в main!) ---
    // a.sfx_click   = LoadSound("sounds/click.wav");
    // a.sfx_error   = LoadSound("sounds/error.wav");
    // a.sfx_success = LoadSound("sounds/success.wav");
    // a.sfx_door    = LoadSound("sounds/door.wav");
    // a.sfx_step    = LoadSound("sounds/step.wav");
    // SetSoundVolume(a.sfx_click,   a.sfxVolume);
    // SetSoundVolume(a.sfx_error,   a.sfxVolume);
    // SetSoundVolume(a.sfx_success, a.sfxVolume);
    // SetSoundVolume(a.sfx_door,    a.sfxVolume);
    // SetSoundVolume(a.sfx_step,    a.sfxVolume);

    // --- Музыка (нужен InitAudioDevice() в main!) ---
    // a.music_menu   = LoadMusicStream("sounds/music_menu.ogg");
    // a.music_floor1 = LoadMusicStream("sounds/music_floor1.ogg");
    // a.music_floor2 = LoadMusicStream("sounds/music_floor2.ogg");
    // a.music_menu.looping   = true;
    // a.music_floor1.looping = true;
    // a.music_floor2.looping = true;
    // SetMusicVolume(a.music_menu,   a.musicVolume);
    // SetMusicVolume(a.music_floor1, a.musicVolume);
    // SetMusicVolume(a.music_floor2, a.musicVolume);
}

inline void UnloadAssets(Assets& a) {
    UnloadFont(a.font);
    UnloadTexture(a.door_opened);
    // UnloadTexture(a.player_sprite);
    // UnloadTexture(a.bg_floor1);
    // UnloadTexture(a.aura_terminal);
    // UnloadSound(a.sfx_click);
    // UnloadSound(a.sfx_error);
    // UnloadSound(a.sfx_success);
    // UnloadSound(a.sfx_door);
    // UnloadSound(a.sfx_step);
    // UnloadMusicStream(a.music_menu);
    // UnloadMusicStream(a.music_floor1);
    // UnloadMusicStream(a.music_floor2);
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
