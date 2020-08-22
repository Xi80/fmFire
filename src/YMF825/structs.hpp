#pragma once

#include <mbed.h>

struct midiStatus_t {
    uint8_t modulation = 0;
    uint8_t partLevel = 100;
    uint8_t expression = 127;
    bool hold = false;

    uint16_t pitchBend = 8192;
    uint8_t pitchBendSensitivity = 2;

    uint8_t inst;
};

struct fmStatus_t {
    bool isUsed = false;
    bool isRelease = false;
    uint8_t midiChannel = 0;
    uint8_t note = 0;
    uint16_t time = 0;
};