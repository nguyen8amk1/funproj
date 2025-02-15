#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define MAX_RECORDS 10000

typedef struct {
    float x, y, t;
} PositionRecord;

typedef enum {
    VIEW_MAIN,
    VIEW_SECONDARY
} View;

PositionRecord records[MAX_RECORDS];
View currentView = VIEW_MAIN;
const Rectangle switchViewButton = {SCREEN_WIDTH - 100, 10, 80, 30};

typedef struct {
    Vector2 mousePos;
    float currentTime;
} UIState;

typedef struct {
    int recordIndex;
    float startTime;
    int playbackIndex;
    bool isDragging;
    bool isPlaying;
    bool isPaused;
    bool isScrubbing;

    // draw 
    Vector2 circlePos;
    float circleRadius;
    Rectangle playButton;
    Rectangle pauseButton;
    Rectangle rewindButton;
    Rectangle timeline;
} FirstViewState;

typedef struct {
    float zoom;
    Vector2 pan;
    bool isPanning;
    Vector2 lastMousePos;
} SecondViewState;

UIState ui_state = {}; 

void update_first_view_state(FirstViewState *first_view_state) {
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        float dist = (ui_state.mousePos.x - first_view_state->circlePos.x) * (ui_state.mousePos.x - first_view_state->circlePos.x) + 
            (ui_state.mousePos.y - first_view_state->circlePos.y) * (ui_state.mousePos.y - first_view_state->circlePos.y);
        if (dist <= first_view_state->circleRadius * first_view_state->circleRadius) {
            first_view_state->isDragging = true;
            first_view_state->recordIndex = 0;
            first_view_state->startTime = ui_state.currentTime;
        }
        if (CheckCollisionPointRec(ui_state.mousePos, first_view_state->timeline)) {
            first_view_state->isScrubbing = true;
        }
    }

    if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
        first_view_state->isDragging = false;
        first_view_state->isScrubbing = false;
    }

    if (first_view_state->isDragging && first_view_state->recordIndex < MAX_RECORDS) {
        records[first_view_state->recordIndex].x = ui_state.mousePos.x;
        records[first_view_state->recordIndex].y = ui_state.mousePos.y;
        records[first_view_state->recordIndex].t = ui_state.currentTime - first_view_state->startTime;
        first_view_state->recordIndex++;
    }

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (CheckCollisionPointRec(ui_state.mousePos, first_view_state->playButton)) {
            first_view_state->isPlaying = true;
            first_view_state->isPaused = false;
        } else if (CheckCollisionPointRec(ui_state.mousePos, first_view_state->pauseButton)) {
            first_view_state->isPaused = !first_view_state->isPaused;
        } else if (CheckCollisionPointRec(ui_state.mousePos, first_view_state->rewindButton)) {
            first_view_state->playbackIndex = 0;
        }
    }
    
    if (first_view_state->isScrubbing && first_view_state->recordIndex > 0) {
        first_view_state->playbackIndex = ((ui_state.mousePos.x - first_view_state->timeline.x) / first_view_state->timeline.width) * first_view_state->recordIndex;
        if (first_view_state->playbackIndex < 0) first_view_state->playbackIndex = 0;
        if (first_view_state->playbackIndex >= first_view_state->recordIndex) first_view_state->playbackIndex = first_view_state->recordIndex - 1;
        first_view_state->circlePos.x = records[first_view_state->playbackIndex].x;
        first_view_state->circlePos.y = records[first_view_state->playbackIndex].y;
    }

    if (first_view_state->isPlaying && !first_view_state->isPaused && first_view_state->playbackIndex < first_view_state->recordIndex) {
        first_view_state->circlePos.x = records[first_view_state->playbackIndex].x;
        first_view_state->circlePos.y = records[first_view_state->playbackIndex].y;
        first_view_state->playbackIndex++;
        if (first_view_state->playbackIndex >= first_view_state->recordIndex) {
            first_view_state->isPlaying = false;
        }
    }

    if (first_view_state->isDragging) {
        first_view_state->circlePos = ui_state.mousePos;
    }
}

