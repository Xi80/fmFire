#pragma once
#include <mbed.h>


const uint8_t gmSystemOn[] = {0x7E, 0x7F, 0x09, 0x01};

const uint8_t xgSystemOn[] = {0x43, 0x10, 0x4C, 0x00, 0x00, 0x7E, 0x00};

const uint8_t gsReset[] = {0x41, 0x10, 0x42, 0x12, 0x40, 0x00, 0x7F, 0x00, 0x41};

namespace msg{
    const uint8_t noteOff = 0x80;
    const uint8_t noteOn = 0x90;
    const uint8_t controlChange = 0xB0;
    const uint8_t programChange = 0xC0;
    const uint8_t pitchBend = 0xE0;

    const uint8_t allSoundOff = 0x78;
    const uint8_t allNoteOff = 0x7A;

    const uint8_t sysEx = 0xF0;
    const uint8_t beginSysEx= 0xF0;
    const uint8_t endSysEx = 0xF7;

    const uint8_t modulation = 0x01;
    const uint8_t channelVolume = 0x07;
    const uint8_t expression = 0x0B;
    const uint8_t hold = 0x40;

    const uint8_t dataEntry = 0x06;
    const uint8_t rpnLsb = 0x64;
    const uint8_t rpnMsb = 0x65;
    const uint8_t pitchBendSensitivity = 0x00;

    const uint8_t reset = 0xFF;
}