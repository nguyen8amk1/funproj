#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define MAX_RECORDS 1000

typedef struct {
    float x, y, t;
} PositionRecord;

typedef enum {
    VIEW_MAIN,
    VIEW_SECONDARY
} View;

int main() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Drag the Circle with Recording and Playback Controls");
    SetTargetFPS(60);
    
    View currentView = VIEW_MAIN;
    
    Vector2 circlePos = {SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2};
    float circleRadius = 50.0f;
    bool isDragging = false;
    bool isPlaying = false;
    bool isPaused = false;
    bool isScrubbing = false;
    
    PositionRecord records[MAX_RECORDS];
    int recordIndex = 0;
    float startTime = 0;
    int playbackIndex = 0;
    
    Rectangle playButton = {10, SCREEN_HEIGHT - 50, 80, 30};
    Rectangle pauseButton = {100, SCREEN_HEIGHT - 50, 80, 30};
    Rectangle rewindButton = {190, SCREEN_HEIGHT - 50, 80, 30};
    Rectangle timeline = {10, SCREEN_HEIGHT - 20, SCREEN_WIDTH - 20, 5};
    Rectangle switchViewButton = {SCREEN_WIDTH - 100, 10, 80, 30};
    
    float zoom = 1.0f;
    Vector2 pan = {0, 0};
    bool isPanning = false;
    Vector2 lastMousePos = {0, 0};
    
    while (!WindowShouldClose()) {
        Vector2 mousePos = GetMousePosition();
        float currentTime = GetTime();
        
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(mousePos, switchViewButton)) {
            currentView = (currentView == VIEW_MAIN) ? VIEW_SECONDARY : VIEW_MAIN;
        }
        
        if (currentView == VIEW_MAIN) {
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                float dist = (mousePos.x - circlePos.x) * (mousePos.x - circlePos.x) + 
                             (mousePos.y - circlePos.y) * (mousePos.y - circlePos.y);
                if (dist <= circleRadius * circleRadius) {
                    isDragging = true;
                    recordIndex = 0;
                    startTime = currentTime;
                }
                if (CheckCollisionPointRec(mousePos, timeline)) {
                    isScrubbing = true;
                }
            }
            
            if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
                isDragging = false;
                isScrubbing = false;
            }
            
            if (isDragging && recordIndex < MAX_RECORDS) {
                records[recordIndex].x = mousePos.x;
                records[recordIndex].y = mousePos.y;
                records[recordIndex].t = currentTime - startTime;
                recordIndex++;
            }
            
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                if (CheckCollisionPointRec(mousePos, playButton)) {
                    isPlaying = true;
                    isPaused = false;
                } else if (CheckCollisionPointRec(mousePos, pauseButton)) {
                    isPaused = !isPaused;
                } else if (CheckCollisionPointRec(mousePos, rewindButton)) {
                    playbackIndex = 0;
                }
            }
            
            if (isScrubbing && recordIndex > 0) {
                playbackIndex = ((mousePos.x - timeline.x) / timeline.width) * recordIndex;
                if (playbackIndex < 0) playbackIndex = 0;
                if (playbackIndex >= recordIndex) playbackIndex = recordIndex - 1;
                circlePos.x = records[playbackIndex].x;
                circlePos.y = records[playbackIndex].y;
            }
            
            if (isPlaying && !isPaused && playbackIndex < recordIndex) {
                circlePos.x = records[playbackIndex].x;
                circlePos.y = records[playbackIndex].y;
                playbackIndex++;
                if (playbackIndex >= recordIndex) {
                    isPlaying = false;
                }
            }
            
            if (isDragging) {
                circlePos = mousePos;
            }
        } else {
            zoom += GetMouseWheelMove() * 0.1f;
            if (zoom < 0.1f) zoom = 0.1f;
            
            if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
                isPanning = true;
                lastMousePos = mousePos;
            }
            
            if (IsMouseButtonReleased(MOUSE_RIGHT_BUTTON)) {
                isPanning = false;
            }
            
            if (isPanning) {
                Vector2 delta = {mousePos.x - lastMousePos.x, mousePos.y - lastMousePos.y};
                pan.x += delta.x;
                pan.y += delta.y;
                lastMousePos = mousePos;
            }
        }
        
        BeginDrawing();
        ClearBackground(RAYWHITE);
        
        if (currentView == VIEW_MAIN) {
            for (int i = 1; i < recordIndex; i++) {
                float intensity = records[i].t / records[recordIndex - 1].t;
                Color pathColor = (Color){0, 0, 255, (unsigned char)(intensity * 255)};
                DrawLineV((Vector2){records[i - 1].x, records[i - 1].y}, (Vector2){records[i].x, records[i].y}, pathColor);
            }
            
            DrawCircleV(circlePos, circleRadius, isDragging ? DARKGRAY : BLUE);
            DrawRectangleRec(playButton, GREEN);
            DrawRectangleRec(pauseButton, ORANGE);
            DrawRectangleRec(rewindButton, RED);
            DrawText("Play", playButton.x + 20, playButton.y + 7, 20, WHITE);
            DrawText("Pause", pauseButton.x + 15, pauseButton.y + 7, 20, WHITE);
            DrawText("Rewind", rewindButton.x + 10, rewindButton.y + 7, 20, WHITE);
            DrawRectangleRec(timeline, LIGHTGRAY);
        } else {
            DrawText("Graphing Mode", 10, 10, 20, DARKGRAY);
            for (int i = 1; i < recordIndex; i++) {
                DrawLine(records[i - 1].t * 100 * zoom + pan.x, SCREEN_HEIGHT - records[i - 1].y * zoom + pan.y,
                         records[i].t * 100 * zoom + pan.x, SCREEN_HEIGHT - records[i].y * zoom + pan.y, BLUE);
            }
        }
        DrawRectangleRec(switchViewButton, BLUE);
        DrawText("Switch", switchViewButton.x + 10, switchViewButton.y + 7, 20, WHITE);
        EndDrawing();
    }
    
    CloseWindow();
    return 0;
}
