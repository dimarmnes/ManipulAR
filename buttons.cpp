#include <Arduino.h>
#include "config.h"
#include "buttons.h"

byte readButtons() {

  if (digitalRead(BTN_UP) == LOW) {
    delay(20);
    if (digitalRead(BTN_UP) == LOW)
      return 1;
  }

  if (digitalRead(BTN_DOWN) == LOW) {
    delay(20);
    if (digitalRead(BTN_DOWN) == LOW)
      return 2;
  }

  if (digitalRead(BTN_SELECT) == LOW) {
    delay(20);
    if (digitalRead(BTN_SELECT) == LOW)
      return 3;
  }

  return 0;
}
