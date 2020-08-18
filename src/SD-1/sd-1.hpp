#pragma once

#include <mbed.h>

/*
 *  2020-08-18
 *  Luna Tsukinashi
 *  sd-1.hpp
 *  SD-1制御ライブラリ
 */

#include "structs.hpp"

#include "table.hpp"

class SD1{
    public:

        SD1(PinName mosi,PinName miso,PinName sclk,PinName ss,PinName rst);

        void noteOff(uint8_t,uint8_t);
        void noteOn(uint8_t,uint8_t,uint8_t);
        void programChange(uint8_t,uint8_t);
        void pitchBend(uint8_t,uint16_t);

        void modulation(uint8_t,uint8_t);
        void pitchBendSensitivity(uint8_t,uint8_t);
        void expression(uint8_t,uint8_t);
        void partLevel(uint8_t,uint8_t);
        void hold(uint8_t,bool);
        void allSoundsOff(uint8_t);
        void allNotesOff(uint8_t);

        void reset(void);
    private:

        SPI _spi;
        DigitalOut _ss,_rst;

        uint8_t recentCh = 0;
        uint8_t toneBuffer[485];

        struct fmStatus_t fmStatus[16];
        struct midiStatus_t midiStatus[16];

        inline uint8_t  channelSearch(void);
        inline uint8_t  channelSearch(uint8_t,uint8_t);
        inline void     setSlaveSelect(bool);
        inline void     setChannel(uint8_t);
        inline void     singleWrite(uint8_t,uint8_t);
        inline void     burstWrite(uint8_t,uint8_t*,uint8_t);
        inline void     write(uint8_t data);
};