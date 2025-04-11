#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <SPIFFS.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>

const char *ssid = "tu wifi";
const char *password = "tu contraseña wifi";

#define PIR_PIN 2
#define BUTTON_PIN 4
#define SPEAKER_PIN 5
#define LED_PIN 15

LiquidCrystal_I2C lcd(0x27, 16, 2);
AsyncWebServer server(80);

bool pirState = false;
bool ledState = false;
bool alarmState = false;
bool pirActivated = false;
bool prevPirState = false;

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
    if (pirActivated && pirState) {
      lcd.setCursor(0, 0);
      lcd.print("PIR: ON ");
    } else if (pirActivated && !pirState) {
      lcd.setCursor(0, 0);
      lcd.print("PIR: OFF");
    }
    request->send(200, "text/plain", pirState ? "Detectado" : "No detectado");
  });

  server.on("/enviar-archivo", HTTP_GET, [](AsyncWebServerRequest *request){
    enviarArchivo();
    request->send(200, "text/plain", "Archivo enviado");
  });

  server.begin();
  
  obtenerDatos();
}

void loop() {
  pirState = digitalRead(PIR_PIN);

  if (pirState && pirActivated) {
    if (!alarmState) {
      digitalWrite(SPEAKER_PIN, HIGH);
      alarmState = true;
      enviarDatos(1, 0); // Enviar valor 1 al servidor solo cuando se detecte movimiento
    }
    digitalWrite(LED_PIN, HIGH);
  } else {
    if (alarmState) {
    digitalWrite(SPEAKER_PIN, LOW);
    alarmState = false;
    digitalWrite(LED_PIN, LOW);
    enviarDatos(0, 0);
   } // Enviar valor 0 al servidor cuando no se detecte movimiento
  }

  if (pirActivated && pirState != prevPirState) {
    if (pirState) {
      lcd.setCursor(0, 0);
      lcd.print("PIR: ON ");
    } else {
      lcd.setCursor(0, 0);
      lcd.print("PIR: OFF");
    }
  }
  
  prevPirState = pirState;

  if (digitalRead(BUTTON_PIN) == LOW) {
    pirActivated = !pirActivated;
    if (!pirActivated) {
      digitalWrite(PIR_PIN, LOW);
      lcd.setCursor(0, 1);
      lcd.print("PIR desactivado");
      enviarDatos(0, 0); // Enviar valor 0 al servidor cuando el PIR se desactiva y el LED se apaga
    } else {
      lcd.setCursor(0, 1);
      lcd.print("PIR activado   ");
      enviarDatos(1, 1); // Enviar valor 1 al servidor cuando el PIR se activa y el LED se enciende
    }
     delay(500); // Pequeña pausa para evitar el rebote del botón
  }

  delay(1000);
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

void enviarDatos(int pirValue, int ledValue) {
  HTTPClient http;

  String apiUrl = "tu api/sendData";
  http.begin(apiUrl);
  http.addHeader("Content-Type", "application/json");

  StaticJsonDocument<256> jsonDocument;
  jsonDocument["ledState"] = ledValue; // Estado del LED
  jsonDocument["buttonState"] = 0; // Estado del botón (siempre 0 en este contexto)
  jsonDocument["pirState"] = pirValue; // Estado del sensor PIR
  
  String jsonString;
  serializeJson(jsonDocument, jsonString);

  int httpResponseCode = http.POST(jsonString);

  if (httpResponseCode > 0) {
    Serial.printf("[HTTP] POST exitoso, código de respuesta: %d\n", httpResponseCode);
    String payload = http.getString();
    Serial.println(payload);
  } else {
    Serial.printf("[HTTP] Error al enviar la solicitud, código de error: %d\n", httpResponseCode);
  }

  http.end();
}


void obtenerDatos() {
  HTTPClient http;

  String apiUrl = "tu api/getData";
  http.begin(apiUrl);

  int httpResponseCode = http.GET();

  if (httpResponseCode == 200) {
    String payload = http.getString();
    Serial.println("Respuesta del servidor:");
    Serial.println(payload);
  } else {
    Serial.print("Error al hacer la solicitud. Código de respuesta: ");
    Serial.println(httpResponseCode);
  }

  http.end();
}

void enviarArchivo() {
  Serial.println("Enviando archivo...");
  
  if (SPIFFS.begin()) {
    Serial.println("SPIFFS inicializado correctamente");
  } else {
    Serial.println("Error al inicializar SPIFFS");
    return;
  }

  // Leer el archivo desde SPIFFS
  File file = SPIFFS.open("/archivo.txt", "r");
  if (!file) {
    Serial.println("Error al abrir el archivo");
    return;
  }

  // Obtener el tamaño del archivo
  size_t fileSize = file.size();

  // Crear un búfer para almacenar el contenido del archivo
  std::unique_ptr<char[]> buf(new char[fileSize + 1]); // Agregar espacio para el carácter nulo final
  if (!buf) {
    Serial.println("Error al asignar memoria para el búfer");
    file.close();
    return;
  }

  // Leer el contenido del archivo en el búfer
  file.readBytes(buf.get(), fileSize);

  // Agregar un carácter nulo al final del búfer
  buf[fileSize] = '\0';

  // Cerrar el archivo
  file.close();

  // Crear un cliente HTTP
  HTTPClient http;

  // Configurar la URL de la API y el método HTTP
  String apiUrl = "tu api/sendData";
  http.begin(apiUrl);
  http.addHeader("Content-Type", "text/plain");

  // Enviar el contenido del archivo como parte de la solicitud POST
  int httpResponseCode = http.POST(buf.get());

  // Verificar la respuesta de la API
  if (httpResponseCode > 0) {
    Serial.printf("[HTTP] POST exitoso, código de respuesta: %d\n", httpResponseCode);
    String payload = http.getString();
    Serial.println(payload);
  } else {
    Serial.printf("[HTTP] Error al enviar la solicitud, código de error: %d\n", httpResponseCode);
  }

  // Cerrar la conexión HTTP
  http.end();
}