#define RAYLIB_NUKLEAR_IMPLEMENTATION
#include <raylib.h>
#include <raylib-nuklear.h>

int main() {
    InitWindow(1600, 900, "raylib-nuklear example");

    // Create the Nuklear Context
    int fontSize = 10;
    struct nk_context *ctx = InitNuklear(fontSize);

    struct Color bg = {0, 255, 0, 255};


    while (!WindowShouldClose()) {
        // Update the Nuklear context, along with input
        UpdateNuklear(ctx);

        // Nuklear GUI Code
        // https://github.com/Immediate-Mode-UI/Nuklear/wiki/Window
        if (nk_begin(ctx, "Nuklear", nk_rect(100, 100, 220, 220),
                NK_WINDOW_BORDER|NK_WINDOW_MOVABLE|NK_WINDOW_CLOSABLE)) {
            nk_layout_row_static(ctx, 50, 150, 1);
            if (nk_button_label(ctx, "Button")) {
                // Button was clicked!
            }
        }
        nk_end(ctx);

        // Render
        BeginDrawing();
            ClearBackground(bg);

            // Render the Nuklear GUI
            DrawNuklear(ctx);
            DrawFPS(30,900-30);

        EndDrawing();
    }

    // De-initialize the Nuklear GUI
    UnloadNuklear(ctx);

    CloseWindow();
    return 0;
}