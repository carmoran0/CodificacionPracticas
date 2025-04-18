/*
 * Controladora para Sistema de Climatización con ESP32-S3
 * 
 * Hardware:
 * - Sensor de temperatura LM35 conectado a GPIO1
 * - LED Verde (temperatura dentro del rango) en GPIO2
 * - LED Rojo (temperatura baja) en GPIO3
 * - LED Azul (temperatura alta) en GPIO4
 * 
 * Funcionalidad:
 * - Control de temperatura con histéresis
 * - Interfaz por terminal serie
 * - Almacenamiento en EEPROM
 */

#include <EEPROM.h>

// Definición de pines para ESP32-S3
#define PIN_SENSOR_TEMP 1   // GPIO1 (ADC1_CH0)
#define PIN_LED_VERDE 2     // GPIO2
#define PIN_LED_ROJO 3      // GPIO3
#define PIN_LED_AZUL 4      // GPIO4

// Tamaño de la EEPROM
#define EEPROM_SIZE 32

// Direcciones de memoria EEPROM
#define EEPROM_TEMP_ADDR 0
#define EEPROM_HIST_ADDR 4

// Constantes del sistema
#define TEMP_DEFAULT 25.0
#define HIST_DEFAULT 2.0
#define INTERVALO_ACTUALIZACION 1000  // ms

// Variables globales
float temperaturaActual = 0.0;
float temperaturaPrograma = TEMP_DEFAULT;
float histeresis = HIST_DEFAULT;
unsigned long ultimaActualizacion = 0;
bool configuracionInicial = false;

void setup() {
  // Inicialización de la comunicación serial
  Serial.begin(115200);
  
  // Configuración de pines LED
  pinMode(PIN_LED_VERDE, OUTPUT);
  pinMode(PIN_LED_ROJO, OUTPUT);
  pinMode(PIN_LED_AZUL, OUTPUT);
  
  // Apagamos todos los LEDs
  digitalWrite(PIN_LED_VERDE, LOW);
  digitalWrite(PIN_LED_ROJO, LOW);
  digitalWrite(PIN_LED_AZUL, LOW);
  
  // Inicializar EEPROM en ESP32-S3
  EEPROM.begin(EEPROM_SIZE);
  
  // Recuperar datos de EEPROM
  EEPROM.get(EEPROM_TEMP_ADDR, temperaturaPrograma);
  EEPROM.get(EEPROM_HIST_ADDR, histeresis);
  
  // Verificar si hay datos válidos en EEPROM
  if (isnan(temperaturaPrograma) || temperaturaPrograma < -50 || temperaturaPrograma > 100) {
    temperaturaPrograma = TEMP_DEFAULT;
  }
  
  if (isnan(histeresis) || histeresis < 0.1 || histeresis > 10) {
    histeresis = HIST_DEFAULT;
  }
  
  Serial.println("Inicializando sistema de climatización...");
  delay(1000);
  
  // Mostrar menú
  mostrarMenu();
}

void loop() {
  // Comprobar si hay datos disponibles en el puerto serie
  if (Serial.available() > 0) {
    procesarComandoSerial();
  }
  
  // Actualizar lectura de temperatura cada segundo
  if (millis() - ultimaActualizacion >= INTERVALO_ACTUALIZACION) {
    ultimaActualizacion = millis();
    leerTemperatura();
    actualizarControl();
  }
}



void mostrarMenu() {
  Serial.println("\n=== SISTEMA DE CLIMATIZACIÓN ===");
  Serial.println("1 - Fijar la temperatura del termostato");
  Serial.println("2 - Fijar la histéresis del termostato");
  Serial.println("3 - Mostrar la temperatura actual");
  Serial.println("4 - Mostrar la histéresis actual");
  Serial.print("> ");
}

void procesarComandoSerial() {
  char opcion = Serial.read();
  
  switch(opcion) {
    case '1':
      fijarTemperatura();
      break;
    case '2':
      fijarHisteresis();
      break;
    case '3':
      mostrarTemperaturaActual();
      break;
    case '4':
      mostrarHisteresisActual();
      break;
    case '\n':
    case '\r':
      // Ignorar retornos de carro
      break;
    default:
      Serial.println("Opción no válida.");
      mostrarMenu();
  }
}

