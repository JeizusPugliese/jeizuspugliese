#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Definición de pines
const int led1Pin = 13;
const int led2Pin = 12;
const int switch1Pin = 27;
const int switch2Pin = 26;
const int switch3Pin = 25;
const int relay = 34;


// LCD con dirección corregida (ajústala si el escáner muestra otro valor)
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
    pinMode(led1Pin, OUTPUT);
    pinMode(led2Pin, OUTPUT);
    pinMode(relay, OUTPUT);
    pinMode(switch1Pin, INPUT_PULLUP);
    pinMode(switch2Pin, INPUT_PULLUP);
    pinMode(switch3Pin, INPUT_PULLUP);

    lcd.init();          // Inicializar LCD
    lcd.backlight();     // Encender luz de fondo

    Serial.begin(115200);
}

void loop() {
    bool switch1State = digitalRead(switch1Pin);
    bool switch2State = digitalRead(switch2Pin);
    bool switch3State = digitalRead(switch3Pin);

    digitalWrite(led1Pin, !switch1State);
    digitalWrite(led2Pin, !switch2State);
    digitalWrite(relay, !switch3State);

    lcd.setCursor(0, 1);
    lcd.print("LED1:");
    lcd.print(switch1State ? "OFF " : "ON  ");
    lcd.setCursor(0, 0);
    lcd.print("LED2:");
    lcd.print(switch2State ? "OFF " : "ON  ");
    lcd.setCursor(9, 0);
    lcd.print("RL:");
    lcd.print(switch3State ? "OFF " : "ON  ");

    delay(100);  // Pequeño retardo para evitar parpadeo
}
