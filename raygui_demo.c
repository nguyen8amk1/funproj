#define RAYGUI_IMPLEMENTATION
#include "raylib.h"
#include "raygui.h"

int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 800;
    const int screenHeight = 600;

    InitWindow(screenWidth, screenHeight, "raygui - Complex UI Demo");

    // GUI controls initialization
    bool checkboxChecked = false;
    int dropdownBoxActive = 0;
    bool dropdownEditMode = false;
    int spinnerValue = 0;
    float sliderValue = 50.0f;
    float sliderBarValue = 60.0f;
    char textBoxText[64] = "Text Box";
    bool textBoxEditMode = false;
    int listViewScrollIndex = 0;
    int listViewActive = -1;
    int listViewExActive = 0;
    int listViewExScrollIndex = 0;
    bool listViewExEditMode = false;
    bool toggleGroupActive = false;

    char *listViewItems[] = {"Item 1", "Item 2", "Item 3", "Item 4", "Item 5", "Item 6", "Item 7"};
    int listViewItemsCount = sizeof(listViewItems) / sizeof(listViewItems[0]);

    SetTargetFPS(60);
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())
    {
        // Update
        //----------------------------------------------------------------------------------

        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(RAYWHITE);

            // Draw GUI controls
            GuiPanel((Rectangle){ 20, 20, 760, 560 }, "Complex UI Demo");

            GuiGroupBox((Rectangle){ 30, 50, 200, 150 }, "Group Box");

            if (GuiButton((Rectangle){ 50, 80, 160, 30 }, "Button")) { /* Action */ }
            if (GuiButton((Rectangle){ 50, 120, 160, 30 }, "Another Button")) { /* Action */ }

            GuiLabel((Rectangle){ 250, 60, 100, 20 }, "Label:");
            // GuiTextBox((Rectangle){ 320, 50, 150, 30 }, textBoxText, 64, textBoxEditMode);
            // textBoxEditMode = GuiTextBoxIsActive();

            // GuiSpinner((Rectangle){ 320, 90, 100, 30 }, &spinnerValue, 0, 100, false);
            GuiSlider((Rectangle){ 320, 130, 150, 20 }, "Slider", TextFormat("%2.2f", sliderValue), &sliderValue, 0, 100);
            GuiSliderBar((Rectangle){ 320, 160, 150, 20 }, "SliderBar", TextFormat("%2.2f", sliderBarValue), &sliderBarValue, 0, 100);

            // checkboxChecked = GuiCheckBox((Rectangle){ 500, 50, 20, 20 }, "Checkbox", checkboxChecked);

            if (GuiDropdownBox((Rectangle){ 500, 80, 150, 30 }, "Option 1;Option 2;Option 3", &dropdownBoxActive, dropdownEditMode)) dropdownEditMode = !dropdownEditMode;

            // GuiListView((Rectangle){ 500, 120, 200, 200 }, "Item 1;Item 2;Item 3;Item 4;Item 5;Item 6;Item 7", &listViewScrollIndex, &listViewActive);
            //
            // if (GuiToggleGroup((Rectangle){ 50, 200, 160, 30 }, "ON;OFF", &toggleGroupActive)) { /* Action */ }

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}
