#include "raylib.h"
#include "raymath.h"


// Function for drawing a checkered bitmap
void DrawCheckeredBitmap(int width, int height, int cellSize, float zoom, Vector2 panOffset) {
    int rows = height / cellSize;
    int cols = width / cellSize;
    
    for (int y = 0; y < rows; y++) {
        for (int x = 0; x < cols; x++) {
            Color color = ((x + y) % 2 == 0) ? LIGHTGRAY : DARKGRAY;
            
            // Calculate the position of the cell with zoom and pan applied
            float cellX = (x * cellSize + panOffset.x) * zoom;
            float cellY = (y * cellSize + panOffset.y) * zoom;
            float cellSizeZoomed = cellSize * zoom;
            
            // Draw the cell
            DrawRectangle(cellX, cellY, cellSizeZoomed, cellSizeZoomed, color);
        }
    }
}

int main(void) {
    // Initialization of the window
    const int screenWidth = 800;
    const int screenHeight = 600;
    InitWindow(screenWidth, screenHeight, "Desmos-like Zoom and Pan Demo");

    // Initial zoom factor and pan offset
    float zoom = 1.0f;
    Vector2 panOffset = { 0.0f, 0.0f };

    // Initial position for panning calculation
    Vector2 initialMousePosition = { 0.0f, 0.0f };
    bool isPanning = false;

    SetTargetFPS(60);
    
    while (!WindowShouldClose()) {
        // Zooming with mouse wheel
        float mouseWheelMove = GetMouseWheelMove();
        Vector2 mousePosition = GetMousePosition();
        Vector2 mouseWorldPosBeforeZoom = {
            (mousePosition.x / zoom) - panOffset.x,
            (mousePosition.y / zoom) - panOffset.y
        };
        
        zoom += mouseWheelMove * 0.1f;  // Adjust zoom factor with mouse wheel move
        if (zoom < 0.1f) zoom = 0.1f;   // Limit zoom factor to prevent too much zoom out
        if (zoom > 10.0f) zoom = 10.0f; // Limit zoom factor to prevent too much zoom in
        
        Vector2 mouseWorldPosAfterZoom = {
            (mousePosition.x / zoom) - panOffset.x,
            (mousePosition.y / zoom) - panOffset.y
        };
        
        panOffset.x += mouseWorldPosBeforeZoom.x - mouseWorldPosAfterZoom.x;
        panOffset.y += mouseWorldPosBeforeZoom.y - mouseWorldPosAfterZoom.y;

        // Panning with mouse drag
        if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
            initialMousePosition = GetMousePosition();
            isPanning = true;
        }
        
        if (IsMouseButtonReleased(MOUSE_BUTTON_RIGHT)) {
            isPanning = false;
        }
        
        if (isPanning) {
            Vector2 currentMousePosition = GetMousePosition();
            Vector2 delta = Vector2Subtract(currentMousePosition, initialMousePosition);
            panOffset = Vector2Add(panOffset, Vector2Scale(delta, 1.0f / zoom));
            initialMousePosition = currentMousePosition;  // Update initial position for next frame
        }

        // Start drawing
        BeginDrawing();
        ClearBackground(RAYWHITE);
        
        // Draw the checkered bitmap with zoom and pan applied
        DrawCheckeredBitmap(screenWidth, screenHeight, 50, zoom, panOffset);
        
        // Draw instructions
        DrawText("Use mouse wheel to zoom in/out", 10, 10, 20, DARKGRAY);
        DrawText("Use middle mouse button to pan", 10, 40, 20, DARKGRAY);
        
        EndDrawing();
    }
    
    // De-initialization
    CloseWindow();

    return 0;
}
