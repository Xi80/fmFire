#include <mbed.h>

/*
 *  2020-08-18
 *  Luna Tsukinashi
 *  sd-1.hpp
 *  SD-1制御ライブラリ
 */

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

        inline uint8_t  channelSearch(void);
        inline void     channelSet(uint8_t,bool);
        inline void     setSlaveSelect(bool);
        inline void     setChannel(uint8_t);
        inline void     setAddress(uint8_t);
        inline void     singleWrite(uint8_t);
        inline void     burstWrite(uint8_t*,uint8_t);
};