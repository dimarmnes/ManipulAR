#ifndef PREFS_H
#define PREFS_H

#include <Arduino.h>

#define SAVED_FLG 0
#define GROUP_NUM 1
#define GROUP_DLY 2
#define KEY_SPEED 3
#define CHAR_SET  4
#define KOCH_NUM  5
#define KOCH_SKIP 6
#define RESERVED_OUT_MODE 7
#define SHOW_DLY  8
#define KEYER_MODE 9
#define NUM_PREFS 10
#define PERSONAL_CHARS_LEN 16
#define PERSONAL_CHARS_ADDR NUM_PREFS

#define KEY_SPEED_MIN 10
#define KEY_SPEED_MAX 30
#define KEY_SPEED_DEFAULT 20

// La frecuencia de sidetone se guarda separada para no mover la lista Personal.
// Valor en decenas de Hz: 70 equivale a 700 Hz.
#define TONE_PITCH_MIN 50
#define TONE_PITCH_MAX 80
#define TONE_PITCH_STEP 2
#define TONE_PITCH_DEFAULT 70
#define TONE_PITCH_ADDR (PERSONAL_CHARS_ADDR + PERSONAL_CHARS_LEN)

#define KEYER_STRAIGHT 0
#define KEYER_IAMBIC_A 1
#define KEYER_IAMBIC_B 2

extern byte prefs[NUM_PREFS];
extern char personal_chars[PERSONAL_CHARS_LEN + 1];
extern byte tone_pitch;

unsigned int tone_frequency_hz();
void prefs_init();
void prefs_save();

#endif
