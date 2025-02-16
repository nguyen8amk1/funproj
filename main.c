#define RAYGUI_IMPLEMENTATION
#include "raylib.h"
#include "raygui.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define SCREEN_WIDTH 1366
#define SCREEN_HEIGHT 768
#define MAX_RECORDS 10000
#define COLOR_TO_INT(c) ((c.r << 16) | (c.g << 8) | (c.b))

/* TODO: (nttn), current app idea: geometric contraints editor and position record + average 
 * */ 
typedef struct {
    float x, y, t;
} PositionRecord;

typedef enum {
    VIEW_MAIN,
    VIEW_SECONDARY
} View;

typedef struct {
    Vector2 pos; 
    float rad;
    PositionRecord records[MAX_RECORDS]; 
} PositionRecordedObject;

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
    Rectangle playButton;
    Rectangle pauseButton;
    Rectangle rewindButton;
    Rectangle dumpButton; // New button for dumping data
    Rectangle timeline;
} FirstViewState;

typedef struct {
    float zoom;
    Vector2 offset;
} SecondViewState;

UIState ui_state = {}; 
PositionRecordedObject *ptrCurrentCircle;
PositionRecordedObject circles[10];
PositionRecordedObject circle = {
    .pos = {SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2}, 
    .rad = 50.0f,
    .records = {0}
};

// TODO: (trantr) add more circles 
int recordIndex = 0;
View currentView = VIEW_MAIN;
const Rectangle switchViewButton = {SCREEN_WIDTH - 100, 10, 80, 30};

void dump_recorded_data_to_file(const char* filename) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        printf("Failed to open file for writing\n");
        return;
    }
    for (int i = 0; i < recordIndex; i++) {
        fprintf(file, "x = %f, y = %f, t = %f\n", i, circle.records[i].x, circle.records[i].y, circle.records[i].t);
    }
    fclose(file);
    printf("Data successfully dumped to %s\n", filename);
}

void update_first_view_state(FirstViewState *first_view_state) {
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        float dist = (ui_state.mousePos.x - circle.pos.x) * (ui_state.mousePos.x - circle.pos.x) + 
            (ui_state.mousePos.y - circle.pos.y) * (ui_state.mousePos.y - circle.pos.y);
        if (dist <= circle.rad * circle.rad) {
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
        for(int i = 0; i < recordIndex; i++) {
            printf("%f, %f\n", circle.records[i].t, circle.records[i].y); 
        }
    }

    if (first_view_state->isDragging && recordIndex < MAX_RECORDS) {
        circle.records[recordIndex].x = ui_state.mousePos.x;
        circle.records[recordIndex].y = ui_state.mousePos.y;
        circle.records[recordIndex].t = ui_state.currentTime - first_view_state->startTime;
        recordIndex++;
    }

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (CheckCollisionPointRec(ui_state.mousePos, first_view_state->pauseButton)) {
            first_view_state->isPaused = !first_view_state->isPaused;
        } else if (CheckCollisionPointRec(ui_state.mousePos, first_view_state->rewindButton)) {
            first_view_state->playbackIndex = 0;
        } else if (CheckCollisionPointRec(ui_state.mousePos, first_view_state->dumpButton)) { // Check for dump button press
            dump_recorded_data_to_file("recorded_data.txt");
        }
    }
    
    if (first_view_state->isScrubbing && recordIndex > 0) {
        first_view_state->playbackIndex = ((ui_state.mousePos.x - first_view_state->timeline.x) / first_view_state->timeline.width) * recordIndex;
        if (first_view_state->playbackIndex < 0) first_view_state->playbackIndex = 0;
        if (first_view_state->playbackIndex >= recordIndex) first_view_state->playbackIndex = recordIndex - 1;
        circle.pos.x = circle.records[first_view_state->playbackIndex].x;
        circle.pos.y = circle.records[first_view_state->playbackIndex].y;
    }

    if (first_view_state->isPlaying && !first_view_state->isPaused && first_view_state->playbackIndex < recordIndex) {
        circle.pos.x = circle.records[first_view_state->playbackIndex].x;
        circle.pos.y = circle.records[first_view_state->playbackIndex].y;
        first_view_state->playbackIndex++;
        if (first_view_state->playbackIndex >= recordIndex) {
            first_view_state->isPlaying = false;
        }
    }

    if (first_view_state->isDragging) {
        circle.pos = ui_state.mousePos;
    }
}

