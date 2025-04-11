#define BLYNK_TEMPLATE_ID "TMPL2jfgjzm8v"
#define BLYNK_TEMPLATE_NAME "esp32 iot"
#define BLYNK_AUTH_TOKEN "Vug-94FxIi8kYsEYYBdUlW4UXEHcfFHj"
#define BLYNK_AUTH_TOKEN "fql0OQMLx9SQNi-TJfT8G3-qlf4JXQNF"

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>

#define PIR_PIN 2        // Pin del sensor PIR
#define BUTTON_PIN 4     // Pin del botón de encendido/apagado
#define SPEAKER_PIN 5    // Pin del altavoz
#define LED_PIN 12       // Pin del LED

char ssid[] = "Familia Pc";     // Nombre de tu red WiFi
char pass[] = "1305303011";  // Contraseña de tu red WiFi

// Inicializar el objeto de la pantalla LCD I2C
LiquidCrystal_I2C lcd(0x27, 16, 2);

volatile bool pirState = false;
volatile bool pirEnabled = true;

void setup() {
  Serial.begin(115200);

  pinMode(PIR_PIN, INPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(SPEAKER_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);

  lcd.init();
  lcd.begin(16, 2);  // Inicializar la pantalla LCD
  lcd.backlight();   // Encender la retroiluminación
  lcd.print("Connecting...");

  // Conexión a la red WiFi
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  // Esperar hasta que se establezca la conexión WiFi
  while (Blynk.connected() == false) {
    delay(500);
  }

  lcd.clear();
  lcd.print("Connected to WiFi");
  delay(2000); // Mostrar el mensaje durante 2 segundos
  lcd.clear();
  lcd.print("PIR: ON");
  
  attachInterrupt(digitalPinToInterrupt(PIR_PIN), pirInterrupt, CHANGE);
}

void loop() {
  Blynk.run(); // Esta función es necesaria para que Blynk funcione correctamente
  lcd.setCursor(0, 1);  // Configurar la posición del cursor en la segunda línea

  if (pirEnabled) {
    lcd.setCursor(0, 1);
    lcd.print("move: ");
    lcd.print(pirState ? "movement detected " : "No movement ");
    if (pirState) {
      triggerAlarm();
      digitalWrite(LED_PIN, HIGH); // Encender LED cuando se detecta movimiento
    } else {
      digitalWrite(LED_PIN, LOW); // Apagar LED cuando no hay movimiento
    }
  } else {
    digitalWrite(LED_PIN, LOW); // Si el PIR está deshabilitado, asegurarse de que el LED esté apagado
  }

  delay(500);  // Pequeño retraso para evitar parpadeo en la pantalla

  if (digitalRead(BUTTON_PIN) == LOW) {
    delay(50);  // Debounce
    pirEnabled = !pirEnabled;
    lcd.clear();  // Limpiar la pantalla al cambiar el estado del PIR
    if (pirEnabled) {
      lcd.print("PIR: ON");
    } else {
      lcd.print("PIR: OFF");
    }
    delay(500); // Evitar múltiples cambios con un solo clic
  }
}

void pirInterrupt() {
  pirState = digitalRead(PIR_PIN);
}

void triggerAlarm() {
  tone(SPEAKER_PIN, 1000, 1000);  // Frecuencia y duración del sonido
  delay(2000);  // Espera antes de volver a detectar movimiento
}