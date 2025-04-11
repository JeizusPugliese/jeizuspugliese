const int sensorPin = A0; // Pin del sensor conectado al pin A0
int valorSensor;          // Variable para almacenar la lectura del sensor
unsigned long tiempoAnterior = 0; // Tiempo previo para calcular BPM
int latidos = 0;          // Contador de latidos
bool pulsoDetectado = false;

void setup() {
  Serial.begin(9600);       // Inicializar comunicación serial
  pinMode(sensorPin, INPUT); // Configurar el pin como entrada
}

void loop() {
  valorSensor = analogRead(sensorPin); // Leer valor del sensor
  unsigned long tiempoActual = millis();

  // Mostrar el valor en el monitor serial
  Serial.println(valorSensor);

  // Detectar un latido basado en un umbral
  if (valorSensor > 512 && !pulsoDetectado) { // Ajusta el umbral según tu sensor
    pulsoDetectado = true;
    latidos++;
  }

  if (valorSensor < 512 && pulsoDetectado) {
    pulsoDetectado = false;
  }

  // Calcular BPM (latidos por minuto) cada 10 segundos
  if (tiempoActual - tiempoAnterior >= 10000) { 
    int bpm = (latidos * 6); // Convertir a BPM
    Serial.print("Latidos por minuto (BPM): ");
    Serial.println(bpm);
    tiempoAnterior = tiempoActual;
    latidos = 0; // Reiniciar contador de latidos
  }

  delay(10); // Pequeño retraso para estabilizar las lecturas
}
