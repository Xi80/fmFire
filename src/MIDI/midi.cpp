#include "midi.hpp"


////////////////////////////////////////////////////////////////////////////////
/// @file           midi.cpp
/// @brief          mbed向けMIDIライブラリ
/// @author         LunaTsukinashi
/// @date           2020-07-29
/// @version        1.00
/// @note           ファイルに備考などを明記する場合はここへ書き込む
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

////////////////////////////////////////////////////////////////////////////////
/// @brief          MIDIライブラリコンストラクタ
/// @fn             MIDI
/// @param[in]      ボーレート(baud)
/// @param[out]     シリアル(UnbufferedSerial)
/// @return         なし
/// @author         LunaTsukinashi
/// @date           2020-07-29
/// @note           UnbufferedSerialを使用するため必然的にmbedOS6以降のみ対応
/// @attention      特になし
/// @par            History
///                 Ver1.00 初版
///
////////////////////////////////////////////////////////////////////////////////
MIDI::MIDI(PinName tx, PinName rx, uint16_t baud) : _serial(tx, rx) {

    //各種コールバック関数のリセット
    callbackFunc.noteOnFunc = NULL;
    callbackFunc.noteOffFunc = NULL;
    callbackFunc.controlChangeFunc = NULL;
    callbackFunc.programChangeFunc = NULL;
    callbackFunc.pitchBendFunc = NULL;
    callbackFunc.allSoundOffFunc = NULL;
    callbackFunc.allNoteOfffunc = NULL;
    callbackFunc.sysExFunc = NULL;
    callbackFunc.resetFunc = NULL;
    callbackFunc.modulationFunc = NULL;
    callbackFunc.channelVolumeFunc = NULL;
    callbackFunc.expressionFunc = NULL;
    callbackFunc.holdFunc = NULL;
    callbackFunc.pitchBendSensitivityFunc = NULL;

    //MIDI受信用の設定(Default:31250-8-N-1)
    _serial.baud(baud);

    _serial.format(
            /*ビット長*/        8,
            /*パリティ*/        SerialBase::None,
            /*ストップビット*/  1
    );

    //受信割込みにgetDataを登録
    _serial.attach(callback(this, &MIDI::getData), SerialBase::RxIrq);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief          初期化
/// @fn             init
/// @param[in]      なし
/// @param[out]     なし
/// @return         なし
/// @author         LunaTsukinashi
/// @date           2020-07-29
/// @note           UnbufferedSerialを使用するため必然的にmbedOS6以降のみ対応
/// @attention      他コードでシリアル送信を使う場合はCircularBufferの修正が
///                 必要。
/// @par            History
///                 Ver1.00 初版
///
////////////////////////////////////////////////////////////////////////////////
void MIDI::init(void) {
    circularBuffer.clear();
    _nullChk(callbackFunc.noteOnFunc);
     _nullChk(callbackFunc.noteOffFunc);
    callbackFunc.noteOnFunc(0x00,0x40,0x7F);
    thread_sleep_for(200);
    callbackFunc.noteOffFunc(0x00,0x40);
}



////////////////////////////////////////////////////////////////////////////////
/// @brief          データの受信
/// @fn             getData
/// @param[in]      なし
/// @param[out]     なし
/// @return         なし
/// @author         LunaTsukinashi
/// @date           2020-07-29
/// @note           UnbufferedSerialを使用するため必然的にmbedOS6以降のみ対応
/// @attention      他コードでシリアル送信を使う場合はCircularBufferの修正が
///                 必要。
/// @par            History
///                 Ver1.00 初版
///
////////////////////////////////////////////////////////////////////////////////
void MIDI::getData(void) {
    //データの存在チェック(無視してもおそらく問題ない)
    if (_serial.read(&temp, 1)) {
        circularBuffer.push_back(temp);
    }
}

////////////////////////////////////////////////////////////////////////////////
/// @brief          メッセージの整形
/// @fn             parseMIDI
/// @param[in]      なし
/// @param[out]     なし
/// @return         なし
/// @author         LunaTsukinashi
/// @date           2020-07-29
/// @note           なし
/// @attention      なし
/// @par            History
///                 Ver1.00 初版
///
////////////////////////////////////////////////////////////////////////////////
void MIDI::parseMIDI(void) {

    //システムエクスクルーシブメッセージ中か？
    if (isSystemExclusive) {
        parseSystemExclusives();
    } else {
        parseNormalMessages();
    }

    return;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief          2バイト目の格納
/// @fn             setTwoBytes
/// @param[in]      なし
/// @param[out]     flag
/// @return         なし
/// @author         LunaTsukinashi
/// @date           2020-07-29
/// @note           なし
/// @attention      なし
/// @par            History
///                 Ver1.00 初版
///
////////////////////////////////////////////////////////////////////////////////
void MIDI::setTwoBytes(bool flag) {
    //2バイト目にストア
    msg.second = temp;

    //3バイト目が来るか
    if (flag) {

        //3バイト目フラグをセット
        threeByteFlag = true;

    } else {

        //デコード処理
        decodeNormalMessage();
    }
    return;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief          通常メッセージのパース処理
/// @fn             parseNoramalMessages
/// @param[in]      なし
/// @param[out]     なし
/// @return         なし
/// @author         LunaTsukinashi
/// @date           2020-07-29
/// @note           なし
/// @attention      なし
/// @par            History
///                 Ver1.00 初版
///
////////////////////////////////////////////////////////////////////////////////
void MIDI::parseNormalMessages(void) {

    //バッファに何かあれば取り出す
    if (!circularBuffer.size())return;
    temp = circularBuffer.pull();

    //システムエクスクルーシブ開始か？
    if (temp == 0xF0) {

        //システムエクスクルーシブモードオン
        isSystemExclusive = true;

        //システムエクスクルーシブバッファクリア
        sysExBuf.pos = 0;

        return;
    }

    if (temp >> 7) {
        //ステータスバイト
        //0xF8以上は無視する
        if (temp >= 0xF8)return;

        //ランニングステータスバッファに格納
        msg.message = temp & 0xF0;
        msg.channel = temp & 0x0F;

        //3バイト目フラグをクリア
        threeByteFlag = false;

        return;

    } else {

        //データバイト
        //3バイト目フラグチェック
        if (threeByteFlag) {

            //3バイト目フラグをクリア
            threeByteFlag = false;

            //3バイト目にストア
            msg.third = temp;

            //デコード処理
            decodeNormalMessage();

            return;

        } else {

            //ランニングステータスバッファが0なら無視する
            if (!msg.message && !msg.channel)return;

            //ステータス< C0h
            if (msg.message < 0xC0) {

                //3バイトメッセージ
                setTwoBytes(true);

                return;

            } else {

                //ステータス<E0h
                if (msg.message < 0xE0) {

                    //2バイトメッセージ
                    setTwoBytes(false);

                    return;

                } else {

                    //ステータス < F0h
                    if (msg.message < 0xF0) {

                        //3バイトメッセージ
                        setTwoBytes(true);

                        return;
                    } else {

                        //ステータス >= F0h
                        if (msg.message >= 0xF0) {

                            //ステータス = F2h
                            if (msg.message == 0xF2) {

                                //ランニングステータスバッファをクリア
                                msg.message = 0x00;
                                msg.channel = 0x00;

                                //3バイトメッセージ
                                setTwoBytes(true);

                                return;
                            } else {
                                //ステータス = F3h or F1h
                                if (msg.message == 0xF3 || msg.message == 0xF1) {
                                    //ランニングステータスバッファをクリア
                                    msg.message = 0x00;
                                    msg.channel = 0x00;

                                    //2バイトメッセージ
                                    setTwoBytes(false);

                                    return;

                                } else {

                                    //ランニングステータスバッファをクリア
                                    msg.message = 0x00;
                                    msg.channel = 0x00;
                                    //無視
                                    return;

                                }
                            }
                        }
                    }
                }
            }

        }
    }
}

////////////////////////////////////////////////////////////////////////////////
/// @brief          システムエクスクルーシブのパース(格納処理)
/// @fn             parseSystemExclusives
/// @param[in]      なし
/// @param[out]     なし
/// @return         なし
/// @author         LunaTsukinashi
/// @date           2020-07-29
/// @note           なし
/// @attention      なし
/// @par            History
///                 Ver1.00 初版
///
////////////////////////////////////////////////////////////////////////////////
void MIDI::parseSystemExclusives(void) {
    //バッファに何かあれば取り出す
    if (!circularBuffer.size())return;
    temp = circularBuffer.pull();

    //システムエクスクルーシブ終了か？
    if (temp == 0xF7) {

        //システムエクスクルーシブモードオフ
        isSystemExclusive = false;

        //デコード処理
        decodeSystemExclusive();
        return;
    }

    //データ格納
    sysExBuf.buffer[sysExBuf.pos++] = temp;

}

////////////////////////////////////////////////////////////////////////////////
/// @brief          通常メッセージのデコード、関数呼び出し
/// @fn             decodeNormalMessage
/// @param[in]      なし
/// @param[out]     なし
/// @return         なし
/// @author         LunaTsukinashi
/// @date           2020-07-29
/// @note           なし
/// @attention      なし
/// @par            History
///                 Ver1.00 初版
///
////////////////////////////////////////////////////////////////////////////////
void MIDI::decodeNormalMessage(void) {
    switch (msg.message) {
        case msg::noteOn:

            //ベロシティが0=ノートオフ
            if (msg.third == 0x00) {
                _nullChk(callbackFunc.noteOffFunc);
                callbackFunc.noteOffFunc(msg.channel, msg.second);
                break;
            }

            //ノートオン
            _nullChk(callbackFunc.noteOnFunc);
            callbackFunc.noteOnFunc(msg.channel, msg.second, msg.third);
            break;
        case msg::noteOff:

            //ノートオフ
            _nullChk(callbackFunc.noteOffFunc);
            callbackFunc.noteOffFunc(msg.channel, msg.second);
            break;
        case msg::programChange:

            //プログラムチェンジ
            _nullChk(callbackFunc.programChangeFunc);
            callbackFunc.programChangeFunc(msg.channel, msg.second);
            break;
        case msg::controlChange:

            //コントロールチェンジ
            decodeControlChange();
            break;
        case msg::pitchBend:

            //ピッチベンド
            _nullChk(callbackFunc.pitchBendFunc);
            callbackFunc.pitchBendFunc(msg.channel, (msg.third << 7) | msg.second);
            break;
        default:

            //無視
            break;
    }
    return;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief          コントロールチェンジのデコード、関数呼び出し
/// @fn             decodeNormalMessage
/// @param[in]      なし
/// @param[out]     なし
/// @return         なし
/// @author         LunaTsukinashi
/// @date           2020-07-29
/// @note           なし
/// @attention      なし
/// @par            History
///                 Ver1.00 初版
///
////////////////////////////////////////////////////////////////////////////////
void MIDI::decodeControlChange(void) {
    switch (msg.second) {
        case msg::modulation:

            //モジュレーション
            _nullChk(callbackFunc.modulationFunc);
            callbackFunc.modulationFunc(msg.channel, msg.third);
            break;
        case msg::dataEntry:

            //データエントリー
            decodeRpn();
            break;
        case msg::channelVolume:

            //チャネルボリューム
            _nullChk(callbackFunc.channelVolumeFunc);
            callbackFunc.channelVolumeFunc(msg.channel, msg.third);
            break;
        case msg::expression:

            //エクスプレッション
            _nullChk(callbackFunc.expressionFunc);
            callbackFunc.expressionFunc(msg.channel, msg.third);
            break;
        case msg::hold:

            //ホールド1
            _nullChk(callbackFunc.holdFunc);
            callbackFunc.holdFunc(msg.channel, msg.third);
            break;
        case msg::rpnMsb:

            //RPN MSB
            rpnMsg.msb = msg.third;
            break;
        case msg::rpnLsb:

            //RPN LSB
            rpnMsg.lsb = msg.third;
            break;
        case msg::allSoundOff:

            //オールサウンドオフ
            _nullChk(callbackFunc.allSoundOffFunc);
            callbackFunc.allSoundOffFunc(msg.channel);
            break;
        case msg::allNoteOff:

            //オールノートオフ
            _nullChk(callbackFunc.allNoteOfffunc);
            callbackFunc.allNoteOfffunc(msg.channel);
            break;
    }
    return;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief          RPNのデコード、関数呼び出し
/// @fn             decodeNormalMessage
/// @param[in]      なし
/// @param[out]     なし
/// @return         なし
/// @author         LunaTsukinashi
/// @date           2020-07-29
/// @note           なし
/// @attention      なし
/// @par            History
///                 Ver1.00 初版
///
////////////////////////////////////////////////////////////////////////////////
void MIDI::decodeRpn(void) {
    uint16_t rpn = (rpnMsg.msb << 8) | rpnMsg.lsb;

    switch (rpn) {
        case 0x0000:
            //ピッチベンドセンシティビティ
            _nullChk(callbackFunc.pitchBendSensitivityFunc);
            callbackFunc.pitchBendSensitivityFunc(msg.channel, msg.third);
            break;
        default:

            //無視
            break;
    }

    //初期化
    rpnMsg.msb = 0x7F;
    rpnMsg.lsb = 0x7F;

    return;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief          システムエクスクルーシブのデコード、関数呼び出し
/// @fn             parseSystemExclusives
/// @param[in]      なし
/// @param[out]     なし
/// @return         なし
/// @author         LunaTsukinashi
/// @date           2020-07-29
/// @note           なし
/// @attention      なし
/// @par            History
///                 Ver1.00 初版
///
////////////////////////////////////////////////////////////////////////////////
void MIDI::decodeSystemExclusive(void) {
    if (checkReset()) {
        //リセット時の関数呼び出し
        _nullChk(callbackFunc.resetFunc);
        callbackFunc.resetFunc();
    } else {

        //それ以外の時の関数呼び出し
        _nullChk(callbackFunc.sysExFunc);
        callbackFunc.sysExFunc(sysExBuf.buffer, sysExBuf.pos);

    }

    return;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief          システムエクスクルーシブのパース(格納処理)
/// @fn             parseSystemExclusives
/// @param[in]      なし
/// @param[out]     なし
/// @return         bool(true:リセット/false:それ以外)
/// @author         LunaTsukinashi
/// @date           2020-07-29
/// @note           なし
/// @attention      なし
/// @par            History
///                 Ver1.00 初版
///
////////////////////////////////////////////////////////////////////////////////
bool MIDI::checkReset(void) {

    //リセットかどうか
    bool flag;
    //まず長さで判断
    if (sysExBuf.pos < 3)return false;
    //GMシステムオンと比較
    flag = true;
    for (int i = 0; i <= 3; i++) {
        if (gmSystemOn[i] != sysExBuf.buffer[i]) {
            flag = false;
        }
    }
    if (flag) {
        return true;
    }
    if (sysExBuf.pos < 6)return false;
    //XGシステムオンと比較
    flag = true;
    for (int i = 0; i <= 6; i++) {
        if (xgSystemOn[i] != sysExBuf.buffer[i]) {
            flag = false;
        }
    }
    if (flag)return true;
    //GSリセットと比較
    if (sysExBuf.pos < 8)return false;
    flag = true;
    for (int i = 0; i <= 8; i++) {
        if (gsReset[i] != sysExBuf.buffer[i]) {
            flag = false;
        }
    }
    if (flag)return true;

    return false;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief          コールバック関数の設定
/// @fn             setCallback
/// @param[in]      関数ポインタ(uint8_t,uint8_t,uint8_t)
/// @param[out]     コールバックタイプ(イベント)
/// @return         なし
/// @author         LunaTsukinashi
/// @date           2020-07-29
/// @note           なし
/// @attention      なし
/// @par            History
///                 Ver1.00 初版
///
////////////////////////////////////////////////////////////////////////////////
void MIDI::setCallback(uint8_t msgType, void (*func)(uint8_t, uint8_t, uint8_t)) {
    switch (msgType) {
        case msg::noteOn:

            //ノートオン
            callbackFunc.noteOnFunc = func;
            break;
        case msg::controlChange:

            //コントロールチェンジ
            callbackFunc.controlChangeFunc = func;
            break;
        default:

            //無視
            break;
    }
    return;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief          コールバック関数の設定
/// @fn             setCallback
/// @param[in]      関数ポインタ(uint8_t,uint8_t,uint8_t)
/// @param[out]     コールバックタイプ(イベント)
/// @return         なし
/// @author         LunaTsukinashi
/// @date           2020-07-29
/// @note           なし
/// @attention      なし
/// @par            History
///                 Ver1.00 初版
///
////////////////////////////////////////////////////////////////////////////////
void MIDI::setCallback(uint8_t msgType, void (*func)(uint8_t, uint16_t)) {
    switch (msgType) {
        case msg::pitchBend:

            //ピッチベンド
            callbackFunc.pitchBendFunc = func;
            break;
        default:

            //無視
            break;
    }
    return;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief          コールバック関数の設定
/// @fn             setCallback
/// @param[in]      関数ポインタ(uint8_t,uint8_t,uint8_t)
/// @param[out]     コールバックタイプ(イベント)
/// @return         なし
/// @author         LunaTsukinashi
/// @date           2020-07-29
/// @note           なし
/// @attention      なし
/// @par            History
///                 Ver1.00 初版
///
////////////////////////////////////////////////////////////////////////////////
void MIDI::setCallback(uint8_t msgType, void (*func)(uint8_t, uint8_t)) {
    switch (msgType) {
        case msg::noteOff:

            //ノートオフ
            callbackFunc.noteOffFunc = func;
            break;
        case msg::programChange:

            //プログラムチェンジ
            callbackFunc.programChangeFunc = func;
            break;
        case msg::hold:

            //ホールド
            callbackFunc.holdFunc = func;
            break;
        case msg::pitchBendSensitivity:

            //ピッチベンドセンシティビティ
            callbackFunc.pitchBendSensitivityFunc = func;
            break;
        case msg::modulation:

            //モジュレーション
            callbackFunc.modulationFunc = func;
            break;
        case msg::channelVolume:

            //チャネルボリューム
            callbackFunc.channelVolumeFunc = func;
            break;
        case msg::expression:

            //エクスプレッション
            callbackFunc.expressionFunc = func;
            break;
        default:

            //無視
            break;
    }
    return;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief          コールバック関数の設定
/// @fn             setCallback
/// @param[in]      関数ポインタ(uint8_t,uint8_t,uint8_t)
/// @param[out]     コールバックタイプ(イベント)
/// @return         なし
/// @author         LunaTsukinashi
/// @date           2020-07-29
/// @note           なし
/// @attention      なし
/// @par            History
///                 Ver1.00 初版
///
////////////////////////////////////////////////////////////////////////////////
void MIDI::setCallback(uint8_t msgType, void (*func)(uint8_t)) {
    switch (msgType) {
        case msg::allSoundOff:

            //オールサウンドオフ
            callbackFunc.allSoundOffFunc = func;
            break;
        case msg::allNoteOff:

            //オールノートオフ
            callbackFunc.allNoteOfffunc = func;
            break;

        default:

            //無視
            break;
    }
    return;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief          コールバック関数の設定
/// @fn             setCallback
/// @param[in]      関数ポインタ(uint8_t,uint8_t,uint8_t)
/// @param[out]     コールバックタイプ(イベント)
/// @return         なし
/// @author         LunaTsukinashi
/// @date           2020-07-29
/// @note           なし
/// @attention      なし
/// @par            History
///                 Ver1.00 初版
///
////////////////////////////////////////////////////////////////////////////////
void MIDI::setCallback(uint8_t msgType, void (*func)(void)) {
    switch (msgType) {
        case msg::reset:

            //リセット
            callbackFunc.resetFunc = func;
            break;
        default:

            //無視
            break;
    }
    return;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief          コールバック関数の設定
/// @fn             setCallback
/// @param[in]      関数ポインタ(uint8_t*,uint8_t)
/// @param[out]     コールバックタイプ(イベント)
/// @return         なし
/// @author         LunaTsukinashi
/// @date           2020-07-29
/// @note           なし
/// @attention      なし
/// @par            History
///                 Ver1.00 初版
///
////////////////////////////////////////////////////////////////////////////////
void MIDI::setCallback(uint8_t msgType, void (*func)(uint8_t *, uint8_t)) {
    switch (msgType) {
        case msg::sysEx:

            //リセット
            callbackFunc.sysExFunc = func;
            break;
        default:

            //無視
            break;
    }
    return;
}