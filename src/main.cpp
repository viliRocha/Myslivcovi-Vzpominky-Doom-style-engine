#include <raylib.h>

int main() 
{   
    constexpr int screenWidth = 800;
    constexpr int screenHeight = 600;
    
    InitWindow(screenWidth, screenHeight, "Myslivcovi-Vzpominky");
    SetTargetFPS(60);
    
    while (!WindowShouldClose()) {
        BeginDrawing();
            ClearBackground(RAYWHITE);
            DrawText(TextFormat("FPS: %i", GetFPS()), 10, 10, 20, DARKGRAY);
            DrawText("Congrats! You created your first window!", 190, 200, 20, LIGHTGRAY);
        EndDrawing();
    }
    
    CloseWindow();
}