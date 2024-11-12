#include <Wire.h>
#include <LiquidCrystal_I2C.h>

const int potPin = A0;    
const int potPin2 = A1;    
const int motorPin = 9;
const int motorPin2 = 10;

LiquidCrystal_I2C lcd(32, 16, 2);

void setup() {
  pinMode(motorPin, OUTPUT);
  pinMode(motorPin2, OUTPUT);
  Serial.begin(9600);        
  
  lcd.init();
  lcd.setCursor(0, 0);
  lcd.backlight();
  lcd.display();
  
  lcd.print("Inicializando...");
  delay(2000);
  lcd.clear();
}

void loop() {
  int potValue = analogRead(potPin);  
  int motorSpeed = map(potValue, 0, 1023, 0, 255); 
  analogWrite(motorPin, motorSpeed);
    
  lcd.setCursor(0, 0);
  lcd.print("Vel. M1: ");
  lcd.print(motorSpeed);
  
  
  int potValue2 = analogRead(potPin2);  
  int motorSpeed2 = map(potValue2, 0, 1023, 0, 255); 
  analogWrite(motorPin2, motorSpeed2);
  
  lcd.setCursor(0, 1);
  lcd.print("Vel. M2: ");
  lcd.print(motorSpeed2);

  delay(1000);
}
