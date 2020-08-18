#pragma once

#include <mbed.h>

//2020-08-18 LunaTsukinashi
//fmTone.h
//SD-1ライブラリの音色操作

#include "fmDriver.h"

//0-反映させるchのGM音色番号,反映させるch
inline uint8_t* makeToneList(uint8_t*,uint8_t);

//0-反映させるchのGM音色番号,反映させるch
void sendToneList(uint8_t*,uint8_t);
