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
    
    const size_t windowSize = 2048;
    const size_t bufferSize = 512;
    const size_t freqDataSize = 10;

    AudioWindow window(windowSize);
    float buffer[bufferSize];

    std::vector<float> freqData;
    float hz;
    int index = 0;

    float stableFreq = 0.0f;


    while(!WindowShouldClose()) {

        hz = analyzeInput(stream, buffer, window, bufferSize, windowSize);

        if (hz != 0.0f && freqData.size() < freqDataSize) {
            freqData.push_back(hz);
        }
        else if (hz != 0.0f) {
            freqData[index] = hz;
            index = (index + 1) % freqDataSize;
        }

        if (freqData.size() == freqDataSize) {
            float m = median(freqData);

            if (60 < m && m < 380) {
                stableFreq = m;
            }
        }

        BeginDrawing();
            ClearBackground(BLACK);

            DrawText(TextFormat("%.1f", stableFreq), 320, 260, 60, WHITE);

        EndDrawing();
    }

    Pa_Terminate();
}