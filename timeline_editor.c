#define RAYGUI_IMPLEMENTATION
#include "raylib.h"
#include "raygui.h"

#define MAX_FRAMES 120
#define MAX_ELEMENTS 5

typedef struct {
    float position;
    int order;
    Color color;
} TimelineElement;

void SwapElements(TimelineElement *a, TimelineElement *b) {
    TimelineElement temp = *a;
    *a = *b;
    *b = temp;
}

int main(void)
{
    // Initialization
    const int screenWidth = 800;
    const int screenHeight = 600;

    InitWindow(screenWidth, screenHeight, "Raylib Timeline Editor");
    GuiLoadStyleDefault();

    int frameCount = 60;
    int currentFrame = 0;
    int elementCount = MAX_ELEMENTS;
    TimelineElement elements[MAX_ELEMENTS];

    for (int i = 0; i < elementCount; i++) {
        elements[i].position = i * 10.0f;
        elements[i].order = i;
        elements[i].color = (Color){ GetRandomValue(0, 255), GetRandomValue(0, 255), GetRandomValue(0, 255), 255 };
    }

    SetTargetFPS(60);

    // Main game loop
    while (!WindowShouldClose())
    {
        // Update
        for (int i = 0; i < elementCount; i++) {
            if (GuiButton((Rectangle){ 10, 100 + i * 30, 20, 20 }, "<")) {
                elements[i].position -= 1.0f;
            }
            if (GuiButton((Rectangle){ 40, 100 + i * 30, 20, 20 }, ">")) {
                elements[i].position += 1.0f;
            }
            if (i > 0 && GuiButton((Rectangle){ 70, 100 + i * 30, 20, 20 }, "^")) {
                SwapElements(&elements[i], &elements[i - 1]);
            }
            if (i < elementCount - 1 && GuiButton((Rectangle){ 100, 100 + i * 30, 20, 20 }, "v")) {
                SwapElements(&elements[i], &elements[i + 1]);
            }
        }

        // Draw
        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Timeline UI
        GuiLabel((Rectangle){ 10, 10, 100, 20 }, "Timeline:");
        // frameCount = GuiSlider((Rectangle){ 10, 40, 200, 20 }, "Frames", TextFormat("%02i", frameCount), frameCount, 1, MAX_FRAMES);
        // currentFrame = GuiSlider((Rectangle){ 10, 70, 200, 20 }, "Current Frame", TextFormat("%02i", currentFrame), currentFrame, 0, frameCount - 1);

        for (int i = 0; i < elementCount; i++) {
            int posY = 100 + elements[i].order * 30;
            GuiLabel((Rectangle){ 130, posY, 200, 20 }, TextFormat("Element %i: %.02f", i, elements[i].position));
            // elements[i].position = GuiSlider((Rectangle){ 340, posY, 200, 20 }, "", TextFormat("%.02f", elements[i].position), elements[i].position, 0.0f, (float)frameCount);
            DrawRectangle(550, posY, 200, 20, elements[i].color);
        }

        // Debug Information
        DrawText("Debug Information", 10, 500, 20, DARKGRAY);
        DrawText(TextFormat("Frame Count: %i", frameCount), 10, 530, 20, DARKGRAY);
        DrawText(TextFormat("Current Frame: %i", currentFrame), 10, 560, 20, DARKGRAY);

        for (int i = 0; i < elementCount; i++) {
            DrawText(TextFormat("Element %i: Position %.02f, Order %i", i, elements[i].position, elements[i].order), 10, 590 + i * 20, 20, elements[i].color);
        }

        EndDrawing();
    }

    // De-Initialization
    CloseWindow();

    return 0;
}
