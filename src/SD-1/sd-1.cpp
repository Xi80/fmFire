#include "sd-1.hpp"

SD1::SD1(PinName mosi,PinName miso,PinName sclk,PinName ss,PinName rst) : _spi(mosi,miso,sclk),_ss(ss),_rst(rst){

}

void SD1::noteOff(uint8_t,uint8_t){

}

void SD1::noteOn(uint8_t,uint8_t,uint8_t){

}

void SD1::programChange(uint8_t,uint8_t){

}

void SD1::pitchBend(uint8_t,uint16_t){

}

void SD1::modulation(uint8_t,uint8_t){

}

void SD1::pitchBendSensitivity(uint8_t,uint8_t){

}

void SD1::expression(uint8_t,uint8_t){

}

void SD1::partLevel(uint8_t,uint8_t){

}

void SD1::hold(uint8_t,bool){

}

void SD1::allSoundsOff(uint8_t){

}

void SD1::allNotesOff(uint8_t){

}

void SD1::reset(void){

}

inline uint8_t SD1::channelSearch(void){

}

inline void SD1::channelSet(uint8_t,bool){

}

inline void SD1::setSlaveSelect(bool){

}

inline void SD1::setChannel(uint8_t){

}

inline void SD1::setAddress(uint8_t){

}

inline void SD1::singleWrite(uint8_t){

}

inline void SD1::burstWrite(uint8_t*,uint8_t){

}