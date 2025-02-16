#include "raylib.h"

#define MAX_POINTS 100

typedef struct {
    Vector2 points[MAX_POINTS];
    int pointCount;
} StickFigure;

void DrawStickFigure(StickFigure *stickFigure);

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
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && stickFigure.pointCount < MAX_POINTS)
        {
            stickFigure.points[stickFigure.pointCount] = GetMousePosition();
            stickFigure.pointCount++;
        }
        
        // Draw
        BeginDrawing();
        
        ClearBackground(RAYWHITE);
        
        DrawText("Left click to add points. Draw segments between points.", 10, 10, 20, DARKGRAY);
        DrawStickFigure(&stickFigure);
        
        EndDrawing();
    }
    
    CloseWindow(); // Close window and OpenGL context
    
    return 0;
}

void DrawStickFigure(StickFigure *stickFigure)
{
    for (int i = 0; i < stickFigure->pointCount; i++)
    {
        DrawCircleV(stickFigure->points[i], 5, RED); // Draw the points
    }
    
    for (int i = 0; i < stickFigure->pointCount - 1; i += 2)
    {
        DrawLineV(stickFigure->points[i], stickFigure->points[i + 1], BLACK); // Draw pair-wise segments
    }
}
