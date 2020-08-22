#pragma once
#include <mbed.h>

struct rpn{
    uint8_t msb;
    uint8_t lsb;
};

struct message{
    uint8_t message;
    uint8_t channel;
    uint8_t second;
    uint8_t third;
};

struct sysExBuffer{
    uint8_t buffer[64];
    uint8_t pos;
};

struct callbacks{
    void (*noteOnFunc)(uint8_t,uint8_t,uint8_t);
    void (*noteOffFunc)(uint8_t,uint8_t);

    void (*controlChangeFunc)(uint8_t,uint8_t,uint8_t);
    void (*programChangeFunc)(uint8_t,uint8_t);

    void (*pitchBendFunc)(uint8_t,uint16_t);

    void (*allSoundOffFunc)(uint8_t);
    void (*allNoteOfffunc)(uint8_t);

    void (*sysExFunc)(uint8_t*,uint8_t);

    void (*resetFunc)(void);

    void (*modulationFunc)(uint8_t,uint8_t);
    void (*channelVolumeFunc)(uint8_t,uint8_t);
    void (*expressionFunc)(uint8_t,uint8_t);
    void (*holdFunc)(uint8_t,uint8_t);

    void (*pitchBendSensitivityFunc)(uint8_t,uint8_t);
};