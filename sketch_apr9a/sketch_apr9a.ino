#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include <BluetoothSerial.h>

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run make menuconfig to and enable it
#endif

BluetoothSerial SerialBT;



#define PIR_PIN 2        // Pin del sensor PIR
#define BUTTON_PIN 4     // Pin del botón de encendido/apagado
#define SPEAKER_PIN 5    // Pin del altavoz

char CharIN = ' ';

// Inicializar el objeto de la pantalla LCD I2C
LiquidCrystal_I2C lcd(0x27, 16, 2);

bool pirState = false;

void setup() {
  Serial.begin(115200);

  pinMode(PIR_PIN, INPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(SPEAKER_PIN, OUTPUT);
  SerialBT.begin("Esp32_test"); //Bluetooth device name
  Serial.printf("The device with name \"%s\" is started, Now you can pair it with Bluetooth!");


   Serial.println("iniciando Control ...");

  lcd.init();
  lcd.begin(16, 2);  // Inicializar la pantalla LCD
  lcd.backlight();   // Encender la retroiluminación
  
}

void loop() {
  if (Serial.available()) {
    SerialBT.write(Serial.read());
  }
  if (SerialBT.available()) {
    Serial.write(SerialBT.read());
  }
  delay(20);

  lcd.setCursor(0, 1);  
    
  if (pirState == false){
    if (digitalRead(2) == HIGH) {
       lcd.clear();
       lcd.setCursor(0, 1);
        lcd.print("Detectado ");
        delay(500);
        triggerAlarm();
    
     } else {
       lcd.setCursor(0, 1);
       lcd.print("No detectado ");
       delay(500);
     }
  }
  if (!pirState && digitalRead(BUTTON_PIN) == LOW){
      pirState = true;
      lcd.setCursor(0, 0);
      lcd.print("Pir OFF");
      delay(100);
  }else{
    if(pirState && digitalRead(BUTTON_PIN) == LOW){
      pirState = false;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Pir On");
      delay(100);
    }
  }

 if(SerialBT.available()) {
       CharIN = SerialBT.read();
       Serial.print(CharIN);
       OnOffPIR();
        
      Serial.println();
  }


  delay(10);
     
}

void OnOffPIR(void) {
 if (CharIN == 'A'){pirState = false;
 lcd.clear();
 lcd.setCursor(0, 0);
lcd.print("Pir ON");}
 if (CharIN == 'B'){pirState = true;
lcd.setCursor(0, 0);
lcd.print("Pir OFF");}
}

void triggerAlarm() {
  tone(SPEAKER_PIN, 1000, 1000); 
  delay(2000);
}