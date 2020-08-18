#include "sd-1.hpp"

SD1::SD1(PinName mosi,PinName miso,PinName sclk,PinName ss,PinName rst) : _spi(mosi,miso,sclk),_ss(ss),_rst(rst){
    _spi.frequency(10000000UL);
    reset();
}

void SD1::noteOff(uint8_t midiChannel,uint8_t midiNote){
    uint8_t ch  = channelSearch(midiChannel,midiNote);
    if(!fmStatus[ch].isHold){
        setChannel(ch);
        singleWrite(0x0F, 0x00);
        fmStatus[ch].isUsed = false;
    }

    return;
}

void SD1::noteOn(uint8_t midiChannel,uint8_t midiNote,uint8_t midiVelocity){
    uint8_t ch = channelSearch();
    fmStatus[ch].isUsed = true;
    setChannel(ch);

    //ピッチベンド
    uint8_t pit;
    uint8_t INT,FRAC;
    if(midiStatus[midiChannel].pitchBendSensitivity){
        pit = midiStatus[midiChannel].pitchBend >> 6;
        INT = (pitchBendTable[midiStatus[midiChannel].pitchBendSensitivity - 1][pit] >> 9) & 0x03;
        FRAC = pitchBendTable[midiStatus[midiChannel].pitchBendSensitivity - 1][pit] & 0x1FF;

    } else {
        INT = (0x2000 >> 9) & 0x03;
        FRAC = 0x2000 & 0x01FF;
    }
    singleWrite(0x12, (INT << 3) | ((FRAC >> 6) & 0x07));
    singleWrite(0x13, (FRAC & 0x3F) << 1);

    //モジュレーション
    uint8_t modulation = midiStatus[midiChannel].modulation >> 4;
    singleWrite(0x11, modulation);

    //パートレベル・エクスプレッション
    uint8_t chVol;
    if(midiStatus[midiChannel].partLevel || midiStatus[midiChannel].expression){
        chVol = expTable[(midiStatus[midiChannel].partLevel >> 3)][(midiStatus[midiChannel].expression >> 3)];
    } else {
        chVol = 0x00;
    }
    singleWrite(0x10,chVol);

    //ベロシティ
    uint8_t velocity = midiVelocity >> 1;
    singleWrite(0x0C, velocity);

    //ホールド
    fmStatus[ch].isHold = midiStatus[midiChannel].isHold;

    //ノートオン
    singleWrite(0x0D, fNumberTableHigh[midiNote]);
    singleWrite(0x0E, fNumberTableLow[midiNote]);

    singleWrite(0x0F, 0x40 | (midiChannel & 0x0F));

    return;
}

void SD1::programChange(uint8_t midiChannel,uint8_t inst){
    midiStatus[midiChannel].programNumber = inst;

    toneBuffer[0] = 0x81 + midiChannel;

    for(uint8_t i = 0;i <= midiChannel;i++){
        for(uint8_t j = 0;j < 30;j++){
            toneBuffer[30 * i + j] = gmTable[midiStatus[i].programNumber][j];
        }
    }

    toneBuffer[30 * (midiChannel + 1) + 1] = 0x80;
    toneBuffer[30 * (midiChannel + 1) + 2] = 0x03;
    toneBuffer[30 * (midiChannel + 1) + 3] = 0x81;
    toneBuffer[30 * (midiChannel + 1) + 4] = 0x80;

    burstWrite(0x07,&toneBuffer[0],30 * (midiChannel + 1) + 5);
    return;
}

