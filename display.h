#ifndef DISPLAY_H
#define DISPLAY_H

#include <Arduino.h>
#include <LiquidCrystal_I2C.h>

extern LiquidCrystal_I2C lcd;

void display_init();
void displaySplash();

void displayMessage(const char* line1, const char* line2);
void displayClearLine(byte row);

#endif
