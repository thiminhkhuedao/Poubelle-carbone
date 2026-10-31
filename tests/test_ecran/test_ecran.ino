#include <LiquidCrystal.h>
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

void setup() {
  lcd.begin(16, 2);
  lcd.print("Poubelle Carbone");
  lcd.setCursor(0, 1);
  lcd.print("ca marche !");
}

void loop() {}
