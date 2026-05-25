#ifndef KEYER_H
#define KEYER_H

#include <Arduino.h>

void keyer_init();
void keyer_set_speed(byte speed);
void keyer_update();
void keyer_stop();
void keyer_set_radio_output(boolean enabled);
void keyer_decode_reset();
void keyer_decode_update();
boolean keyer_decode_available();
char keyer_decode_read();
const char* keyer_mode_name(byte mode);

#endif
