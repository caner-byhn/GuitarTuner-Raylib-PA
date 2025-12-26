#include <iostream>
#include <cstdint>
#include "raylib.h"

constexpr int screenHeight = 1200;
constexpr int screenWidth = 800;

int main() {
    InitWindow(screenWidth, screenHeight, "Guitar Tuner");
    SetTargetFPS(60);

    while(!WindowShouldClose()) {


        BeginDrawing();


        EndDrawing();
    }

}