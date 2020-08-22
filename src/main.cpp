#include <mbed.h>
#include "midi/midi.hpp"
#include "ymf825/ymf825.hpp"

MIDI midi(PA_9, PA_10, 31250);
YMF825 ymf825(PB_5, PB_4, PB_3, PB_6, PA_15);

//MIDI Callback Functions
void noteOnFunc(uint8_t ch, uint8_t note, uint8_t vel) {
    ymf825.noteOn(ch, note, vel);
    return;
}

void noteOffFunc(uint8_t ch, uint8_t note) {
    ymf825.noteOff(ch, note);
    return;
}

void pitchBendFunc(uint8_t ch, uint16_t val) {
    ymf825.pitchBend(ch, val);
    return;
}

void pitchBendSensitivityFunc(uint8_t ch, uint8_t val) {
    ymf825.pitchBendSensitivity(ch, val);
    return;
}

void modulationFunc(uint8_t ch, uint8_t val) {
    ymf825.modulation(ch, val);
    return;
}

void partLevelFunc(uint8_t ch, uint8_t val) {
    ymf825.partLevel(ch, val);
    return;
}

void expressionFunc(uint8_t ch, uint8_t val) {
    ymf825.expression(ch, val);
    return;
}

void holdFunc(uint8_t ch, uint8_t val) {
    if (val > 63) {
        ymf825.hold(ch, true);
    } else {
        ymf825.hold(ch, false);
    }
    return;
}

void programChangeFunc(uint8_t ch, uint8_t val) {
    ymf825.programChange(ch, val);
    return;
}

void allNoteOffFunc(uint8_t ch) {
    ymf825.allkeyOff();
    return;
}

void allSoundOffFunc(uint8_t ch) {
    ymf825.allMute();
    return;
}

void resetFunc(void) {
    ymf825.init();
    return;
}

//sub routines
void initCallbacks(void) {
    midi.setCallback(msg::noteOff, noteOffFunc);
    midi.setCallback(msg::noteOn, noteOnFunc);

    midi.setCallback(msg::pitchBend, pitchBendFunc);
    midi.setCallback(msg::pitchBendSensitivity, pitchBendSensitivityFunc);

    midi.setCallback(msg::modulation, modulationFunc);
    midi.setCallback(msg::channelVolume, partLevelFunc);
    midi.setCallback(msg::expression, expressionFunc);
    midi.setCallback(msg::hold, holdFunc);

    midi.setCallback(msg::programChange, programChangeFunc);
    midi.setCallback(msg::allNoteOff, allNoteOffFunc);
    midi.setCallback(msg::allSoundOff, allSoundOffFunc);

    midi.setCallback(msg::reset, resetFunc);
}

int main() {
    ymf825.init();
    initCallbacks();
    while (1) {
        midi.parseMIDI();
        // put your main code here, to run repeatedly:
    }
}