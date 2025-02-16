#define RAYGUI_IMPLEMENTATION
#include "raylib.h"
#include "raygui.h"
#include "raymath.h"

#define MAX_KEYFRAMES 100
#define MAX_ANIMATIONS 5


typedef struct {
    float time;
    Vector2 position;
} Keyframe;

typedef struct {
    Keyframe keyframes[MAX_KEYFRAMES];
    int keyframeCount;
    float currentTime;
    float duration;
    bool isPlaying;
    Vector2 position;
    Color color;
} Animation;

void UpdateAnimation(Animation* anim, float deltaTime) {
    if (anim->isPlaying) {
        anim->currentTime += deltaTime;
        if (anim->currentTime > anim->duration) {
            anim->currentTime = anim->duration;
            anim->isPlaying = false;
        }
    }
}

Vector2 Interpolate(Animation* anim) {
    if (anim->keyframeCount == 0) return (Vector2){0.0f, 0.0f};

    for (int i = 0; i < anim->keyframeCount - 1; i++) {
        Keyframe kf1 = anim->keyframes[i];
        Keyframe kf2 = anim->keyframes[i + 1];
        if (anim->currentTime >= kf1.time && anim->currentTime <= kf2.time) {
            float t = (anim->currentTime - kf1.time) / (kf2.time - kf1.time);
            return Vector2Lerp(kf1.position, kf2.position, t);
        }
    }

    return anim->keyframes[anim->keyframeCount - 1].position;
}

void AddKeyframe(Animation* anim, float time, Vector2 position) {
    if (anim->keyframeCount < MAX_KEYFRAMES) {
        anim->keyframes[anim->keyframeCount].time = time;
        anim->keyframes[anim->keyframeCount].position = position;
        anim->keyframeCount++;
        if (time > anim->duration) {
            anim->duration = time;
        }
    }
}

void DrawPath(Animation* anim) {
    for (int i = 0; i < anim->keyframeCount - 1; i++) {
        DrawLineV(anim->keyframes[i].position, anim->keyframes[i + 1].position, GRAY);
    }
}

int main(void) {
    InitWindow(800, 600, "Multiple Replay Control Systems with Raygui");

    Animation animations[MAX_ANIMATIONS] = {0};
    int selectedAnimation = -1;

    // Initialize animations with keyframes
    for (int i = 0; i < MAX_ANIMATIONS; i++) {
        AddKeyframe(&animations[i], 0.0f, (Vector2){100 + i*50, 100 + i*50});
        AddKeyframe(&animations[i], 1.0f, (Vector2){700 - i*50, 100 + i*50});
        AddKeyframe(&animations[i], 2.0f, (Vector2){700 - i*50, 500 - i*50});
        AddKeyframe(&animations[i], 3.0f, (Vector2){100 + i*50, 500 - i*50});
        AddKeyframe(&animations[i], 4.0f, (Vector2){100 + i*50, 100 + i*50});
        animations[i].color = (Color){ GetRandomValue(50, 255), GetRandomValue(50, 255), GetRandomValue(50, 255), 255 };
    }

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        float deltaTime = GetFrameTime();

        BeginDrawing();
        ClearBackground(RAYWHITE);

        for (int i = 0; i < MAX_ANIMATIONS; i++) {
            UpdateAnimation(&animations[i], deltaTime);

            Vector2 circlePos = Interpolate(&animations[i]);
            animations[i].position = circlePos;

            DrawPath(&animations[i]);  // Draw the path of the animation
            DrawCircleV(circlePos, 20, animations[i].color);

            if (CheckCollisionPointCircle(GetMousePosition(), circlePos, 20) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                selectedAnimation = i;
            }
        }

        if (selectedAnimation != -1) {
            Animation* anim = &animations[selectedAnimation];

            // Play/Pause button
            if (GuiButton((Rectangle){ 10, 10, 120, 30 }, anim->isPlaying ? "Pause" : "Play")) anim->isPlaying = !anim->isPlaying;
            // Restart button
            if (GuiButton((Rectangle){ 10, 50, 120, 30 }, "Restart")) anim->currentTime = 0.0f;

            // Scrubber slider
            float scrubberPos = anim->currentTime / anim->duration * 700.0f;
            if (GuiSliderBar((Rectangle){ 140, 25, 650, 20 }, "", "", &scrubberPos, 0.0f, 700.0f)) {
                anim->currentTime = (scrubberPos / 700.0f) * anim->duration;
                anim->isPlaying = false;
            }
        }

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
