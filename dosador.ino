#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define INC_PIN 6  
#define U_D_PIN 7  
#define CS_PIN 8   

LiquidCrystal_I2C lcd(0x27, 16, 2);

int potValue = 0;

void setup() {
  pinMode(INC_PIN, OUTPUT);
  pinMode(U_D_PIN, OUTPUT);
  pinMode(CS_PIN, OUTPUT);

  digitalWrite(CS_PIN, HIGH);
  digitalWrite(INC_PIN, HIGH);

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Potenciometro:");
  lcd.setCursor(0, 1);
  lcd.print("Valor: ");
  lcd.print(potValue);

  Serial.begin(9600);
  Serial.println("Digite 'a' para aumentar ou 'd' para diminuir.");
}

void loop() {
  if (Serial.available() > 0) {
    char command = Serial.read();

    if (command == 'a' && potValue < 255) {
      digitalWrite(CS_PIN, LOW);
      digitalWrite(U_D_PIN, HIGH);  
      digitalWrite(INC_PIN, LOW);
      delay(1);
      digitalWrite(INC_PIN, HIGH);
      delay(1);
      digitalWrite(CS_PIN, HIGH);
      potValue++;
      Serial.println("Aumentando");
    }
    else if (command == 'd' && potValue > 0) {
      digitalWrite(CS_PIN, LOW);
      digitalWrite(U_D_PIN, LOW);
      digitalWrite(INC_PIN, LOW);
      delay(1);
      digitalWrite(INC_PIN, HIGH);
      delay(1);
      digitalWrite(CS_PIN, HIGH);
      potValue--;
      Serial.println("Diminuindo");
    } else {
      Serial.println("Comando invalido ou limite atingido!");
    }

    lcd.setCursor(0, 1);
    lcd.print("Valor: ");
    lcd.print("    "); 
    lcd.setCursor(7, 1);
    lcd.print(potValue);
  }
}
