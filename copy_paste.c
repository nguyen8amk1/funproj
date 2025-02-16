#define RAYGUI_IMPLEMENTATION
#include "raylib.h"
#include "raymath.h"
#include "raygui.h"
#include <stdlib.h>
#include <string.h>

#define MAX_SHAPES 50

typedef struct {
    Rectangle rect;
    Color color;
    bool selected;
} Shape;

typedef struct {
    Shape shapes[MAX_SHAPES];
    int count;
} ShapeCollection;

ShapeCollection copiedShapes;
ShapeCollection shapes;
Shape *selectedShape = NULL;
Vector2 mouseStartPos;

void InitializeShapes(ShapeCollection *shapes) {
    shapes->shapes[0] = (Shape){{100, 100, 100, 50}, RED, false};
    shapes->shapes[1] = (Shape){{300, 100, 100, 50}, GREEN, false};
    shapes->shapes[2] = (Shape){{500, 100, 100, 50}, BLUE, false};
    shapes->count = 3;
}

void CopySelectedShapes(ShapeCollection *source, ShapeCollection *dest) {
    dest->count = 0;
    for (int i = 0; i < source->count; i++) {
        if (source->shapes[i].selected && dest->count < MAX_SHAPES) {
            dest->shapes[dest->count++] = source->shapes[i];
        }
    }
}

void PasteShapes(ShapeCollection *source, ShapeCollection *dest, Vector2 offset) {
    for (int i = 0; i < source->count; i++) {
        if (dest->count < MAX_SHAPES) {
            Shape newShape = source->shapes[i];
            newShape.rect.x += offset.x;
            newShape.rect.y += offset.y;
            newShape.selected = false;
            dest->shapes[dest->count++] = newShape;
        }
    }
}

int main(void)
{
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "Copy and Paste Shapes with Raygui Buttons");

    InitializeShapes(&shapes);

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        // Handle mouse input
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            Vector2 mousePos = GetMousePosition();
            for (int i = 0; i < shapes.count; i++) {
                if (CheckCollisionPointRec(mousePos, shapes.shapes[i].rect)) {
                    shapes.shapes[i].selected = !shapes.shapes[i].selected;
                    selectedShape = &shapes.shapes[i];
                    mouseStartPos = mousePos;
                    break;
                }
            }
        }

        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON) && selectedShape != NULL) {
            Vector2 mousePos = GetMousePosition();
            Vector2 delta = Vector2Subtract(mousePos, mouseStartPos);
            selectedShape->rect.x += delta.x;
            selectedShape->rect.y += delta.y;
            mouseStartPos = mousePos;
        }

        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
            selectedShape = NULL;
        }

        // Handle copy and paste buttons
        if (GuiButton((Rectangle){ 10, 10, 100, 30 }, "Copy")) {
            CopySelectedShapes(&shapes, &copiedShapes);
        }

        if (GuiButton((Rectangle){ 120, 10, 100, 30 }, "Paste")) {
            Vector2 mousePos = GetMousePosition();
            PasteShapes(&copiedShapes, &shapes, mousePos);
        }

        // Draw
        BeginDrawing();
        ClearBackground(RAYWHITE);

        for (int i = 0; i < shapes.count; i++) {
            DrawRectangleRec(shapes.shapes[i].rect, shapes.shapes[i].color);
            if (shapes.shapes[i].selected) {
                DrawRectangleLinesEx(shapes.shapes[i].rect, 2, BLACK);
            }
        }

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
