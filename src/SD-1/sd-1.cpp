#include "sd-1.hpp"

SD1::SD1(PinName mosi,PinName miso,PinName sclk,PinName ss,PinName rst) : _spi(mosi,miso,sclk),_ss(ss),_rst(rst){
    _spi.frequency(10000000UL);
    reset();
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
    _rst = 0;
    wait_us(100);
    _rst = 1;

    singleWrite(0x1D, 0x01);
    singleWrite(0x02, 0x0E);
    wait_us(1);
    singleWrite(0x00, 0x01);
    singleWrite(0x01, 0x00);
    singleWrite(0x1A, 0xA3);
    wait_us(1);
    singleWrite(0x1A, 0x00);
    thread_sleep_for(30);
    singleWrite(0x02, 0x04);
    wait_us(1);
    singleWrite(0x02, 0x00);
    singleWrite(0x19, 0x95);
    singleWrite(0x1B, 0x3F);
    singleWrite(0x14, 0x00);
    singleWrite(0x03, 0x01);

    singleWrite(0x08, 0xF6);
    wait_us(21);
    singleWrite(0x08, 0x00);
    singleWrite(0x09, 0xF8);
    singleWrite(0x0A, 0x00);

    singleWrite(0x17, 0x40);
    singleWrite(0x18, 0x00);

    for (int i = 0; i < 16; i++) {
        singleWrite(0x0B, i);
        singleWrite(0x0F, 0x30);
        singleWrite(0x10, 0x71);
        singleWrite(0x11, 0x00);
        singleWrite(0x12, 0x08);
        singleWrite(0x13, 0x00);
    }
}

inline uint8_t SD1::channelSearch(void){
    for(uint8_t i = recentCh;i < 0x10;i++){
        if(!fmStatus[i].isUsed){
            recentCh = i;
            return i;
        }
    }
    for(uint8_t i = 0;i < recentCh;i++){
        if(!fmStatus[i].isUsed){
            recentCh = i;
            return i;
        }
    }
    return 0xFF;
}

inline uint8_t  SD1::channelSearch(uint8_t midiChannel,uint8_t midiNote){
    for(uint8_t i = 0;i < 0x10;i++){
        if(fmStatus[i].isUsed && fmStatus[i].midiChannel == midiChannel && fmStatus[i].noteNumber == midiNote){
            return i;
        }
    }
    return 0xFF;
}

inline void SD1::channelSet(uint8_t channel,bool state){
    fmStatus[channel].isUsed = state;
    return;
}

inline void SD1::setSlaveSelect(bool state){

    //スレーブセレクトのアサート/ネゲート
    _ss = (state)? 0 : 1;
    return;
}

inline void SD1::setChannel(uint8_t channel){

    //チャネルを選択
    singleWrite(0x0B,channel);
    return;
}


inline void SD1::singleWrite(uint8_t address,uint8_t data){

    setSlaveSelect(true);
    //アドレス、データ転送
    write(address);
    write(data);

    setSlaveSelect(false);
    return;
}

inline void SD1::burstWrite(uint8_t address,uint8_t* data,uint8_t length){

    //Burst Writeモードに設定
    singleWrite(0x08,0x16);
    thread_sleep_for(1);
    singleWrite(0x08,0x00);

    setSlaveSelect(true);

    //アドレス、データ転送
    write(address);
    for(int i = 0;i < length;i++){
        write(data[i]);
    }

    setSlaveSelect(false);

    return;
}

inline void SD1::write(uint8_t data){
    _spi.write(data);
    return;
}