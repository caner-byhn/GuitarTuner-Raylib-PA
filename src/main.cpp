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

    TuningResult result;
    
    while(!WindowShouldClose()) {

        hz = analyzeInput(stream, buffer, window, bufferSize, windowSize);

        hzBuffer.push(hz);
        float cleanHz = hzBuffer.smoothing();

        result = getPitch(cleanHz);
        std::string instruction = " ";

        if (std::abs(result.cents) < 10.0f) instruction = "IN TUNE";
        else if (result.cents > 0) instruction = "TUNE DOWN";
        else instruction = "TUNE UP";

        BeginDrawing();
            ClearBackground(BLACK);

            DrawText(TextFormat("%.1f", cleanHz), 320, 240, 60, WHITE);
            DrawText(TextFormat("%s%d", result.note.c_str(), result.octave),
                    320, 310, 60, WHITE);
            DrawText(instruction.c_str(), 240, 380, 60, WHITE);

        EndDrawing();
    }

    Pa_Terminate();
}