#pragma once
#include <raylib.h>
#include <cmath>

// Lerp
inline float LerpF(float a, float b, float t) {
    return a + (b - a) * t;
}

inline Vector2 LerpV2(Vector2 a, Vector2 b, float t) {
    return { LerpF(a.x, b.x, t), LerpF(a.y, b.y, t) };
}

inline Color LerpColor(Color a, Color b, float t) {
    return {
        (unsigned char)LerpF(a.r, b.r, t),
        (unsigned char)LerpF(a.g, b.g, t),
        (unsigned char)LerpF(a.b, b.b, t),
        (unsigned char)LerpF(a.a, b.a, t)
    };
}

// Clamp
inline float ClampF(float v, float mn, float mx) {
    return (v < mn) ? mn : (v > mx) ? mx : v;
}

// Random helpers
inline float RandF(float min, float max) {
    return min + (max - min) * ((float)GetRandomValue(0, 10000) / 10000.0f);
}

inline int RandI(int min, int max) {
    return GetRandomValue(min, max);
}

// Distance
inline float DistV2(Vector2 a, Vector2 b) {
    return sqrtf((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
}

// Rectangle expand
inline Rectangle ExpandRect(Rectangle r, float amount) {
    return { r.x - amount, r.y - amount, r.width + amount * 2.0f, r.height + amount * 2.0f };
}

// Draw glow behind a circle (simple multi-circle approach)
inline void DrawGlow(Vector2 pos, float radius, Color centerColor, Color outerColor, int rings = 8) {
    for (int i = rings; i >= 0; --i) {
        float t = (float)i / (float)rings;
        float r = radius + t * radius * 0.8f;
        Color c = LerpColor(centerColor, outerColor, t);
        DrawCircleV(pos, r, Fade(c, (1.0f - t) * centerColor.a / 255.0f));
    }
}
