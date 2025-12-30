#include <iostream>
#include <cstdint>
#include <string>
#include <vector>
#include "raylib.h"
#include "autocorrelation.h"
#include "pa_initialize.h"
#include "portaudio.h"


int main() {
    constexpr int screenWidth = 800;
    constexpr int screenHeight = 600;

    InitWindow(screenWidth, screenHeight, "Guitar Tuner");
    SetTargetFPS(60);
    paInit();

    PaStream* stream = inputStream();
    
    const size_t windowSize = 4096;
    const size_t bufferSize = 512;
    const size_t freqDataSize = 15;

    AudioWindow window(windowSize);
    float buffer[bufferSize];
    HzRingBuffer hzBuffer(freqDataSize);
    float hz;
    
    while(!WindowShouldClose()) {

        hz = analyzeInput(stream, buffer, window, bufferSize, windowSize);

        hzBuffer.push(hz);
        float cleanHz = hzBuffer.smoothing();

        BeginDrawing();
            ClearBackground(BLACK);

            DrawText(TextFormat("%.1f", cleanHz), 320, 260, 60, WHITE);

        EndDrawing();
    }

    Pa_Terminate();
}