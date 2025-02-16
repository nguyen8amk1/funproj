#include "raylib.h"
#include "raymath.h"
#include <math.h>
#include <stddef.h>

typedef struct {
    Vector2 point1;
    Vector2 point2;
    bool isCoincident;
    bool isHorizontal;
    bool isVertical;
    bool hasFixedDistance;
    bool hasFixedAngle;
    float distance;
    float angle;
    Vector2 *coincidentPoint;
} ConstrainedPoint;

void ApplyConstraints(ConstrainedPoint *point) {
    if (point->isCoincident && point->coincidentPoint) {
        point->point2 = *point->coincidentPoint;
    }
    if (point->isHorizontal) {
        point->point2.y = point->point1.y;
    }
    if (point->isVertical) {
        point->point2.x = point->point1.x;
    }
    if (point->hasFixedDistance) {
        Vector2 dir = Vector2Normalize(Vector2Subtract(point->point2, point->point1));
        point->point2 = Vector2Add(point->point1, Vector2Scale(dir, point->distance));
    }
    if (point->hasFixedAngle) {
        Vector2 dir = { cosf(point->angle), sinf(point->angle) };
        point->point2 = Vector2Add(point->point1, Vector2Scale(dir, Vector2Length(Vector2Subtract(point->point2, point->point1))));
    }
}

void ToggleConstraint(ConstrainedPoint *point, int constraint) {
    switch (constraint) {
        case 0: point->isCoincident = !point->isCoincident; break;
        case 1: point->isHorizontal = !point->isHorizontal; break;
        case 2: point->isVertical = !point->isVertical; break;
        case 3: point->hasFixedDistance = !point->hasFixedDistance; break;
        case 4: point->hasFixedAngle = !point->hasFixedAngle; break;
    }
}

int main(void) {
    // Initialization
    const int screenWidth = 800;
    const int screenHeight = 600;

    InitWindow(screenWidth, screenHeight, "Geometric Constraints Points Demo");

    ConstrainedPoint points[5];
    int pointCount = 0;
    points[pointCount++] = (ConstrainedPoint){{100, 200}, {300, 200}, false, true, false, false, false, 0, 0, NULL};
    points[pointCount++] = (ConstrainedPoint){{400, 100}, {400, 300}, false, false, true, false, false, 0, 0, NULL};
    points[pointCount++] = (ConstrainedPoint){{500, 300}, {600, 300}, true, false, false, false, false, 0, 0, &points[0].point2};
    points[pointCount++] = (ConstrainedPoint){{200, 400}, {200, 500}, false, false, false, true, false, 100, 0, NULL};
    points[pointCount++] = (ConstrainedPoint){{300, 500}, {400, 600}, false, false, false, false, true, 0, PI/4, NULL};

    SetTargetFPS(60);

    int selectedPoint = 0;
    int selectedConstraint = 0;

    // Main game loop
    while (!WindowShouldClose()) {
        // Update
        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
            Vector2 mousePosition = GetMousePosition();
            points[selectedPoint].point2 = mousePosition;
        }

        if (IsKeyPressed(KEY_UP)) selectedPoint = (selectedPoint + 1) % pointCount;
        if (IsKeyPressed(KEY_DOWN)) selectedPoint = (selectedPoint - 1 + pointCount) % pointCount;
        if (IsKeyPressed(KEY_RIGHT)) selectedConstraint = (selectedConstraint + 1) % 5;
        if (IsKeyPressed(KEY_LEFT)) selectedConstraint = (selectedConstraint - 1 + 5) % 5;
        if (IsKeyPressed(KEY_SPACE)) ToggleConstraint(&points[selectedPoint], selectedConstraint);

        for (int i = 0; i < pointCount; i++) {
            ApplyConstraints(&points[i]);
        }

        // Draw
        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawText("Use arrow keys to select point and constraint, SPACE to toggle", 10, 10, 20, DARKGRAY);
        DrawText(TextFormat("Selected Point: %d", selectedPoint), 10, 40, 20, DARKGRAY);
        DrawText(TextFormat("Selected Constraint: %d", selectedConstraint), 10, 70, 20, DARKGRAY);

        for (int i = 0; i < pointCount; i++) {
            DrawLineV(points[i].point1, points[i].point2, RED);
            DrawCircleV(points[i].point1, 5, BLUE);
            DrawCircleV(points[i].point2, 5, GREEN);
        }

        EndDrawing();
    }

    // De-Initialization
    CloseWindow();

    return 0;
}
