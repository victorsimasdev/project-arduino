#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>

#define INC_PIN 6  
#define U_D_PIN 7  
#define CS_PIN 8   
#define MOTOR_PIN 10 

LiquidCrystal_I2C lcd(0x27, 16, 2); 

int potValue = 0;    
int valueOLD = 0;    

void setup() {
  potValue = EEPROM.read(0);
  valueOLD = potValue;

  pinMode(INC_PIN, OUTPUT);
  pinMode(U_D_PIN, OUTPUT);
  pinMode(CS_PIN, OUTPUT);
  pinMode(MOTOR_PIN, OUTPUT); 

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
  Serial.println("Digite 'a' para aumentar, 'd' para diminuir, ou um valor entre 0 e 100.");


  analogWrite(MOTOR_PIN, potValue);
}

void ajustarPotenciometro(int novoValor) {
  int valueNEW = map(novoValor, 0, 100, 0, 255);

  if (valueNEW > valueOLD) {
    digitalWrite(U_D_PIN, HIGH);
  } else if (valueNEW < valueOLD) {
    digitalWrite(U_D_PIN, LOW);
  }

  for (int i = 0; i < abs(valueNEW - valueOLD); i++) {
    digitalWrite(CS_PIN, LOW);
    digitalWrite(INC_PIN, LOW);
    delay(10);
    digitalWrite(INC_PIN, HIGH);
    delay(10);
    digitalWrite(CS_PIN, HIGH);
  }

  valueOLD = valueNEW;
  potValue = valueNEW;

  EEPROM.write(0, potValue);

  lcd.setCursor(0, 1);
  lcd.print("Valor: ");
  lcd.print("    "); 
  lcd.setCursor(7, 1);
  lcd.print(potValue);

  Serial.print("Novo valor ajustado: ");
  Serial.println(novoValor);

  analogWrite(MOTOR_PIN, potValue);  
}

void loop() {
  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');
    input.trim(); 

    if (input == "a" && potValue < 255) {
      ajustarPotenciometro(map(potValue + 1, 0, 255, 0, 100));
      Serial.println("Aumentando");
    } else if (input == "d" && potValue > 0) {
      ajustarPotenciometro(map(potValue - 1, 0, 255, 0, 100));
      Serial.println("Diminuindo");
    } else {
      int novoValor = input.toInt();
      if (novoValor >= 0 && novoValor <= 100) {
        ajustarPotenciometro(novoValor);
      } else {
        Serial.println("Entrada invalida! Digite 'a', 'd', ou um valor entre 0 e 100.");
      }
    }
  }
}
