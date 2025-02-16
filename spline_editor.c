#include "raylib.h"
#include <stdio.h>
#include <math.h>

#define MAX_POINTS 100

typedef struct {
    Vector2 points[MAX_POINTS];
    int count;
} Spline;

Spline spline = {0};

void GenerateCirclePoints(Vector2 center, float radius, int divisions) {
    spline.count = 0;
    float angleStep = 2 * PI / divisions;
    for (int i = 0; i < divisions; i++) {
        float angle = i * angleStep;
        Vector2 point = {center.x + radius * cosf(angle), center.y + radius * sinf(angle)};
        spline.points[spline.count++] = point;
    }
}

void AddPoint(Vector2 point) {
    if (spline.count < MAX_POINTS) {
        spline.points[spline.count++] = point;
    }
}

void RemovePoint(int index) {
    if (index >= 0 && index < spline.count) {
        for (int i = index; i < spline.count - 1; i++) {
            spline.points[i] = spline.points[i + 1];
        }
        spline.count--;
    }
}

Vector2 GetCatmullRomPoint(Vector2 p0, Vector2 p1, Vector2 p2, Vector2 p3, float t) {
    float t2 = t * t;
    float t3 = t2 * t;

    float x = 0.5f * ((2.0f * p1.x) +
                      (-p0.x + p2.x) * t +
                      (2.0f * p0.x - 5.0f * p1.x + 4.0f * p2.x - p3.x) * t2 +
                      (-p0.x + 3.0f * p1.x - 3.0f * p2.x + p3.x) * t3);

    float y = 0.5f * ((2.0f * p1.y) +
                      (-p0.y + p2.y) * t +
                      (2.0f * p0.y - 5.0f * p1.y + 4.0f * p2.y - p3.y) * t2 +
                      (-p0.y + 3.0f * p1.y - 3.0f * p2.y + p3.y) * t3);

    return (Vector2){x, y};
}

void DrawSpline() {
    if (spline.count < 4) return;

    for (int i = 0; i < spline.count; i++) {
        Vector2 p0 = spline.points[(i - 1 + spline.count) % spline.count];
        Vector2 p1 = spline.points[i];
        Vector2 p2 = spline.points[(i + 1) % spline.count];
        Vector2 p3 = spline.points[(i + 2) % spline.count];

        for (float t = 0; t <= 1.0f; t += 0.01f) {
            Vector2 point = GetCatmullRomPoint(p0, p1, p2, p3, t);
            DrawCircleV(point, 2, RED);
        }
    }
}

int main(void) {
    const int screenWidth = 800;
    const int screenHeight = 600;

    InitWindow(screenWidth, screenHeight, "Closed Loop Spline Editor with Raylib");

    SetTargetFPS(60);

    Vector2 center = {screenWidth / 2, screenHeight / 2};
    float radius = 100.0f;
    int divisions = 8;
    GenerateCirclePoints(center, radius, divisions);

    Vector2 selectedPoint = {0};
    int selectedIndex = -1;
    bool isDragging = false;

    while (!WindowShouldClose()) {
        Vector2 mousePosition = GetMousePosition();

        // Add point
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            AddPoint(mousePosition);
        }

        // Select point
        if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
            for (int i = 0; i < spline.count; i++) {
                if (CheckCollisionPointCircle(mousePosition, spline.points[i], 5)) {
                    selectedIndex = i;
                    isDragging = true;
                    break;
                }
            }
        }

        // Drag point
        if (isDragging && IsMouseButtonDown(MOUSE_RIGHT_BUTTON)) {
            spline.points[selectedIndex] = mousePosition;
        }

        // Release point
        if (isDragging && IsMouseButtonReleased(MOUSE_RIGHT_BUTTON)) {
            isDragging = false;
        }

        // Remove point
        if (IsKeyPressed(KEY_DELETE) && selectedIndex != -1) {
            RemovePoint(selectedIndex);
            selectedIndex = -1;
        }

        BeginDrawing();

        ClearBackground(RAYWHITE);

        DrawSpline();

        for (int i = 0; i < spline.count; i++) {
            DrawCircleV(spline.points[i], 5, BLUE);
        }

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