void draw_first_view(FirstViewState *first_view_state) {
    for (int i = 1; i < recordIndex; i++) {
        float intensity = circle.records[i].t / circle.records[recordIndex - 1].t;
        Color pathColor = (Color){0, 0, 255, (unsigned char)(intensity * 255)};
        DrawLineV((Vector2){circle.records[i - 1].x, circle.records[i - 1].y}, (Vector2){circle.records[i].x, circle.records[i].y}, pathColor);
    }
    DrawCircleV(circle.pos, circle.rad, first_view_state->isDragging ? DARKGRAY : BLUE);

    GuiSetStyle(BUTTON, BASE_COLOR_NORMAL, COLOR_TO_INT(GREEN));
    GuiSetStyle(BUTTON, TEXT_COLOR_NORMAL, COLOR_TO_INT(WHITE));
    if(GuiButton(first_view_state->playButton, "Play")) {
        first_view_state->isPlaying = true;
        first_view_state->isPaused = false;
    }

    DrawRectangleRec(first_view_state->pauseButton, ORANGE);
    DrawRectangleRec(first_view_state->rewindButton, RED);
    DrawText("Pause", first_view_state->pauseButton.x + 15, first_view_state->pauseButton.y + 7, 20, WHITE);
    DrawText("Rewind", first_view_state->rewindButton.x + 10, first_view_state->rewindButton.y + 7, 20, WHITE);
    DrawRectangleRec(first_view_state->timeline, LIGHTGRAY);
    if (recordIndex > 0) {
        float scrubberX = first_view_state->timeline.x + (first_view_state->timeline.width * first_view_state->playbackIndex / recordIndex);
        DrawRectangle(scrubberX, first_view_state->timeline.y - 5, 10, 20, RED);
    }

    GuiSetStyle(BUTTON, BASE_COLOR_NORMAL, COLOR_TO_INT(ORANGE));
    GuiSetStyle(BUTTON, TEXT_COLOR_NORMAL, COLOR_TO_INT(WHITE));
    if(GuiButton(first_view_state->dumpButton, "Dump Data")) {
        dump_recorded_data_to_file("recorded_data.txt");
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
    if (IsMouseButtonDown(MOUSE_RIGHT_BUTTON)) {
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

    const Color functionColor = RED;
    DrawText("Graphing Mode", 10, 10, 20, DARKGRAY);

    // FIXME: (nttn) fix this, i want to render the graph 
    for (int i = 1; i < recordIndex; i++) {
        // const float t1 = circle.records[i - 1].t;
        // const float t2 = circle.records[i].t;
        // const float x1 = (t1 - second_view_state->offset.x)/second_view_state->zoom;
        // const float x2 = (t2 - second_view_state->offset.x)/second_view_state->zoom;
        //
        // const float y1 = x1; 
        // const float y2 = x2; 
        //
        // const float sy1 = second_view_state->offset.y - y1 * second_view_state->zoom;
        // const float sy2 = second_view_state->offset.y - y2 * second_view_state->zoom;
        // printf("render data: x1: %f, y1: %f\n",  x1, sy1);
        // printf("record data: x1: %f, y1: %f, t1: %f\n",  circle.records[i-1].x, circle.records[i-1].y, circle.records[i-1].t);
        //DrawLine(x1, sy1, x2, sy2, functionColor);
        
        // Screen space rendering
        const float t1 = circle.records[i - 1].t;
        const float t2 = circle.records[i].t;
        const float y1 = circle.records[i - 1].y;
        const float y2 = circle.records[i].y;
        DrawLine(t1*100, y1, t2*100, y2, functionColor);
    }
}

int* split_cycles_from_position_record_data(const PositionRecord *data) {
    // TODO: (nttn) 
}

int main() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Drag the Circle with Recording and Playback Controls");
    SetTargetFPS(60);
    FirstViewState first_view_state = {
        .playButton = {10, SCREEN_HEIGHT - 50, 80, 30}, 
        .pauseButton = {100, SCREEN_HEIGHT - 50, 80, 30}, 
        .rewindButton = {190, SCREEN_HEIGHT - 50, 80, 30}, 
        .dumpButton = {280, SCREEN_HEIGHT - 50, 110, 30}, // New button for dumping data
        .timeline = {10, SCREEN_HEIGHT - 20, SCREEN_WIDTH - 20, 5}
    };

    SecondViewState second_view_state = {
        .offset = { SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 }, // Center of screen
        .zoom = 50.0f // Pixels per unit
    }; 

    // Change button color (default state)
    while (!WindowShouldClose()) {
        ui_state.mousePos = GetMousePosition();
        ui_state.currentTime = GetTime();

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

        GuiSetStyle(BUTTON, BORDER_COLOR_NORMAL, 0x0000FF);  // Blue border
        GuiSetStyle(BUTTON, BASE_COLOR_NORMAL, 0x00FF00);    // Green background
        GuiSetStyle(BUTTON, TEXT_COLOR_NORMAL, 0xFFFFFF);    // White text
        if(GuiButton(switchViewButton, "Switch")) {
            currentView = (currentView == VIEW_MAIN) ? VIEW_SECONDARY : VIEW_MAIN;
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
