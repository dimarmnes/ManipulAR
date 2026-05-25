#include <Arduino.h>
#include "MorseEnDecoder.h"

#include "cw_engine.h"
#include "prefs.h"

extern MorseEncoder morseOutput;

void cw_set_speed(byte speed) {

  morseOutput.setspeed(speed);
}

void cw_write(char c) {

  morseOutput.write(c);
}