#include <iostream>
#include <cstdint>
#include <vector>
#include <math.h>
#include "autocorrelation.h"
#include "portaudio.h"
#include "pa_initialize.h"


void paInit() {
    PaError err = Pa_Initialize();
    if (err != paNoError) {
        std::cerr << "PortAudio Error: " << Pa_GetErrorText(err) << "\n";
    }
}

PaStream* inputStream() {
    PaStream* stream;
    PaStreamParameters inputParams{};

    inputParams.device =  Pa_GetDefaultInputDevice();
    inputParams.channelCount = 1;
    inputParams.sampleFormat = paFloat32;
    inputParams.suggestedLatency = Pa_GetDeviceInfo(inputParams.device)->defaultLowInputLatency;
    inputParams.hostApiSpecificStreamInfo = nullptr;

    PaError err = Pa_OpenStream(
        &stream,
        &inputParams,
        nullptr,
        44100,
        512,
        paNoFlag,
        nullptr,
        nullptr
    );

    if (err != paNoError) {
        std::cerr << Pa_GetErrorText(err) << "\n";
        return nullptr;
    }

    Pa_StartStream(stream);
    return stream;
}