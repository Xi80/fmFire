#pragma once

#include <mbed.h>

//2020-08-18 LunaTsukinashi
//fmDriver.h
//SD-1の低レベル制御

#include "pinAssign.h"

void init(void);

inline void singleWrite(uint8_t,uint8_t);

inline void burstWrite(uint8_t*,uint8_t);

inline void setSlaveSelect(bool);
