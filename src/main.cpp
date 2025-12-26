#include <iostream>
#include <cstdint>
#include "raylib.h"

constexpr int screenWidth = 800;
constexpr int screenHeight = 600;

int main() {
    InitWindow(screenWidth, screenHeight, "Guitar Tuner");
    SetTargetFPS(60);

    while(!WindowShouldClose()) {


        BeginDrawing();


        EndDrawing();
    }

}