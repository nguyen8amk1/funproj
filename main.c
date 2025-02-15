#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

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
int recordIndex = 0;
View currentView = VIEW_MAIN;
const Rectangle switchViewButton = {SCREEN_WIDTH - 100, 10, 80, 30};

typedef struct {
    Vector2 mousePos;
    float currentTime;
} UIState;

typedef struct {
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
    Vector2 offset;
} SecondViewState;

UIState ui_state = {}; 

void update_first_view_state(FirstViewState *first_view_state) {
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        float dist = (ui_state.mousePos.x - first_view_state->circlePos.x) * (ui_state.mousePos.x - first_view_state->circlePos.x) + 
            (ui_state.mousePos.y - first_view_state->circlePos.y) * (ui_state.mousePos.y - first_view_state->circlePos.y);
        if (dist <= first_view_state->circleRadius * first_view_state->circleRadius) {
            first_view_state->isDragging = true;
            recordIndex = 0;
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

    if (first_view_state->isDragging && recordIndex < MAX_RECORDS) {
        records[recordIndex].x = ui_state.mousePos.x;
        records[recordIndex].y = ui_state.mousePos.y;
        records[recordIndex].t = ui_state.currentTime - first_view_state->startTime;
        recordIndex++;
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
    
    if (first_view_state->isScrubbing && recordIndex > 0) {
        first_view_state->playbackIndex = ((ui_state.mousePos.x - first_view_state->timeline.x) / first_view_state->timeline.width) * recordIndex;
        if (first_view_state->playbackIndex < 0) first_view_state->playbackIndex = 0;
        if (first_view_state->playbackIndex >= recordIndex) first_view_state->playbackIndex = recordIndex - 1;
        first_view_state->circlePos.x = records[first_view_state->playbackIndex].x;
        first_view_state->circlePos.y = records[first_view_state->playbackIndex].y;
    }

    if (first_view_state->isPlaying && !first_view_state->isPaused && first_view_state->playbackIndex < recordIndex) {
        first_view_state->circlePos.x = records[first_view_state->playbackIndex].x;
        first_view_state->circlePos.y = records[first_view_state->playbackIndex].y;
        first_view_state->playbackIndex++;
        if (first_view_state->playbackIndex >= recordIndex) {
            first_view_state->isPlaying = false;
        }
    }

    if (first_view_state->isDragging) {
        first_view_state->circlePos = ui_state.mousePos;
    }
}

void draw_first_view(const FirstViewState *first_view_state) {
    for (int i = 1; i < recordIndex; i++) {
        float intensity = records[i].t / records[recordIndex - 1].t;
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
    if (recordIndex > 0) {
        float scrubberX = first_view_state->timeline.x + (first_view_state->timeline.width * first_view_state->playbackIndex / recordIndex);
        DrawRectangle(scrubberX, first_view_state->timeline.y - 5, 10, 20, RED);
    }
}


void update_second_view_state(SecondViewState *second_view_state) {
    // Handle zoom
    const float zoomFactor = GetMouseWheelMove();
    if (zoomFactor != 0) {
        const float scaleFactor = 1.1f;
        const float prevZoom = second_view_state->zoom;
        second_view_state->zoom *= (zoomFactor > 0) ? scaleFactor : (1 / scaleFactor);

        second_view_state->offset.x -= (ui_state.mousePos.x - second_view_state->offset.x) * (second_view_state->zoom - prevZoom) / prevZoom;
        second_view_state->offset.y -= (ui_state.mousePos.y - second_view_state->offset.y) * (second_view_state->zoom - prevZoom) / prevZoom;
    }

    // Handle panning
    if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
        const Vector2 delta = GetMouseDelta();
        second_view_state->offset.x += delta.x;
        second_view_state->offset.y += delta.y;
    }
}



void draw_second_view(const SecondViewState *second_view_state) {
    // Draw grid
    const int numLines = 100;
    const float step = second_view_state->zoom / 10;
    const Color gridColor = LIGHTGRAY;

    for (int i = -numLines; i <= numLines; i++) {
        float x = second_view_state->offset.x + i * step;
        float y = second_view_state->offset.y + i * step;
        DrawLine(x, 0, x, SCREEN_HEIGHT, gridColor);
        DrawLine(0, y, SCREEN_WIDTH, y, gridColor);
    }

    // Draw axes
    DrawLine(0, second_view_state->offset.y, SCREEN_WIDTH, second_view_state->offset.y, BLACK);
    DrawLine(second_view_state->offset.x, 0, second_view_state->offset.x, SCREEN_HEIGHT, BLACK);

    // Plot y = sin(x)

    // for (float sx = 0; sx < SCREEN_WIDTH; sx += precision) {
    //     // Convert screen x to graph x
    //     float x1 = (sx - second_view_state->offset.x) / second_view_state->zoom;
    //     float x2 = ((sx + precision) - second_view_state->offset.x) / second_view_state->zoom;
    //
    //     // Compute function values
    //     float y1 = (x1);
    //     float y2 = (x2);
    //
    //     // Convert graph y to screen y
    //     float sy1 = second_view_state->offset.y - y1 * second_view_state->zoom;
    //     float sy2 = second_view_state->offset.y - y2 * second_view_state->zoom;
    //
    //     // Draw function segment
    //     DrawLine(sx, sy1, sx + precision, sy2, functionColor);
    // }

    const Color functionColor = RED;
    DrawText("Graphing Mode", 10, 10, 20, DARKGRAY);
    for (int i = 1; i < recordIndex; i++) {
        const float t1 = records[i - 1].t;
        const float t2 = records[i].t;
        const float x1 = (t1 - second_view_state->offset.x)/second_view_state->zoom;
        const float x2 = (t2 - second_view_state->offset.x)/second_view_state->zoom;

        const float y1 = x1; 
        const float y2 = x2; 

        const float sy1 = second_view_state->offset.y - y1 * second_view_state->zoom;
        const float sy2 = second_view_state->offset.y - y2 * second_view_state->zoom;
        DrawLine(x1, sy1, x2, sy2, functionColor);
    }
}

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
    SecondViewState second_view_state = {
        .offset = { SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 }, // Center of screen
        .zoom = 50.0f // Pixels per unit
    }; 

    while (!WindowShouldClose()) {
        ui_state.mousePos = GetMousePosition();
        ui_state.currentTime = GetTime();

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(ui_state.mousePos, switchViewButton)) {
            currentView = (currentView == VIEW_MAIN) ? VIEW_SECONDARY : VIEW_MAIN;
        }

        if (currentView == VIEW_MAIN) {
            update_first_view_state(&first_view_state);
        } else if(currentView == VIEW_SECONDARY){
            update_second_view_state(&second_view_state);
        }
        
        BeginDrawing();
        ClearBackground(RAYWHITE);

        if (currentView == VIEW_MAIN) {
            draw_first_view(&first_view_state);
        } 
        else if (currentView == VIEW_SECONDARY){
            draw_second_view(&second_view_state);
        }
        
        DrawRectangleRec(switchViewButton, BLUE);
        DrawText("Switch", switchViewButton.x + 10, switchViewButton.y + 7, 20, WHITE);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
