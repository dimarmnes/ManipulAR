#include "display.h"
#include "config.h"

LiquidCrystal_I2C lcd(LCD_ADDR, LCD_COLS, LCD_ROWS);

void display_init() {
  lcd.init();
  lcd.backlight();
}

void displaySplash() {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("ManipulAR");
}

void displayMessage(const char* line1, const char* line2) {

  lcd.clear();

  lcd.setCursor(0,0);
  lcd.print(line1);

  lcd.setCursor(0,1);
  lcd.print(line2);
}

void displayClearLine(byte row) {
  lcd.setCursor(0, row);
  for (byte i = 0; i < LCD_COLS; i++) {
    lcd.print(' ');
  }
  lcd.setCursor(0, row);
}
