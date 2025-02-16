#include "raylib.h"

#define MAX_POINTS 100
#define POINT_RADIUS 5

typedef struct {
    Vector2 points[MAX_POINTS];
    int pointCount;
    int tempPointCount;
    Vector2 tempPoints[2];
} StickFigure;

void DrawStickFigure(StickFigure *stickFigure);
int GetPointIndexAtPosition(StickFigure *stickFigure, Vector2 position);

int main(void)
{
    // Initialization
    const int screenWidth = 800;
    const int screenHeight = 600;
    
    InitWindow(screenWidth, screenHeight, "Stick Figure Drawing - raylib");
    SetTargetFPS(60);
    
    StickFigure stickFigure = { 0 };

    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        Vector2 mousePosition = GetMousePosition();
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            int pointIndex = GetPointIndexAtPosition(&stickFigure, mousePosition);
            if (pointIndex != -1) {
                // Select existing point if clicked near it
                stickFigure.tempPoints[stickFigure.tempPointCount] = stickFigure.points[pointIndex];
            } else if (stickFigure.pointCount < MAX_POINTS) {
                // Add new point to tempPoints
                stickFigure.tempPoints[stickFigure.tempPointCount] = mousePosition;
            }
            stickFigure.tempPointCount++;

            // If we have two points in tempPoints, add them to the main points array
            if (stickFigure.tempPointCount == 2) {
                stickFigure.points[stickFigure.pointCount] = stickFigure.tempPoints[0];
                stickFigure.pointCount++;
                stickFigure.points[stickFigure.pointCount] = stickFigure.tempPoints[1];
                stickFigure.pointCount++;
                stickFigure.tempPointCount = 0;
            }
        }
        
        // Draw
        BeginDrawing();
        
        ClearBackground(RAYWHITE);
        
        DrawText("Left click to add points. Draw segments pair-wise.", 10, 10, 20, DARKGRAY);
        DrawStickFigure(&stickFigure);

        // Draw debug information
        DrawText(TextFormat("Total Points: %d", stickFigure.pointCount), 10, 40, 20, BLUE);
        DrawText(TextFormat("Temporary Points: %d", stickFigure.tempPointCount), 10, 60, 20, BLUE);
        DrawText(TextFormat("Mouse Position: [ %.2f, %.2f ]", mousePosition.x, mousePosition.y), 10, 80, 20, BLUE);
        
        EndDrawing();
    }
    
    CloseWindow(); // Close window and OpenGL context
    
    return 0;
}

void DrawStickFigure(StickFigure *stickFigure)
{
    for (int i = 0; i < stickFigure->pointCount; i++)
    {
        DrawCircleV(stickFigure->points[i], POINT_RADIUS, RED); // Draw the points
    }
    
    for (int i = 0; i < stickFigure->pointCount - 1; i += 2)
    {
        DrawLineV(stickFigure->points[i], stickFigure->points[i + 1], BLACK); // Draw pair-wise segments
    }
}

int GetPointIndexAtPosition(StickFigure *stickFigure, Vector2 position)
{
    for (int i = 0; i < stickFigure->pointCount; i++)
    {
        if (CheckCollisionPointCircle(position, stickFigure->points[i], POINT_RADIUS))
        {
            return i;
        }
    }
    return -1;
}
