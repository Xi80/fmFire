#pragma once

#include <mbed.h>
#include "constants.hpp"
#include "structs.hpp"

class YMF825 {
public:
    //constructor
    YMF825(PinName mosi, PinName miso, PinName sck, PinName ss, PinName rst);

    void init(void);

    void noteOn(uint8_t channel, uint8_t note, uint8_t velocity);

    void noteOff(uint8_t channel, uint8_t note);

    void modulation(uint8_t channel, uint8_t value);

    void expression(uint8_t channel, uint8_t volume);

    void partLevel(uint8_t channel, uint8_t volume);

    void hold(uint8_t channel, bool state);

    void programChange(uint8_t channel, uint8_t inst);

    void pitchBend(uint8_t channel, uint16_t value);

    void pitchBendSensitivity(uint8_t channel, uint8_t value);

    void allkeyOff(void);

    void allMute(void);

private:
    SPI _spi;
    DigitalOut _ss, _rst;
    Ticker tk;

    inline void setChannelVolume(uint8_t channel, uint8_t exp, uint8_t paLv);

    inline void singleWrite(uint8_t addr, uint8_t data);

    inline void burstWrite(uint8_t addr, uint8_t *data, uint16_t length);

    void tick(void);

    inline uint8_t search();

    struct fmStatus_t fmStatus[16];
    struct midiStatus_t midiStatus[16];
    uint8_t prevChannel = 1;
    uint8_t tones[485];
};