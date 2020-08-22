#include "ymf825.hpp"

YMF825::YMF825(PinName mosi, PinName miso, PinName sck, PinName ss, PinName rst) : _spi(mosi, miso, sck),
                                                                                   _ss(ss), _rst(rst) {
    _spi.frequency(10000000UL);
    init();
    tk.attach(callback(this, &YMF825::tick), 10ms);
}

void YMF825::init() {
    for (uint8_t i = 0; i < 16; i++) {
        fmStatus[i].isUsed = false;
        midiStatus[i].inst = 0;
        midiStatus[i].expression = 127;
        midiStatus[i].partLevel = 100;
        midiStatus[i].modulation = 0;
        midiStatus[i].hold = false;
        midiStatus[i].pitchBend = 8192;
        midiStatus[i].pitchBendSensitivity = 2;
    }

    _rst = 0;
    wait_us(100);
    _rst = 1;
    _ss = 1;

    singleWrite(0x1D, 0x00);
    singleWrite(0x02, 0x0E);
    thread_sleep_for(1);
    singleWrite(0x00, 0x01);
    singleWrite(0x01, 0x00);
    singleWrite(0x1A, 0xA3);
    thread_sleep_for(1);
    singleWrite(0x1A, 0x00);
    thread_sleep_for(30);
    singleWrite(0x02, 0x04);
    wait_us(10);
    singleWrite(0x02, 0x00);
    singleWrite(0x19, 0xCC);
    singleWrite(0x1B, 0x3F);
    singleWrite(0x14, 0x00);
    singleWrite(0x03, 0x01);
    singleWrite(0x09, 0xF8);
    programChange(15, 0);
    return;
}

inline void YMF825::singleWrite(uint8_t addr, uint8_t data) {
    _ss = 0;
    _spi.write(addr);
    _spi.write(data);
    _ss = 1;
    return;
}

inline void YMF825::burstWrite(uint8_t addr, uint8_t *data, uint16_t length) {
    singleWrite(0x08, 0x16);
    thread_sleep_for(1);
    singleWrite(0x08, 0x00);

    _ss = 0;
    _spi.write(addr);
    for (uint16_t i = 0; i < length; i++) {
        _spi.write(data[i]);
    }
    _ss = 1;

    return;
}

inline uint8_t YMF825::search() {
    uint8_t search = prevChannel;
    do {
        if (!fmStatus[search].isUsed) {
            prevChannel = search;
            return search;
        }
        search = (search + 1) % 16;
    } while (search != prevChannel);
    return 16;
}

void YMF825::noteOn(uint8_t channel, uint8_t note, uint8_t velocity) {
    uint8_t assign = search();

    if (assign == 16) {
        uint8_t maxVo = 0;
        uint8_t maxT = 0;
        for (uint8_t i = 0; i < 16; i++) {
            if (fmStatus[i].time >= maxT) {
                maxT = fmStatus[i].time;
                maxVo = i;
            }
        }
        assign = maxVo;
    }

    fmStatus[assign].isUsed = true;
    fmStatus[assign].midiChannel = channel;
    fmStatus[assign].note = note;

    singleWrite(0x0B, assign);
    singleWrite(0x0F, 0x00);

    if (midiStatus[channel].pitchBendSensitivity != 0) {
        uint8_t pit = midiStatus[channel].pitchBend >> 6;
        uint16_t INT = (pitchBendTable[midiStatus[channel].pitchBendSensitivity - 1][pit] >> 9) & 0x03;
        uint16_t FRAC = pitchBendTable[midiStatus[channel].pitchBendSensitivity - 1][pit] & 0x1FF;
        singleWrite(0x12, (INT << 3) | ((FRAC >> 6) & 0x07));
        singleWrite(0x13, (FRAC & 0x3F) << 1);
    }

    if ((midiStatus[channel].partLevel >> 3) && (midiStatus[channel].expression >> 3)) {
        singleWrite(0x10,
                    expTable[(midiStatus[channel].partLevel >> 3) - 1][(midiStatus[channel].expression >> 3) - 1]);
    } else {
        singleWrite(0x10, 0x00);
    }

    if (midiStatus[channel].modulation) {
        uint8_t modu = midiStatus[channel].modulation >> 4;
        singleWrite(0x11, modu);
    }

    fmStatus[assign].time = 0;
    singleWrite(0x0D, fNumberTableHigh[note]);
    singleWrite(0x0E, fNumberTableLow[note]);
    singleWrite(0x0C, velocity >> 1);
    singleWrite(0x0F, 0x40 | (channel & 0x0F));

    return;
}

