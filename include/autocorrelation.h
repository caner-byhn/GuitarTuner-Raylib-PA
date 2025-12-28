#pragma once

#include <cstdint>
#include <vector>
#include "portaudio.h"

struct AudioWindow {
    float* data = nullptr;
    size_t writeIndex = 0;
    size_t size = 0;
    size_t filled = 0;

    AudioWindow(size_t windowSize);
    ~AudioWindow();
};


float autocorrelation(const std::vector<float>& samples, uint32_t sampleRate);
float analyzeInput(PaStream* stream, float buffer[], AudioWindow& window, size_t bufferSize, size_t windowSize);
float median(std::vector<float> freqData);
float rms(const float* samples, size_t size);