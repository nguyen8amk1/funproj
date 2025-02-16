#define RAYGUI_IMPLEMENTATION
#include "raylib.h"
#include "raygui.h"
#include "raymath.h"

#define MAX_CIRCLES 100

typedef struct Circle {
    Vector2 position;
    float radius;
    bool dragging;
    bool selected;
} Circle;

int main(void)
{
    // Initialization
    const int screenWidth = 800;
    const int screenHeight = 600;

    InitWindow(screenWidth, screenHeight, "raylib - multi-select and drag circles demo with raygui");

    Circle circles[MAX_CIRCLES];
    int circleCount = 0;

    bool draggingSelection = false;
    bool draggingCircles = false;
    Vector2 selectionStart = { 0, 0 };
    Rectangle selectionRect = { 0, 0, 0, 0 };
    Vector2 initialMousePos = { 0, 0 };
    Vector2 initialCirclePositions[MAX_CIRCLES];

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        if (GuiButton((Rectangle){ 10, 10, 100, 40 }, "Spawn Circle") && circleCount < MAX_CIRCLES)
        {
            circles[circleCount].position = GetMousePosition();
            circles[circleCount].radius = 20.0f;
            circles[circleCount].dragging = false;
            circles[circleCount].selected = false;
            circleCount++;
        }

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            Vector2 mousePosition = GetMousePosition();
            bool circleClicked = false;

            // Check if any circle is clicked to start dragging
            for (int i = 0; i < circleCount; i++)
            {
                if (CheckCollisionPointCircle(mousePosition, circles[i].position, circles[i].radius))
                {
                    circleClicked = true;
                    circles[i].dragging = true;
                    circles[i].selected = true;
                }
            }

            // If no circle is clicked, start rectangle selection
            if (!circleClicked)
            {
                draggingSelection = true;
                selectionStart = mousePosition;
                selectionRect = (Rectangle){ selectionStart.x, selectionStart.y, 0, 0 };

                // Deselect all circles
                for (int i = 0; i < circleCount; i++)
                {
                    circles[i].selected = false;
                }
            }
            else
            {
                // Start dragging selected circles
                draggingCircles = true;
                initialMousePos = mousePosition;
                for (int i = 0; i < circleCount; i++)
                {
                    if (circles[i].selected)
                    {
                        initialCirclePositions[i] = circles[i].position;
                    }
                }
            }
        }

        // Update selection rectangle while dragging
        if (draggingSelection)
        {
            Vector2 mousePosition = GetMousePosition();
            selectionRect.width = mousePosition.x - selectionStart.x;
            selectionRect.height = mousePosition.y - selectionStart.y;

            // Select circles within the selection rectangle
            for (int i = 0; i < circleCount; i++)
            {
                if (CheckCollisionCircleRec(circles[i].position, circles[i].radius, selectionRect))
                {
                    circles[i].selected = true;
                }
                else
                {
                    circles[i].selected = false;
                }
            }
        }

        // Drag the selected circles with the mouse
        if (draggingCircles)
        {
            Vector2 mouseDelta = Vector2Subtract(GetMousePosition(), initialMousePos);
            for (int i = 0; i < circleCount; i++)
            {
                if (circles[i].selected)
                {
                    circles[i].position = Vector2Add(initialCirclePositions[i], mouseDelta);
                }
            }
        }

        // Stop dragging when mouse button is released
        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
        {
            for (int i = 0; i < circleCount; i++)
            {
                circles[i].dragging = false;
            }
            draggingSelection = false;
            draggingCircles = false;
        }

        // Draw
        BeginDrawing();

        ClearBackground(RAYWHITE);

        // Draw all circles
        for (int i = 0; i < circleCount; i++)
        {
            Color color = circles[i].selected ? BLUE : MAROON;
            DrawCircleV(circles[i].position, circles[i].radius, color);
            DrawCircleLines(circles[i].position.x, circles[i].position.y, circles[i].radius, DARKGRAY);
        }

        // Draw selection rectangle
        if (draggingSelection)
        {
            DrawRectangleLinesEx(selectionRect, 1, DARKGRAY);
        }

        EndDrawing();
    }

    // De-Initialization
    CloseWindow();        // Close window and OpenGL context

    return 0;
}
