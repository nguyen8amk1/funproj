#include "raylib.h"
#include <math.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

int main(void) {
    // Initialize window
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Raylib Desmos Clone");
    SetTargetFPS(60);

    // Camera settings
    Vector2 offset = { SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 }; // Center of screen
    float zoom = 50.0f; // Pixels per unit

    while (!WindowShouldClose()) {
        // Handle zoom
        float zoomFactor = GetMouseWheelMove();
        if (zoomFactor != 0) {
            float scaleFactor = 1.1f;
            float prevZoom = zoom;
            zoom *= (zoomFactor > 0) ? scaleFactor : (1 / scaleFactor);

            Vector2 mousePos = GetMousePosition();
            offset.x -= (mousePos.x - offset.x) * (zoom - prevZoom) / prevZoom;
            offset.y -= (mousePos.y - offset.y) * (zoom - prevZoom) / prevZoom;
        }

        // Handle panning
        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
            Vector2 delta = GetMouseDelta();
            offset.x += delta.x;
            offset.y += delta.y;
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Draw grid
        int numLines = 100;
        float step = zoom / 10;
        Color gridColor = LIGHTGRAY;

        for (int i = -numLines; i <= numLines; i++) {
            float x = offset.x + i * step;
            float y = offset.y + i * step;
            DrawLine(x, 0, x, SCREEN_HEIGHT, gridColor);
            DrawLine(0, y, SCREEN_WIDTH, y, gridColor);
        }

        // Draw axes
        DrawLine(0, offset.y, SCREEN_WIDTH, offset.y, BLACK);
        DrawLine(offset.x, 0, offset.x, SCREEN_HEIGHT, BLACK);

        // Plot y = sin(x)
        Color functionColor = RED;
        float precision = 1.0f; // Smaller = smoother

        for (float sx = 0; sx < SCREEN_WIDTH; sx += precision) {
            // Convert screen x to graph x
            float x1 = (sx - offset.x) / zoom;
            float x2 = ((sx + precision) - offset.x) / zoom;

            // Compute function values
            float y1 = sin(x1);
            float y2 = sin(x2);

            // Convert graph y to screen y
            float sy1 = offset.y - y1 * zoom;
            float sy2 = offset.y - y2 * zoom;

            // Draw function segment
            DrawLine(sx, sy1, sx + precision, sy2, functionColor);
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