void SD1::pitchBend(uint8_t midiChannel,uint16_t pitch){
    midiStatus[midiChannel].pitchBend = pitch;

    uint8_t pit;
    uint8_t INT,FRAC;
    if(midiStatus[midiChannel].pitchBendSensitivity){
        pit = midiStatus[midiChannel].pitchBend >> 6;
        INT = (pitchBendTable[midiStatus[midiChannel].pitchBendSensitivity - 1][pit] >> 9) & 0x03;
        FRAC = pitchBendTable[midiStatus[midiChannel].pitchBendSensitivity - 1][pit] & 0x1FF;

    } else {
        INT = (0x2000 >> 9) & 0x03;
        FRAC = 0x2000 & 0x01FF;
    }

    for(uint8_t i = 0;i < 0x10;i++){
        if(fmStatus[i].isUsed && fmStatus[i].midiChannel == midiChannel){
            setChannel(i);
            singleWrite(0x12, (INT << 3) | ((FRAC >> 6) & 0x07));
            singleWrite(0x13, (FRAC & 0x3F) << 1);
        }
    }

    return;
}

void SD1::modulation(uint8_t midiChannel,uint8_t value){
    midiStatus[midiChannel].modulation = value;
    uint8_t modulation = midiStatus[midiChannel].modulation >> 4;

    for(uint8_t i = 0;i < 0x10;i++){
        if(fmStatus[i].isUsed && fmStatus[i].midiChannel == midiChannel){
            setChannel(i);
            singleWrite(0x11, modulation);
        }
    }

    return;
}

void SD1::pitchBendSensitivity(uint8_t midiChannel,uint8_t value){
    midiStatus[midiChannel].pitchBendSensitivity = value;
    return;
}

void SD1::expression(uint8_t midiChannel,uint8_t volume){
    midiStatus[midiChannel].expression = volume;
    uint8_t chVol;

    if(midiStatus[midiChannel].partLevel || midiStatus[midiChannel].expression){
        chVol = expTable[(midiStatus[midiChannel].partLevel >> 3)][(midiStatus[midiChannel].expression >> 3)];
    } else {
        chVol = 0x00;
    }

    for(uint8_t i = 0;i < 0x10;i++){
        if(fmStatus[i].isUsed && fmStatus[i].midiChannel == midiChannel){
            setChannel(i);
            singleWrite(0x10,chVol);
        }
    }

    return;
}

void SD1::partLevel(uint8_t midiChannel,uint8_t volume){
    midiStatus[midiChannel].partLevel = volume;
    uint8_t chVol;

    if(midiStatus[midiChannel].partLevel || midiStatus[midiChannel].expression){
        chVol = expTable[(midiStatus[midiChannel].partLevel >> 3)][(midiStatus[midiChannel].expression >> 3)];
    } else {
        chVol = 0x00;
    }

    for(uint8_t i = 0;i < 0x10;i++){
        if(fmStatus[i].isUsed && fmStatus[i].midiChannel == midiChannel){
            setChannel(i);
            singleWrite(0x10,chVol);
        }
    }

    return;
}

void SD1::hold(uint8_t midiChannel,bool state){
    if(state){
        midiStatus[midiChannel].isHold = true;
        for(uint8_t i = 0;i < 0x10;i++){
            if(fmStatus[i].isUsed && fmStatus[i].midiChannel == midiChannel){
                fmStatus[i].isHold = true;
            }
        }
    } else {
        midiStatus[midiChannel].isHold = false;
        for(uint8_t i = 0;i < 0x10;i++){
            if(fmStatus[i].isUsed && fmStatus[i].midiChannel == midiChannel){
                fmStatus[i].isHold = false;
                setChannel(i);
                singleWrite(0x0F, 0x00);
                fmStatus[i].isUsed = false;
            }
        }
    }
}

void SD1::allSoundsOff(uint8_t){
    for(uint8_t i = 0;i < 0x10;i++){
        fmStatus[i].isUsed = false;
    }
    singleWrite(0x08, 0x40);
    return;
}

void SD1::allNotesOff(uint8_t){
    for(uint8_t i = 0;i < 0x10;i++){
        fmStatus[i].isUsed = false;
    }
    singleWrite(0x08, 0x80);
    return;
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
        setChannel(i);
        singleWrite(0x0F, 0x30);
        singleWrite(0x10, 0x71);
        singleWrite(0x11, 0x00);
        singleWrite(0x12, 0x08);
        singleWrite(0x13, 0x00);
    }
    programChange(0x0F,0);
    return;
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