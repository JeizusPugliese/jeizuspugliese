#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <SPIFFS.h>
#include <ESPAsyncWebServer.h> // Agregado para manejar solicitudes HTTP de forma asíncrona
#include <ArduinoJson.h>

// Definiciones de red Wi-Fi
const char *ssid = "RED OCULTA";
const char *password = "C4o0Ti|O";

// Pines GPIO
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
  
  // Configurar pines
  pinMode(PIR_PIN, INPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(SPEAKER_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  
  Serial.println("Iniciando Control ...");
  lcd.init();
  lcd.begin(16, 2);
  lcd.backlight();
  
  connectToWiFi(); // Conectar a la red Wi-Fi

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

  server.on("/enviar-archivo", HTTP_GET, [](AsyncWebServerRequest *request){
    enviarArchivo(); // Función para enviar un archivo
    request->send(200, "text/plain", "Archivo enviado");
  });

  server.begin(); // Iniciar el servidor web
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
  String apiUrl = "http://192.168.1.4:5000/sendData";
  http.begin(apiUrl);
  http.addHeader("Content-Type", "text/plain");

  // Enviar el contenido del archivo como parte de la solicitud POST
  int httpResponseCode = http.POST(reinterpret_cast<uint8_t*>(buf.get()), fileSize);

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
