#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include "ThingSpeak.h"

// Define el pin donde está conectado el PIR y el zumbador
#define PIRPIN 27
#define BUZZERPIN 4

// Define el pin donde está conectado el DHT11 en el ESP32
#define DHTPIN 23 

// Define el tipo de sensor DHT11
#define DHTTYPE DHT11 

// Define los pines para el sensor de luz (LDR)
#define LDRPIN 34  // Cambiado a un pin ADC compatible
#define LEDPIN 2

// Define el pin para el control del motor y el zumbador para el gas
#define MOTORPIN 14

// Define el pin del sensor de gas
#define GAS_SENSOR_PIN 35  // Cambiado a un pin ADC compatible
#define BUZZERGASPIN 25

// Umbral para activar el zumbador del sensor de gas
#define GAS_THRESHOLD 2200

// Inicializa el LCD I2C: dirección 0x27, 16 columnas y 2 filas
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Crea una instancia del sensor DHT
DHT dht(DHTPIN, DHTTYPE);

// Credenciales de WiFi
const char* ssid = "Familia Pc";
const char* password = "1305303011";

// Configuración de ThingSpeak
unsigned long myChannelNumber = 2548628;
const char * myWriteAPIKey = "7OEEFMPD0DGI2BV7";

WiFiClient client;
IPAddress serverIP(192, 168, 1, 4); // Cambia esta dirección IP por la de tu servidor local

// Variable global para el estado del PIR
int estadoPIR = 0;  // Inicialmente apagado

