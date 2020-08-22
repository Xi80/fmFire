#pragma once

#include <mbed.h>
#include "circularBuffer.hpp"
#include "constants.hpp"
#include "structs.hpp"

#define _nullChk(x) if((x) == NULL)return

////////////////////////////////////////////////////////////////////////////////
/// @file           midi.hpp
/// @brief          mbed向けMIDIライブラリ
/// @author         LunaTsukinashi
/// @date           2020-07-29
/// @version        1.00
/// @note           なし
/// @attention      元のcircularBufferを使用した場合シリアル送信中の
///                 割り込みに対応できません。
/// @par            History
///                 Ver1.00 初版
///
/// Copyright (c) Luna Tsukinashi All Rights reserved.
///
/// - 本ソフトウェアの一部又は全てを無断で複写複製（コピー）することは、
///   著作権侵害にあたりますので、これを禁止します。
/// - 本製品の使用に起因する侵害または特許権その他権利の侵害に関しては
///   私は一切その責任を負いません。
///
////////////////////////////////////////////////////////////////////////////////

class MIDI {
public:
    MIDI(PinName tx, PinName rx, uint16_t baud);

    void setCallback(uint8_t, void (*func)(uint8_t, uint8_t, uint8_t));

    void setCallback(uint8_t, void (*func)(uint8_t, uint16_t));

    void setCallback(uint8_t, void (*func)(uint8_t, uint8_t));

    void setCallback(uint8_t, void (*func)(uint8_t *, uint8_t));

    void setCallback(uint8_t, void (*func)(uint8_t));

    void setCallback(uint8_t, void (*func)(void));

    void parseMIDI(void);

private:
    UnbufferedSerial _serial;

    lunaLib::CircularBuffer<uint8_t, 512> circularBuffer;

    uint8_t temp;

    //parseMIDI
    struct message msg;
    struct sysExBuffer sysExBuf;
    struct callbacks callbackFunc;
    struct rpn rpnMsg;
    bool threeByteFlag = false;
    bool isSystemExclusive = false;

    void getData(void);

    void setTwoBytes(bool);

    void parseNormalMessages(void);

    void parseSystemExclusives(void);

    void decodeNormalMessage(void);

    void decodeControlChange(void);

    void decodeRpn(void);

    void decodeSystemExclusive(void);

    bool checkReset(void);
};