void YMF825::noteOff(uint8_t channel, uint8_t note) {
    if (midiStatus[channel].hold)return;
    for (uint8_t i = 0; i < 16; i++) {
        if (fmStatus[i].isUsed && fmStatus[i].midiChannel == channel && fmStatus[i].note == note) {
            singleWrite(0x0B, i);
            singleWrite(0x0F, 0x00);
            fmStatus[i].isUsed = false;
            return;
        }
    }
    return;
}


void YMF825::modulation(uint8_t channel, uint8_t value) {
    midiStatus[channel].modulation = value;
    for (uint8_t i = 0; i < 16; i++) {
        if (fmStatus[i].midiChannel == channel) {
            singleWrite(0x0B, channel);
            singleWrite(0x11, midiStatus[channel].modulation >> 4);
        }
    }

    return;
}

inline void YMF825::setChannelVolume(uint8_t channel, uint8_t exp, uint8_t paLv) {
    for (uint8_t i = 0; i < 16; i++) {
        if (fmStatus[i].isUsed && fmStatus[i].midiChannel == channel) {
            singleWrite(0x0B, i);
            if ((exp >> 3) && (paLv >> 3)) {
                singleWrite(0x10,
                            expTable[(midiStatus[channel].partLevel >> 3) - 1][(midiStatus[channel].expression >> 3) -
                                                                               1]);
            } else {
                singleWrite(0x10, 0x00);
            }
        }
    }

    return;
}

void YMF825::expression(uint8_t channel, uint8_t volume) {
    midiStatus[channel].expression = volume;
    setChannelVolume(channel, midiStatus[channel].expression, midiStatus[channel].partLevel);
    return;
}

void YMF825::partLevel(uint8_t channel, uint8_t volume) {
    midiStatus[channel].partLevel = volume;
    setChannelVolume(channel, midiStatus[channel].expression, midiStatus[channel].partLevel);
    return;
}

void YMF825::hold(uint8_t channel, bool state) {
    if (state) {
        midiStatus[channel].hold = true;
    } else {
        midiStatus[channel].hold = false;
        for (uint8_t i = 0; i < 16; i++) {
            if (fmStatus[i].isUsed && fmStatus[i].midiChannel == channel) {
                fmStatus[i].isUsed = false;
            }
        }
    }
    return;
}

void YMF825::programChange(uint8_t channel, uint8_t inst) {
    midiStatus[channel].inst = inst;
    tones[0] = channel + 0x81;
    for (uint8_t i = 0; i <= channel; i++) {
        memcpy(&tones[30 * i + 1], &gmTable[midiStatus[i].inst][0], 30);
    }
    memcpy(&tones[30 * (channel + 1) + 1], &footer, 4);
    burstWrite(0x07, &tones[0], (channel + 1) * 30 + 5);
    return;
}

void YMF825::pitchBend(uint8_t channel, uint16_t value) {
    midiStatus[channel].pitchBend = value;
    if (midiStatus[channel].pitchBendSensitivity == 0)return;
    uint8_t pit = value >> 6;
    uint16_t INT = (pitchBendTable[midiStatus[channel].pitchBendSensitivity - 1][pit] >> 9) & 0x03;
    uint16_t FRAC = pitchBendTable[midiStatus[channel].pitchBendSensitivity - 1][pit] & 0x1FF;
    for (uint8_t i = 0; i < 16; i++) {
        if (fmStatus[i].midiChannel == channel && fmStatus[i].isUsed) {
            singleWrite(0x0B, i);
            singleWrite(0x12, (INT << 3) | ((FRAC >> 6) & 0x07));
            singleWrite(0x13, (FRAC & 0x3F) << 1);
        }
    }
    return;
}

void YMF825::pitchBendSensitivity(uint8_t channel, uint8_t value) {
    midiStatus[channel].pitchBendSensitivity = value;
    return;
}

void YMF825::allkeyOff(void) {
    for (uint8_t i = 0; i < 16; i++) {
        fmStatus[i].isUsed = false;
    }
    singleWrite(0x08, 0x80);
    return;
}

void YMF825::allMute(void) {
    for (uint8_t i = 0; i < 16; i++) {
        fmStatus[i].isUsed = false;
    }
    singleWrite(0x08, 0x40);
    return;
}

void YMF825::tick(void) {
    for (uint8_t i = 0; i < 16; i++) {
        if (fmStatus[i].isUsed || fmStatus[i].isRelease) {
            fmStatus[i].time++;
        }
    }
}