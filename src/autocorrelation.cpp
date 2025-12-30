#include <iostream>
#include <cstdint>
#include <vector>
#include <algorithm>
#include <math.h>
#include "autocorrelation.h"
#include "portaudio.h"


// Take a window of samples.
// For each lag > 0, compare the window with a time-shifted version of itself.
// For each lag, compute the sum of sample[i] * sample[i + lag].
// Each sum is a similarity score for that lag.
// All scores together form the autocorrelation curve.
// Peaks in this curve indicate repeating periods in the signal.

// We want to normalize the sum
AudioWindow::AudioWindow(size_t windowSize) : writeIndex(0), size(windowSize) {
    data = new float[size]();
}

AudioWindow::~AudioWindow(){
    delete[] data;
}


HzRingBuffer::HzRingBuffer(size_t freqDataSize) : size(freqDataSize), index(0), stable(0.0f) {
    data.resize(size, 0.0f);
}

void HzRingBuffer::push(float hz){
    if (hz <= 0.0f) return;

    data[index] = hz;
    index = (index + 1) % size;
}

float HzRingBuffer::smoothing(){
    float smoothingFactor = 0.15f; // Slower for guitar stability
    float threshold = 0.3f;        // Minimum Hz change to trigger update

    float m = median(data);
    if (std::abs(m - stable) > threshold) {
        stable = stable * (1.0f - smoothingFactor) + m * smoothingFactor;
    }
    return stable;
}


float autocorrelation(const float* samples, size_t windowSize, uint32_t sampleRate) {
    float minFreq = 60.0f;
    float maxFreq = 500.0f;

    int minLag = static_cast<int>(sampleRate / maxFreq);
    int maxLag = static_cast<int>(sampleRate / minFreq);

    float bestLag = 0.0f;
    float bestCorr = -1.0f;

    for (int lag = minLag; lag <= maxLag; lag++) {

        float dot = 0.0f;
        float energy1 = 0.0f;
        float energy2 = 0.0f;

        for (int i = 0; i + lag < windowSize; i++) {

            float a = samples[i];
            float b = samples[i + lag];

            dot += a * b;
            energy1 += a * a;
            energy2 += b * b;
        }
        
        if (energy1 == 0.0f || energy2 == 0.0f) continue;

        float normalized = dot / (sqrt(energy1) * sqrt(energy2));
        if (normalized > bestCorr) {
            bestCorr = normalized;
            bestLag = lag;
        }
    }

    return bestLag > 0.3f ? static_cast<float>(sampleRate) / bestLag : 0.0f;
}


float analyzeInput(PaStream* stream, float buffer[], 
                AudioWindow& window, 
                size_t bufferSize, 
                size_t windowSize
                ) {

    Pa_ReadStream(stream, buffer, bufferSize);

    float gain = 20.0f;

    for (int i = 0; i < bufferSize; i++) {
        window.data[window.writeIndex] = buffer[i] * gain;
        window.writeIndex = (window.writeIndex + 1) % windowSize;

        if (window.filled < windowSize) window.filled++;
    }

    if (window.filled < windowSize) return 0.0f;

    std::vector<float> temp(windowSize);

    for (size_t i = 0; i < windowSize; i++) {
        temp[i] = window.data[(window.writeIndex + i) % windowSize];
    }

    float r  = rms(temp.data(), windowSize);
    if (r < 0.015f) {
        std::cout << "RMS: " << r << "\n";
        return 0.0f;
    }

    return autocorrelation(temp.data(), windowSize, 44100);
}


float rms(const float* samples, size_t size) {
    float sum = 0.0f;

    for (int i = 0; i < size; i++) {
        sum += samples[i] * samples[i];
    }

    return sqrtf(sum / size);
}


float median(std::vector<float> freqData) {
    std::sort(freqData.begin(), freqData.end());

    int n = freqData.size();

    if (n % 2 == 1) {
        return freqData[n/2];
    }
    
    return 0.5f * (freqData[(n/2)-1] + freqData[n/2]);
}


TuningResult getPitch(float freq) {
    constexpr float A4 = 440.0f;
    constexpr const char* noteNames[12] = {
        "C", "C#", "D", "D#", "E", "F",
        "F#", "G", "G#", "A", "A#", "B"
    };

    float n = 12.0f * std::log2(freq / A4);
    int nearest = static_cast<int>(std::round(n));

    int midi = 69 + nearest;
    int noteIndex = midi % 12;
    int octave = midi / 12 - 1;

    float targetFreq = A4 * std::pow(2.0f, nearest / 12.0f);
    float cents = 1200.0f * std::log2(freq / targetFreq);

    return {
        noteNames[noteIndex],
        octave,
        cents,
        targetFreq
    };
}

//Use C style ring buffer for processing real time samples.
//Ring buffer is faster than C++ dynamic containers and less memory heavy with embedded hardware.
/*
123456

constexpr int N = 9;
float history[N];
int index = 0;
int count = 0;

void push(float hz) {
    history[index] = hz;
    index = (index + 1) % N;
    count = std::min(count + 1, N);
}
*/