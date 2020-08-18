#pragma once

#include <mbed.h>

/*
 *  2020-08-18
 *  Luna Tsukinashi
 *  structs.hpp
 *  SD-1ライブラリの構造体群
 */

struct fmStatus_t {
    bool isUsed = false;
    bool isHold = false;

    uint8_t midiChannel = 0;
    uint8_t noteNumber = 0;
};

struct midiStatus_t {
    uint8_t partLevel = 100;
    uint8_t expression = 127;
    uint8_t modulation = 0;
    uint8_t pitchBend = 8192 >> 6;
    uint8_t pitchBendSensitivity = 2;
    uint8_t programNumber = 0;

    bool isHold = false;
};