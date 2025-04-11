#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h> // Agregado para manejar solicitudes HTTP de forma asíncrona
#include <ArduinoJson.h>

const char *ssid = "RED OCULTA";
const char *password = "C4o0Ti|O";
#define PIR_PIN 2
#define BUTTON_PIN 4
#define SPEAKER_PIN 5
#define LED_PIN 6

LiquidCrystal_I2C lcd(0x27, 16, 2);
AsyncWebServer server(80); // Instancia del servidor web en el puerto 80

bool pirState = false;
bool ledState = false;

void setup() {
  Serial.begin(115200);
  pinMode(PIR_PIN, INPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(SPEAKER_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  Serial.println("Iniciando Control ...");
  lcd.init();
  lcd.begin(16, 2);
  lcd.backlight();
  connectToWiFi();

  // Configuración de las rutas del servidor web
  server.on("/led/on", HTTP_GET, [](AsyncWebServerRequest *request){
    ledState = true;
    digitalWrite(LED_PIN, HIGH);
    request->send(200, "text/plain", "LED encendido");
  });

  server.on("/led/off", HTTP_GET, [](AsyncWebServerRequest *request){
    ledState = false;
    digitalWrite(LED_PIN, LOW);
    request->send(200, "text/plain", "LED apagado");
  });

  server.on("/pir", HTTP_GET, [](AsyncWebServerRequest *request){
    pirState = digitalRead(PIR_PIN);
    request->send(200, "text/plain", pirState ? "Detectado" : "No detectado");
  });

  server.begin();
}

void loop() {
  // No se necesita un loop, el servidor web se maneja de forma asíncrona
}

void connectToWiFi() {
  Serial.println("Conectando a WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(250);
  }
  Serial.println("WiFi conectado");
  Serial.println("Dirección IP: ");
  Serial.println(WiFi.localIP());
}