#include "raylib.h"
#include <math.h>

#define NUM_POINTS 16  // Number of control points for higher resolution
#define RADIUS 200

typedef struct {
    float angle;  // Store the angle instead of position
    bool selected;
} ControlPoint;

void UpdateControlPoints(ControlPoint *points, int count, Vector2 center, bool shiftAll, float *globalShiftAngle);

int main(void)
{
    const int screenWidth = 800;
    const int screenHeight = 600;

    InitWindow(screenWidth, screenHeight, "Circle Spline Demo");

    Vector2 center = { screenWidth / 2.0f, screenHeight / 2.0f };
    ControlPoint controlPoints[NUM_POINTS];

    // Initialize control points around a circle
    for (int i = 0; i < NUM_POINTS; i++) {
        float angle = (2 * PI / NUM_POINTS) * i;
        controlPoints[i].angle = angle;
        controlPoints[i].selected = false;
    }

    bool shiftAll = false;
    float globalShiftAngle = 0.0f;
    Vector2 lastMousePos = {0, 0};

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        if (IsKeyPressed(KEY_SPACE)) {
            shiftAll = !shiftAll;  // Toggle shift mode
        }

        Vector2 mousePos = GetMousePosition();
        if (shiftAll && IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            float dx = mousePos.x - center.x;
            float dy = mousePos.y - center.y;
            float newAngle = atan2f(dy, dx);
            
            dx = lastMousePos.x - center.x;
            dy = lastMousePos.y - center.y;
            float lastAngle = atan2f(dy, dx);
            
            globalShiftAngle = newAngle - lastAngle;
        } else {
            globalShiftAngle = 0.0f;
        }

        lastMousePos = mousePos;
        UpdateControlPoints(controlPoints, NUM_POINTS, center, shiftAll, &globalShiftAngle);

        BeginDrawing();
        
        ClearBackground(RAYWHITE);

        // Draw the circle spline with higher resolution
        int resolution = 50;  // Number of interpolated points between control points
        for (int i = 0; i < NUM_POINTS; i++) {
            Vector2 start = (Vector2){ center.x + RADIUS * cosf(controlPoints[i].angle), center.y + RADIUS * sinf(controlPoints[i].angle) };
            Vector2 end = (Vector2){ center.x + RADIUS * cosf(controlPoints[(i + 1) % NUM_POINTS].angle), center.y + RADIUS * sinf(controlPoints[(i + 1) % NUM_POINTS].angle) };
            for (int j = 0; j < resolution; j++) {
                float t = (float)j / (float)resolution;
                Vector2 p1 = (Vector2){ start.x * (1 - t) + end.x * t, start.y * (1 - t) + end.y * t };
                t = (float)(j + 1) / (float)resolution;
                Vector2 p2 = (Vector2){ start.x * (1 - t) + end.x * t, start.y * (1 - t) + end.y * t };
                DrawLineV(p1, p2, BLUE);
            }
        }

        // Draw control points
        for (int i = 0; i < NUM_POINTS; i++) {
            Vector2 position = (Vector2){ center.x + RADIUS * cosf(controlPoints[i].angle), center.y + RADIUS * sinf(controlPoints[i].angle) };
            Color pointColor = controlPoints[i].selected ? RED : DARKGRAY;
            DrawCircleV(position, 8, pointColor);
        }

        // Draw mode indicator
        DrawText(shiftAll ? "Mode: Shift All" : "Mode: Shift Single", 10, 10, 20, shiftAll ? GREEN : BLACK);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}

void UpdateControlPoints(ControlPoint *points, int count, Vector2 center, bool shiftAll, float *globalShiftAngle)
{
    if (shiftAll) {
        for (int i = 0; i < count; i++) {
            points[i].angle += *globalShiftAngle;
        }
    } else {
        for (int i = 0; i < count; i++) {
            Vector2 position = (Vector2){ center.x + RADIUS * cosf(points[i].angle), center.y + RADIUS * sinf(points[i].angle) };
            if (CheckCollisionPointCircle(GetMousePosition(), position, 8)) {
                if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                    points[i].selected = true;
                }
            }

            if (points[i].selected) {
                Vector2 mousePos = GetMousePosition();
                float dx = mousePos.x - center.x;
                float dy = mousePos.y - center.y;
                points[i].angle = atan2f(dy, dx);

                if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
                    points[i].selected = false;
                }
            }
        }
    }
}
