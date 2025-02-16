#include "raylib.h"
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

typedef struct Action {
    void (*apply)(void* data);
    void (*revert)(void* data);
    void* data;
} Action;

typedef struct UndoRedoStack {
    Action* actions;
    size_t capacity;
    size_t size;
    size_t index;
} UndoRedoStack;

UndoRedoStack* create_undo_redo_stack(size_t capacity) {
    UndoRedoStack* stack = (UndoRedoStack*)malloc(sizeof(UndoRedoStack));
    stack->actions = (Action*)malloc(sizeof(Action) * capacity);
    stack->capacity = capacity;
    stack->size = 0;
    stack->index = 0;
    return stack;
}

void destroy_undo_redo_stack(UndoRedoStack* stack) {
    free(stack->actions);
    free(stack);
}

bool push_action(UndoRedoStack* stack, Action action) {
    if (stack->index < stack->size) {
        stack->size = stack->index;
    }
    if (stack->size == stack->capacity) {
        memmove(stack->actions, stack->actions + 1, sizeof(Action) * (stack->capacity - 1));
        stack->size--;
    }
    stack->actions[stack->size++] = action;
    stack->index++;
    return true;
}

bool undo(UndoRedoStack* stack) {
    if (stack->index == 0) {
        return false;
    }
    stack->actions[--stack->index].revert(stack->actions[stack->index].data);
    return true;
}

bool redo(UndoRedoStack* stack) {
    if (stack->index == stack->size) {
        return false;
    }
    stack->actions[stack->index++].apply(stack->actions[stack->index - 1].data);
    return true;
}

typedef struct {
    Vector2 position;
    Color color;
} Circle;

typedef struct {
    Circle* circle;
    Vector2 oldPosition;
    Vector2 newPosition;
} MoveActionData;

void ApplyMove(void* data) {
    MoveActionData* moveData = (MoveActionData*)data;
    moveData->circle->position = moveData->newPosition;
}

void RevertMove(void* data) {
    MoveActionData* moveData = (MoveActionData*)data;
    moveData->circle->position = moveData->oldPosition;
}

int main(void) {
    const int screenWidth = 800;
    const int screenHeight = 600;

    InitWindow(screenWidth, screenHeight, "Undo Redo Demo with Raylib");

    Circle circle = { { screenWidth / 2, screenHeight / 2 }, RED };
    UndoRedoStack* undoRedoStack = create_undo_redo_stack(10);

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_Z) && (IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL))) {
            undo(undoRedoStack);
        }
        if (IsKeyPressed(KEY_Y) && (IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL))) {
            redo(undoRedoStack);
        }
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            Vector2 mousePosition = GetMousePosition();
            MoveActionData* moveData = (MoveActionData*)malloc(sizeof(MoveActionData));
            moveData->circle = &circle;
            moveData->oldPosition = circle.position;
            moveData->newPosition = mousePosition;

            Action action = { ApplyMove, RevertMove, moveData };
            push_action(undoRedoStack, action);
            ApplyMove(moveData);
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawCircleV(circle.position, 50, circle.color);
        DrawText("Click to move the circle", 10, 10, 20, DARKGRAY);
        DrawText("Press CTRL+Z to undo, CTRL+Y to redo", 10, 40, 20, DARKGRAY);
        EndDrawing();
    }

    destroy_undo_redo_stack(undoRedoStack);
    CloseWindow();

    return 0;
}