void setup() {
  // Inicializa la comunicación serie
  Serial.begin(115200);
  
  // Inicializa el LCD
  lcd.init();
  lcd.backlight();

  // Configura los pines
  pinMode(PIRPIN, INPUT);
  pinMode(BUZZERPIN, OUTPUT);
  pinMode(LEDPIN, OUTPUT);
  pinMode(BUZZERGASPIN, OUTPUT);
  pinMode(MOTORPIN, OUTPUT);
  pinMode(GAS_SENSOR_PIN, INPUT);
  pinMode(LDRPIN, INPUT);
  digitalWrite(PIRPIN, LOW);

  // Inicializa el sensor DHT
  dht.begin();

  // Mensaje inicial en el LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Luz:Off");
  lcd.setCursor(9, 0);
  lcd.print("PIR:Off");

  // Conexión WiFi
  WiFi.begin(ssid, password);
  Serial.print("Conectando a WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("Conectado!");

  // Inicializa ThingSpeak
  ThingSpeak.begin(client);
}

void cambiarEstadoPIR(int estado) {
  estadoPIR = estado;
  if (estadoPIR == 1) {
    digitalWrite(PIRPIN, HIGH);
  } else {
    digitalWrite(PIRPIN, LOW);
  }
}

void loop() {
  // Manejo del PIR desde la aplicación móvil
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    // Comprobar el estado del PIR y enviar la petición
    if (estadoPIR == 1) {
      http.begin("http://" + serverIP.toString() + "/togglePIR");
      http.addHeader("Content-Type", "application/json");
      String jsonData = "{\"command\":\"pir_on\"}";
      int httpResponseCode = http.POST(jsonData);
      if (httpResponseCode == 200) {
        Serial.println("Comando de encendido de PIR enviado");
      } else {
        Serial.print("Error al enviar comando de encendido de PIR: ");
        Serial.println(httpResponseCode);
      }
    } else if (estadoPIR == 0) {
      http.begin("http://" + serverIP.toString() + "/togglePIR");
      http.addHeader("Content-Type", "application/json");
      String jsonData = "{\"command\":\"pir_off\"}";
      int httpResponseCode = http.POST(jsonData);
      if (httpResponseCode == 200) {
        Serial.println("Comando de apagado de PIR enviado");
      } else {
        Serial.print("Error al enviar comando de apagado de PIR: ");
        Serial.println(httpResponseCode);
      }
    }

    http.end();
  }

  // Leer el estado del sensor PIR
  int estadoPIR = digitalRead(PIRPIN);
  
  // Actualizar el estado del PIR en el LCD
  if (estadoPIR == HIGH) {
    lcd.setCursor(9, 0);
    lcd.print("PIR:On ");
    Serial.println("Movimiento detectado");
    // Activar la alarma (zumbador)
    digitalWrite(BUZZERPIN, HIGH);
  } else {
    lcd.setCursor(9, 0);
    lcd.print("PIR:Off");
    Serial.println("No hay movimiento");
    // Desactivar la alarma (zumbador)
    digitalWrite(BUZZERPIN, LOW);
  }

  // Espera un segundo antes de volver a leer el PIR
  delay(1000);

  // Lee la temperatura en grados Celsius (por defecto)
  float temperatura = dht.readTemperature();
  // Lee la humedad relativa
  float humedad = dht.readHumidity();

  // Verifica si las lecturas de temperatura y humedad son válidas
  if (isnan(temperatura) || isnan(humedad)) {
    Serial.println("Fallo al leer del DHT11!");
    lcd.setCursor(0, 1);
    lcd.print("                "); // Limpia la línea completa
    lcd.setCursor(0, 1);
    lcd.print("Temp: Err");
  } else {
    // Imprime la temperatura y la humedad en el monitor serie
    Serial.print("Temperatura: ");
    Serial.print(temperatura);
    Serial.println(" *C");
    Serial.print("Humedad: ");
    Serial.print(humedad);
    Serial.println(" %");
    // Actualiza la temperatura en el LCD
    lcd.setCursor(0, 1);
    lcd.print("                "); // Limpia la línea completa
    lcd.setCursor(0, 1);
    lcd.print("Temp: ");
    lcd.print(temperatura);
    lcd.print(" C ");
  }

  // Control del motor basado en la temperatura
  if (temperatura >= 40) {
    // Motor al 100% de potencia
    analogWrite(MOTORPIN, 255);
  } else if (temperatura >= 35) {
    // Motor al 50% de potencia
    analogWrite(MOTORPIN, 128);
  } else if (temperatura >= 25) {
    // Motor al 25% de potencia
    analogWrite(MOTORPIN, 64);
  } else {
    // Motor apagado
    analogWrite(MOTORPIN, 0);
  }

  // Lee el valor del sensor de luz (LDR)
  int valorLuz = analogRead(LDRPIN);
  
  // Imprime el valor de luz en el monitor serie
  Serial.print("Luz: ");
  Serial.println(valorLuz);

  // Verifica el umbral de luz para encender o apagar el LED
  if (valorLuz > 500) { // Ajusta este valor de umbral según sea necesario
    // Si no hay suficiente luz, enciende el LED
    digitalWrite(LEDPIN, HIGH);
    // Actualiza el estado de la luz en el LCD
    lcd.setCursor(0, 0);
    lcd.print("Luz:On ");
  } else {
    // Si hay suficiente luz, apaga el LED
    digitalWrite(LEDPIN, LOW);
    // Actualiza el estado de la luz en el LCD
    lcd.setCursor(0, 0);
    lcd.print("Luz:Off");
  }

  // Lee el valor del sensor de gas
  int valorGas = analogRead(GAS_SENSOR_PIN);

  // Imprime el valor del sensor de gas en el monitor serie
  Serial.print("Gas: ");
  Serial.println(valorGas);

  // Controla el zumbador de gas basado en el sensor de gas
  if (valorGas > GAS_THRESHOLD) {
    // Si el valor del gas es alto, enciende el zumbador de gas
    digitalWrite(BUZZERGASPIN, HIGH);
  } else {
    // Si el valor del gas es bajo, apaga el zumbador de gas
    digitalWrite(BUZZERGASPIN, LOW);
  }

  // Enviar datos a ThingSpeak
  ThingSpeak.setField(1, temperatura);
  ThingSpeak.setField(2, humedad);
  ThingSpeak.setField(3, valorGas);
  ThingSpeak.setField(4, valorLuz);
  ThingSpeak.setField(5, estadoPIR);
  
  int response = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  if(response == 200) {
    Serial.println("Canal actualizado correctamente");
  } else {
    Serial.print("Error al actualizar canal. Código de error: ");
    Serial.println(response);
  }

  // Enviar datos a la API
  if(WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin("http://192.168.1.4:5000/sendDato");
    http.addHeader("Content-Type", "application/json");

    String jsonData = String("{\"temperature\":") + temperatura +
                      String(", \"humidity\":") + humedad +
                      String(", \"pir_state\":") + estadoPIR +
                      String(", \"gas\":") + valorGas +
                      String(", \"light\":") + valorLuz + String("}");

    int httpResponseCode = http.POST(jsonData);

    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.print("Respuesta de la petición HTTP: ");
      Serial.println(httpResponseCode);
      Serial.println(response);
    } else {
      Serial.print("Error en la petición HTTP: ");
      Serial.println(httpResponseCode);
    }

    http.end();
  }

  // Espera dos segundos antes de volver a leer el DHT11, LDR y sensor de gas
  delay(2000);
}