#include "raylib.h"

// Function to convert screen coordinates to normalized coordinates
Vector2 ScreenToNormalized(Vector2 screenPos) {
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();

    Vector2 normalizedPos;
    normalizedPos.x = screenPos.x / screenWidth;
    normalizedPos.y = screenPos.y / screenHeight;

    return normalizedPos;
}

// Function to convert normalized coordinates to screen coordinates
Vector2 NormalizedToScreen(Vector2 normalizedPos) {
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();

    Vector2 screenPos;
    screenPos.x = normalizedPos.x * screenWidth;
    screenPos.y = normalizedPos.y * screenHeight;

    return screenPos;
}

// Function to convert world coordinates to screen coordinates (considering world offset and scale)
Vector2 WorldToScreen(Vector2 worldPos, Vector2 worldOffset, float worldScale) {
    Vector2 screenPos;
    screenPos.x = (worldPos.x - worldOffset.x) * worldScale;
    screenPos.y = (worldPos.y - worldOffset.y) * worldScale;

    return screenPos;
}

// Function to convert screen coordinates to world coordinates (considering world offset and scale)
Vector2 ScreenToWorld(Vector2 screenPos, Vector2 worldOffset, float worldScale) {
    Vector2 worldPos;
    worldPos.x = screenPos.x / worldScale + worldOffset.x;
    worldPos.y = screenPos.y / worldScale + worldOffset.y;

    return worldPos;
}

int main(void) {
    // Initialization
    int screenWidth = 800;
    int screenHeight = 450;
    InitWindow(screenWidth, screenHeight, "raylib - coordinate mapping");

    Vector2 worldOffset = { 0.0f, 0.0f };
    float worldScale = 1.0f;

    // Main game loop
    while (!WindowShouldClose()) {
        // Update (if needed)
        Vector2 screenPos = GetMousePosition();
        Vector2 normalizedPos = ScreenToNormalized(screenPos);
        Vector2 worldPos = ScreenToWorld(screenPos, worldOffset, worldScale);

        // Draw
        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawText(TextFormat("Screen Pos: [%.2f, %.2f]", screenPos.x, screenPos.y), 10, 10, 20, DARKGRAY);
        DrawText(TextFormat("Normalized Pos: [%.2f, %.2f]", normalizedPos.x, normalizedPos.y), 10, 40, 20, DARKGRAY);
        DrawText(TextFormat("World Pos: [%.2f, %.2f]", worldPos.x, worldPos.y), 10, 70, 20, DARKGRAY);

        EndDrawing();
    }

    // Deinitialization
    CloseWindow();

    return 0;
}
