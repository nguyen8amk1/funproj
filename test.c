#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <raylib.h>

#define SAMPLE_RATE 1000
#define DURATION 1.0
#define NUM_POINTS 100
#define WIDTH 800
#define HEIGHT 600
#define NUM_WAVES 5

typedef struct {
    float time[NUM_POINTS];
    float values[NUM_POINTS];
} Cycle;

// Generate a wave with both positive and negative parts
void GenerateBalancedWave(float *t, float *wave, int length) {
    for (int i = 0; i < length; i++) {
        t[i] = (float)i / length * DURATION;
        wave[i] = sinf(2.0f * M_PI * 3.0f * t[i]); // Base sine wave
    }
    
    // Add random sine components
    for (int w = 0; w < NUM_WAVES; w++) {
        float freq = ((rand() % 10) + 1) * 1.0f;
        float phase = ((rand() % 360) / 360.0f) * 2 * M_PI;
        float amp = 0.5f + ((rand() % 50) / 100.0f);
        
        for (int i = 0; i < length; i++) {
            wave[i] += amp * sinf(2.0f * M_PI * freq * t[i] + phase);
        }
    }

    // Center around zero
    float sum = 0;
    for (int i = 0; i < length; i++) sum += wave[i];
    float mean = sum / length;
    
    for (int i = 0; i < length; i++) wave[i] -= mean;
}

// Find zero crossings
int FindZeroCrossings(float *wave, int length, int *crossings) {
    int count = 0;
    for (int i = 1; i < length; i++) {
        if ((wave[i - 1] <= 0 && wave[i] > 0) || (wave[i - 1] >= 0 && wave[i] < 0)) {
            crossings[count++] = i;
        }
    }
    return count;
}

// Extract full cycles
int ExtractCycles(float *t, float *wave, int *crossings, int num_crossings, Cycle *cycles, int max_cycles) {
    int count = 0;
    for (int i = 0; i < num_crossings - 2 && count < max_cycles; i++) {
        int start = crossings[i];
        int end = crossings[i + 2];

        // Resample to a fixed number of points
        for (int j = 0; j < NUM_POINTS; j++) {
            float alpha = (float)j / (NUM_POINTS - 1);
            int idx = start + alpha * (end - start);
            cycles[count].time[j] = alpha;
            cycles[count].values[j] = wave[idx];
        }
        count++;
    }
    return count;
}

// Compute average cycle
void ComputeAverageCycle(Cycle *cycles, int num_cycles, float *avg_cycle) {
    for (int i = 0; i < NUM_POINTS; i++) {
        float sum = 0;
        for (int j = 0; j < num_cycles; j++) {
            sum += cycles[j].values[i];
        }
        avg_cycle[i] = sum / num_cycles;
    }
}

// Visualize with Raylib
void Visualize(Cycle *cycles, int num_cycles, float *avg_cycle) {
    InitWindow(WIDTH, HEIGHT, "Wave Cycle Averaging");
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Draw cycles
        for (int c = 0; c < num_cycles; c++) {
            for (int i = 0; i < NUM_POINTS - 1; i++) {
                Vector2 p1 = {WIDTH * cycles[c].time[i], HEIGHT / 2 + cycles[c].values[i] * 100};
                Vector2 p2 = {WIDTH * cycles[c].time[i + 1], HEIGHT / 2 + cycles[c].values[i + 1] * 100};
                DrawLineV(p1, p2, Fade(LIGHTGRAY, 0.5f));
            }
        }

        // Draw averaged cycle
        for (int i = 0; i < NUM_POINTS - 1; i++) {
            Vector2 p1 = {WIDTH * (float)i / NUM_POINTS, HEIGHT / 2 + avg_cycle[i] * 100};
            Vector2 p2 = {WIDTH * (float)(i + 1) / NUM_POINTS, HEIGHT / 2 + avg_cycle[i + 1] * 100};
            DrawLineV(p1, p2, RED);
        }

        DrawText("Press ESC to Exit", 10, 10, 20, DARKGRAY);
        EndDrawing();
    }
    CloseWindow();
}

int main() {
    float t[SAMPLE_RATE], wave[SAMPLE_RATE];
    int zero_crossings[SAMPLE_RATE];
    Cycle cycles[50];  
    float avg_cycle[NUM_POINTS];

    // Generate and process wave
    GenerateBalancedWave(t, wave, SAMPLE_RATE);
    int num_crossings = FindZeroCrossings(wave, SAMPLE_RATE, zero_crossings);
    int num_cycles = ExtractCycles(t, wave, zero_crossings, num_crossings, cycles, 50);
    ComputeAverageCycle(cycles, num_cycles, avg_cycle);

    // Visualize
    Visualize(cycles, num_cycles, avg_cycle);
    return 0;
}