void fijarTemperatura() {
  Serial.println("Introduce la temperatura deseada (°C):");
  
  while (!Serial.available()) {
    // Esperar entrada del usuario
    delay(100);
  }
  
  float nuevaTemp = Serial.parseFloat();
  
  if (nuevaTemp >= -50 && nuevaTemp <= 100) {
    temperaturaPrograma = nuevaTemp;
    
    // Guardar en EEPROM
    EEPROM.put(EEPROM_TEMP_ADDR, temperaturaPrograma);
    EEPROM.commit(); // Importante en ESP32-S3: guardar los cambios
    
    Serial.print("Temperatura fijada a: ");
    Serial.print(temperaturaPrograma);
    Serial.println(" °C");
    
    configuracionInicial = true;
  } else {
    Serial.println("Valor fuera de rango (-50 a 100 °C)");
  }
  
  mostrarMenu();
}

void fijarHisteresis() {
  Serial.println("Introduce la histéresis deseada (°C):");
  
  while (!Serial.available()) {
    // Esperar entrada del usuario
    delay(100);
  }
  
  float nuevaHist = Serial.parseFloat();
  
  if (nuevaHist >= 0.1 && nuevaHist <= 10) {
    histeresis = nuevaHist;
    
    // Guardar en EEPROM
    EEPROM.put(EEPROM_HIST_ADDR, histeresis);
    EEPROM.commit(); // Importante en ESP32-S3: guardar los cambios
    
    Serial.print("Histéresis fijada a: ");
    Serial.print(histeresis);
    Serial.println(" °C");
    
    configuracionInicial = true;
  } else {
    Serial.println("Valor fuera de rango (0.1 a 10 °C)");
  }
  
  mostrarMenu();
}

void mostrarTemperaturaActual() {
  Serial.print("Temperatura actual: ");
  Serial.print(temperaturaActual);
  Serial.println(" °C");
  mostrarMenu();
}

void mostrarHisteresisActual() {
  Serial.print("Histéresis actual: ");
  Serial.print(histeresis);
  Serial.println(" °C");
  mostrarMenu();
}

void leerTemperatura() {
  // Leer el valor del sensor
  int sensorValue = analogRead(PIN_SENSOR_TEMP);
  
  // ESP32-S3 tiene un ADC de 12 bits (0-4095)
  // LM35 entrega 10mV por cada °C. Con referencia de 3.3V
  temperaturaActual = (sensorValue * 3.3 * 100.0) / 4095.0;
  
  // Opcional: aplicar filtro para eliminar ruido
  // temperaturaActual = (temperaturaActual * 0.9) + (tempAnterior * 0.1);
}

void actualizarControl() {
  if (!configuracionInicial) {
    return;  // No controlar hasta que se haya configurado
  }
  
  // Rango de temperatura aceptable
  float tempMin = temperaturaPrograma - histeresis;
  float tempMax = temperaturaPrograma + histeresis;
  
  // Temperatura dentro del rango
  if (temperaturaActual >= tempMin && temperaturaActual <= tempMax) {
    digitalWrite(PIN_LED_VERDE, HIGH);
    digitalWrite(PIN_LED_ROJO, LOW);
    digitalWrite(PIN_LED_AZUL, LOW);
  }
  // Temperatura alta
  else if (temperaturaActual > tempMax) {
    // Si apenas superamos el límite, encender el led azul
    digitalWrite(PIN_LED_VERDE, LOW);
    digitalWrite(PIN_LED_ROJO, LOW);
    digitalWrite(PIN_LED_AZUL, HIGH);
  }
  // Si bajamos por debajo de la temperatura programada, restaurar led verde
  else if (temperaturaActual <= temperaturaPrograma && digitalRead(PIN_LED_AZUL) == HIGH) {
    digitalWrite(PIN_LED_VERDE, HIGH);
    digitalWrite(PIN_LED_ROJO, LOW);
    digitalWrite(PIN_LED_AZUL, LOW);
  }
  // Temperatura baja
  else if (temperaturaActual < tempMin) {
    // Si apenas bajamos del límite, encender el led rojo
    digitalWrite(PIN_LED_VERDE, LOW);
    digitalWrite(PIN_LED_ROJO, HIGH);
    digitalWrite(PIN_LED_AZUL, LOW);
  }
  // Si subimos por encima de la temperatura programada, restaurar led verde
  else if (temperaturaActual >= temperaturaPrograma && digitalRead(PIN_LED_ROJO) == HIGH) {
    digitalWrite(PIN_LED_VERDE, HIGH);
    digitalWrite(PIN_LED_ROJO, LOW);
    digitalWrite(PIN_LED_AZUL, LOW);
  }
}