void draw_first_view(const FirstViewState *first_view_state) {
    for (int i = 1; i < first_view_state->recordIndex; i++) {
        float intensity = records[i].t / records[first_view_state->recordIndex - 1].t;
        Color pathColor = (Color){0, 0, 255, (unsigned char)(intensity * 255)};
        DrawLineV((Vector2){records[i - 1].x, records[i - 1].y}, (Vector2){records[i].x, records[i].y}, pathColor);
    }
    DrawCircleV(first_view_state->circlePos, first_view_state->circleRadius, first_view_state->isDragging ? DARKGRAY : BLUE);
    DrawRectangleRec(first_view_state->playButton, GREEN);
    DrawRectangleRec(first_view_state->pauseButton, ORANGE);
    DrawRectangleRec(first_view_state->rewindButton, RED);
    DrawText("Play", first_view_state->playButton.x + 20, first_view_state->playButton.y + 7, 20, WHITE);
    DrawText("Pause", first_view_state->pauseButton.x + 15, first_view_state->pauseButton.y + 7, 20, WHITE);
    DrawText("Rewind", first_view_state->rewindButton.x + 10, first_view_state->rewindButton.y + 7, 20, WHITE);
    DrawRectangleRec(first_view_state->timeline, LIGHTGRAY);
    if (first_view_state->recordIndex > 0) {
        float scrubberX = first_view_state->timeline.x + (first_view_state->timeline.width * first_view_state->playbackIndex / first_view_state->recordIndex);
        DrawRectangle(scrubberX, first_view_state->timeline.y - 5, 10, 20, RED);
    }
}


// void update_second_view_state(SecondViewState *second_view_state) {
//     zoom += GetMouseWheelMove() * 0.1f;
//     if (zoom < 0.1f) zoom = 0.1f;
//
//     if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
//         isPanning = true;
//         lastMousePos = mousePos;
//     }
//
//     if (IsMouseButtonReleased(MOUSE_RIGHT_BUTTON)) {
//         isPanning = false;
//     }
//
//     if (isPanning) {
//         Vector2 delta = {mousePos.x - lastMousePos.x, mousePos.y - lastMousePos.y};
//         pan.x += delta.x;
//         pan.y += delta.y;
//         lastMousePos = mousePos;
//     }
// }
//

//
// void draw_second_view(const SecondViewState *second_view_state) {
//     for (int x = -SCREEN_WIDTH; x < SCREEN_WIDTH; x += 50) {
//         DrawLine(x * zoom + pan.x, 0, x * zoom + pan.x, SCREEN_HEIGHT, LIGHTGRAY);
//     }
//     for (int y = -SCREEN_HEIGHT; y < SCREEN_HEIGHT; y += 50) {
//         DrawLine(0, y * zoom + pan.y, SCREEN_WIDTH, y * zoom + pan.y, LIGHTGRAY);
//     }
//     DrawLine(SCREEN_WIDTH / 2 + pan.x, 0, SCREEN_WIDTH / 2 + pan.x, SCREEN_HEIGHT, DARKGRAY);
//     DrawLine(0, SCREEN_HEIGHT / 2 + pan.y, SCREEN_WIDTH, SCREEN_HEIGHT / 2 + pan.y, DARKGRAY);
//     DrawText("X", SCREEN_WIDTH - 20 + pan.x, SCREEN_HEIGHT / 2 + pan.y, 20, DARKGRAY);
//     DrawText("Y", SCREEN_WIDTH / 2 + pan.x, 20 + pan.y, 20, DARKGRAY);
//
//     DrawText("Graphing Mode", 10, 10, 20, DARKGRAY);
//     for (int i = 1; i < recordIndex; i++) {
//         DrawLine(records[i - 1].t * 100 * zoom + pan.x, SCREEN_HEIGHT - records[i - 1].y * zoom + pan.y,
//                     records[i].t * 100 * zoom + pan.x, SCREEN_HEIGHT - records[i].y * zoom + pan.y, BLUE);
//     }
// }

int main() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Drag the Circle with Recording and Playback Controls");
    SetTargetFPS(60);
    FirstViewState first_view_state = {
        .circlePos = {SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2}, 
        .circleRadius = 50.0f, 

        .playButton = {10, SCREEN_HEIGHT - 50, 80, 30}, 
        .pauseButton = {100, SCREEN_HEIGHT - 50, 80, 30}, 
        .rewindButton = {190, SCREEN_HEIGHT - 50, 80, 30}, 
        .timeline = {10, SCREEN_HEIGHT - 20, SCREEN_WIDTH - 20, 5}
    }; 
    SecondViewState second_view_state = {}; 

    while (!WindowShouldClose()) {
        ui_state.mousePos = GetMousePosition();
        ui_state.currentTime = GetTime();

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(ui_state.mousePos, switchViewButton)) {
            currentView = (currentView == VIEW_MAIN) ? VIEW_SECONDARY : VIEW_MAIN;
        }
        if (currentView == VIEW_MAIN) {
            update_first_view_state(&first_view_state);
        } 
        // if(currentView == VIEW_SECONDARY){
        //     update_second_view_state(&second_view_state);
        // }
        
        BeginDrawing();
        ClearBackground(RAYWHITE);

        if (currentView == VIEW_MAIN) {
            draw_first_view(&first_view_state);
        } 
        // if (currentView == VIEW_SECONDARY){
        //     draw_second_view(&second_view_state);
        // }
        
        DrawRectangleRec(switchViewButton, BLUE);
        DrawText("Switch", switchViewButton.x + 10, switchViewButton.y + 7, 20, WHITE);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
