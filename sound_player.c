#define RAYGUI_IMPLEMENTATION
#include "raylib.h"
#include "raygui.h"
#include <math.h>

// Constants
#define SAMPLE_RATE 44100
#define SAMPLE_SIZE 32
#define BUFFER_SIZE 4096

// Function to generate sine wave samples
void GenerateSineWave(float *buffer, int length, float frequency, float amplitude)
{
    for (int i = 0; i < length; i++)
    {
        buffer[i] = amplitude * sinf((2 * PI * frequency * i) / SAMPLE_RATE);
    }
}

int main(void)
{
    // Initialization
    const int screenWidth = 800;
    const int screenHeight = 450;
    InitWindow(screenWidth, screenHeight, "Sound Player Demo with Raygui");
    InitAudioDevice();

    // Sound generation
    float frequency = 440.0f; // A4 note
    float amplitude = 0.5f;
    float buffer[BUFFER_SIZE];
    GenerateSineWave(buffer, BUFFER_SIZE, frequency, amplitude);
    
    // Create a Sound object
    Sound sound;
    sound.stream = LoadAudioStream(SAMPLE_RATE, SAMPLE_SIZE, 1);
    sound.frameCount = BUFFER_SIZE;
    UpdateAudioStream(sound.stream, buffer, BUFFER_SIZE);

    // Variables for control
    bool isPlaying = false;
    bool isPaused = false;
    float volume = 1.0f;

    // GUI state variables
    Rectangle playButton = { 20, 60, 100, 30 };
    Rectangle pauseButton = { 140, 60, 100, 30 };
    Rectangle stopButton = { 260, 60, 100, 30 };
    Rectangle volumeSlider = { 20, 120, 340, 20 };

    // Main game loop
    while (!WindowShouldClose())
    {
        // Update controls
        if (GuiButton(playButton, "Play"))
        {
            if (!isPlaying)
            {
                PlaySound(sound);
                isPlaying = true;
                isPaused = false;
            }
        }

        if (GuiButton(pauseButton, "Pause"))
        {
            if (isPlaying && !isPaused)
            {
                PauseSound(sound);
                isPaused = true;
            }
            else if (isPaused)
            {
                ResumeSound(sound);
                isPaused = false;
            }
        }

        if (GuiButton(stopButton, "Stop"))
        {
            StopSound(sound);
            isPlaying = false;
            isPaused = false;
        }

        volume = GuiSlider(volumeSlider, "Volume", NULL, &volume, 0.0f, 1.0f);
        SetSoundVolume(sound, volume);

        // Draw
        BeginDrawing();
        ClearBackground(RAYWHITE);
        
        DrawText("Sound Player Demo", 20, 20, 20, DARKGRAY);
        
        DrawText(TextFormat("Volume: %.2f", volume), 20, 160, 20, DARKGRAY);
        EndDrawing();
    }

    // De-Initialization
    UnloadSound(sound);
    CloseAudioDevice();
    CloseWindow();

    return 0;
}
