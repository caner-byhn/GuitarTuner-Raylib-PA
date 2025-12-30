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

struct HzRingBuffer {
    std::vector<float> data;
    size_t size = 0;
    size_t index = 0;
    float stable = 0.0f;

    HzRingBuffer(size_t freqDataSize);
    void push(float hz);
    float smoothing();
};


float autocorrelation(const std::vector<float>& samples, uint32_t sampleRate);
float analyzeInput(PaStream* stream, float buffer[], AudioWindow& window, size_t bufferSize, size_t windowSize);
float rms(const float* samples, size_t size);
void hzData(std::vector<float>& freqData, size_t freqDataSize, float hz);
float median(std::vector<float> freqData);