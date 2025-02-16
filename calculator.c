#define RAYGUI_IMPLEMENTATION
#include "raylib.h"
#include "raygui.h"
#include <stdlib.h>
#include <time.h>

// Define the maximum number of numbers that can be spawned
#define MAX_NUMBERS 100

typedef struct {
    Vector2 position;
    int value;
    bool isDragging;
    bool isSelected;
} Number;

int main(void) {
    // Initialization
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "raylib - spawn and drag numbers with operations using raygui");

    // Initialize variables
    Number numbers[MAX_NUMBERS];
    int numbersCount = 0;
    Number *selectedNumbers[2] = { NULL, NULL };  // To store the references to the selected numbers

    // Seed the random number generator
    srand((unsigned int)time(NULL));

    SetTargetFPS(60);  // Set our game to run at 60 frames-per-second

    // Main game loop
    while (!WindowShouldClose()) {
        // Update
        Vector2 mousePosition = GetMousePosition();

        // Check if spawn button is clicked
        if (GuiButton((Rectangle){ 10, 10, 80, 30 }, "Spawn")) {
            if (numbersCount < MAX_NUMBERS) {
                numbers[numbersCount].position = (Vector2){ 150, 200 };
                numbers[numbersCount].value = rand() % 100;  // Random number between 0 and 99
                numbers[numbersCount].isDragging = false;
                numbers[numbersCount].isSelected = false;
                numbersCount++;
            }
        }

        // Check if any number is being dragged
        for (int i = 0; i < numbersCount; i++) {
            if (numbers[i].isDragging) {
                numbers[i].position = mousePosition;
                if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
                    numbers[i].isDragging = false;
                }
            } else if (CheckCollisionPointRec(mousePosition, (Rectangle){ numbers[i].position.x - 20, numbers[i].position.y - 20, 40, 40 }) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                numbers[i].isDragging = true;
            }
        }

        // Check if any number is selected
        for (int i = 0; i < numbersCount; i++) {
            if (CheckCollisionPointRec(mousePosition, (Rectangle){ numbers[i].position.x - 20, numbers[i].position.y - 20, 40, 40 }) && IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
                numbers[i].isSelected = !numbers[i].isSelected;
                if (numbers[i].isSelected) {
                    if (!selectedNumbers[0]) {
                        selectedNumbers[0] = &numbers[i];
                    } else if (!selectedNumbers[1]) {
                        selectedNumbers[1] = &numbers[i];
                    }
                } else {
                    if (selectedNumbers[0] == &numbers[i]) {
                        selectedNumbers[0] = NULL;
                    } else if (selectedNumbers[1] == &numbers[i]) {
                        selectedNumbers[1] = NULL;
                    }
                }
            }
        }

        // Check if any operation button is clicked
        if (selectedNumbers[0] && selectedNumbers[1]) {
            int result = 0;
            if (GuiButton((Rectangle){ 10, 50, 80, 30 }, "+")) {
                // Addition
                result = selectedNumbers[0]->value + selectedNumbers[1]->value;
            } else if (GuiButton((Rectangle){ 10, 90, 80, 30 }, "-")) {
                // Subtraction
                result = selectedNumbers[0]->value - selectedNumbers[1]->value;
            } else if (GuiButton((Rectangle){ 10, 130, 80, 30 }, "*")) {
                // Multiplication
                result = selectedNumbers[0]->value * selectedNumbers[1]->value;
            } else if (GuiButton((Rectangle){ 10, 170, 80, 30 }, "/") && selectedNumbers[1]->value != 0) {
                // Division (avoid division by zero)
                result = selectedNumbers[0]->value / selectedNumbers[1]->value;
            }

            if (result != 0 && numbersCount < MAX_NUMBERS) {
                numbers[numbersCount].position = (Vector2){ 150, 200 };
                numbers[numbersCount].value = result;
                numbers[numbersCount].isDragging = false;
                numbers[numbersCount].isSelected = false;
                numbersCount++;
                // Deselect the numbers after the operation
                selectedNumbers[0]->isSelected = false;
                selectedNumbers[1]->isSelected = false;
                selectedNumbers[0] = NULL;
                selectedNumbers[1] = NULL;
            }
        }

        // Draw
        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Draw numbers
        for (int i = 0; i < numbersCount; i++) {
            DrawText(TextFormat("%d", numbers[i].value), (int)numbers[i].position.x, (int)numbers[i].position.y, 20, numbers[i].isSelected ? RED : BLACK);
        }

        EndDrawing();
    }

    // De-Initialization
    CloseWindow();  // Close window and OpenGL context

    return 0;
}
