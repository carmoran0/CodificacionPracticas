#include <Preferences.h> // Librería para NVS (almacenamiento no volátil) en ESP32
#include "SevSeg.h"        // Librería SevSeg

// --- Definiciones de Pines (¡Según tus especificaciones!) ---
// !!! VERIFICA QUE ESTOS PINES SON CORRECTOS Y ADECUADOS EN TU PLACA ESP32-S3 !!!
// ESPECIALMENTE PIN_SENSOR_TEMP - DEBE SER UN PIN CON CAPACIDAD ADC UTILIZABLE
#define PIN_SENSOR_TEMP 1   // GPIO1 (ADC1_CH0) - Confirma que este pin tiene capacidad ADC en tu placa
#define PIN_LED_VERDE 2     // GPIO2
#define PIN_LED_ROJO 3      // GPIO3
#define PIN_LED_AZUL 4      // GPIO4

// Pines para el display de 7 segmentos
// Segmentos (en orden A,B,C,D,E,F,G,DP)
#define PIN_SEG_A 17
#define PIN_SEG_B 16
#define PIN_SEG_C 13
#define PIN_SEG_D 11
#define PIN_SEG_E 10
#define PIN_SEG_F 9
#define PIN_SEG_G 14
#define PIN_SEG_DP 12

// Pines para los dígitos (cátodos comunes)
#define PIN_DIGITO_1 7  // Dígito 1 (más a la izquierda)
#define PIN_DIGITO_2 8  // Dígito 2
#define PIN_DIGITO_3 18 // Dígito 3
#define PIN_DIGITO_4 15 // Dígito 4 (más a la derecha)

// --- Definiciones para el Display de 4 dígitos y 7 segmentos ---
const byte numDigits = 4; // Número de dígitos en tu display
const byte digitPins[] = {PIN_DIGITO_1, PIN_DIGITO_2, PIN_DIGITO_3, PIN_DIGITO_4};
const byte segmentPins[] = {PIN_SEG_A, PIN_SEG_B, PIN_SEG_C, PIN_SEG_D, PIN_SEG_E, PIN_SEG_F, PIN_SEG_G, PIN_SEG_DP};

bool resistorOnSegments = true; // true si tienes resistencias en los segmentos (recomendado)
byte hardwareConfig = COMMON_CATHODE; // Tipo de display: CATODO COMUN

// --- Variables Globales ---
float temp_programada = 22.0;  // Temperatura del termostato (valor por defecto)
float hysteresis = 1.0;        // Histéresis (valor por defecto)
bool sistema_configurado = false; // Flag para saber si el sistema ya está configurado
float ultima_temp_leida = 0;      // Almacena la última temperatura leída

enum LED_STATE { STATE_GREEN, STATE_RED, STATE_BLUE }; // Estados para el control de LEDs
static LED_STATE estado_actual = STATE_GREEN;          // Estado inicial del LED

SevSeg sevseg; // Objeto para manejar el display
Preferences preferences; // Objeto para manejar NVS (Preferencias)

// Variables para controlar la alternancia en el display
unsigned long previousMillis_display = 0;
const long interval_display = 5000; // Alternar cada 5 segundos
bool showing_programada = false;    // Bandera para saber qué se está mostrando

// Variables para manejo de intervalo de lectura
unsigned long previousMillis_lectura = 0;
const long interval_lectura = 4000; // Actualiza la lectura cada 4 segundos

// --- Función para leer la temperatura del LM35 ---
float leerTemperaturaLM35() {
  // Tomar varias muestras para mejorar la precisión
  int suma = 0;
  const int muestras = 10;
  
  for (int i = 0; i < muestras; i++) {
    suma += analogRead(PIN_SENSOR_TEMP);
    delay(2);
  }
  
  int valor_promedio = suma / muestras;
  
  // Convertir el valor analógico a voltaje (3.3V ref y 12 bits)
  float voltage = valor_promedio * (3.3 / 4095.0);
  // Convertir voltaje a temperatura para LM35 (10mV/°C)
  float temperatura = voltage * 100.0;
  
  return temperatura;
}

// Determina el estado del LED basado en la temperatura teniendo en cuenta la histéresis
// Esta función simplemente calcula qué LED debería estar encendido
LED_STATE calcularEstadoLED(float temperatura) {
  // Cálculo directo basado en la temperatura y la histéresis configurada
  float umbral_caliente = temp_programada + hysteresis;
  float umbral_frio = temp_programada - hysteresis;

  // Log para depuración
  Serial.print("Temp: "); Serial.print(temperatura);
  Serial.print(", Programada: "); Serial.print(temp_programada);
  Serial.print(", Histéresis: "); Serial.print(hysteresis);
  Serial.print(", Umbral frío: "); Serial.print(umbral_frio);
  Serial.print(", Umbral caliente: "); Serial.println(umbral_caliente);

  if (temperatura < umbral_frio) {
    return STATE_RED;    // Temperatura por debajo del umbral frío -> Calefacción (LED ROJO)
  } else if (temperatura > umbral_caliente) {
    return STATE_BLUE;   // Temperatura por encima del umbral caliente -> Refrigeración (LED AZUL)
  } else {
    return STATE_GREEN;  // Temperatura en el rango adecuado -> Confort (LED VERDE)
  }
}

// Función para controlar los LEDs según la lógica con histéresis
void controlarLEDs(float temperatura, bool mostrar_programada) {
  // Apagar todos los LEDs primero
  digitalWrite(PIN_LED_VERDE, LOW);
  digitalWrite(PIN_LED_ROJO, LOW);
  digitalWrite(PIN_LED_AZUL, LOW);

  // Si estamos mostrando la temperatura programada, encender el LED verde
  // indicando que estamos en modo de configuración
  if (mostrar_programada) {
    digitalWrite(PIN_LED_VERDE, HIGH);
    return;
  }
  
  // Calcular el estado del LED basado en la temperatura actual
  estado_actual = calcularEstadoLED(temperatura);
  
  // Encender el LED correspondiente al estado actual
  switch (estado_actual) {
    case STATE_GREEN:
      digitalWrite(PIN_LED_VERDE, HIGH);
      break;
    case STATE_RED:
      digitalWrite(PIN_LED_ROJO, HIGH);
      break;
    case STATE_BLUE:
      digitalWrite(PIN_LED_AZUL, HIGH);
      break;
  }
  
  // Log para depuración
  Serial.print("Estado LED: ");
  if (estado_actual == STATE_GREEN) Serial.println("VERDE");
  else if (estado_actual == STATE_RED) Serial.println("ROJO");
  else Serial.println("AZUL");
}

// Función para mostrar el menú en la terminal serie
void mostrarMenu() {
  Serial.println("\n--- Menu Controlador Climatizacion ESP32-S3 ---");
  Serial.println("1 - Fijar temperatura del termostato");
  Serial.println("2 - Fijar histeresis del termostato");
  Serial.println("3 - Mostrar temperatura actual");
  Serial.println("4 - Mostrar histeresis actual");
  Serial.println("--------------------------------------");
  Serial.print("Ingrese opcion: ");
}

// Función para manejar los comandos seriales
void manejarComandoSerial() {
  if (Serial.available() > 0) {
    char opcion = Serial.read();
    
    // Limpiar buffer de entrada
    while (Serial.available() > 0) {
      Serial.read();
    }

    Serial.print("Opcion seleccionada: ");
    Serial.println(opcion);

    float nuevo_valor;

    switch (opcion) {
      case '1':
        Serial.print("Ingrese nueva temperatura (ej: 25.0): ");
        while (Serial.available() == 0) {
          // Esperar entrada
          delay(100);
        }
        nuevo_valor = Serial.parseFloat();
        
        // Limpiar buffer después de leer
        while (Serial.available() > 0) Serial.read();
        
        temp_programada = nuevo_valor;
        // Guardar en NVS (Mejora 1)
        preferences.putFloat("temp_p", temp_programada);
        
        // Marcar como configurado si es la primera vez
        if (!sistema_configurado) {
          sistema_configurado = true;
        }
        
        Serial.print("Temperatura programada: ");
        Serial.println(temp_programada);
        
        // Actualizar inmediatamente el estado LED si no estamos en modo programada
        if (sistema_configurado && !showing_programada) {
          controlarLEDs(ultima_temp_leida, showing_programada);
        }
        break;

      case '2':
        Serial.print("Ingrese nueva histeresis (ej: 1.5): ");
        while (Serial.available() == 0) {
          delay(100);
        }
        nuevo_valor = Serial.parseFloat();
        
        // Limpiar buffer después de leer
        while (Serial.available() > 0) Serial.read();
        
        hysteresis = nuevo_valor;
        // Guardar en NVS (Mejora 1)
        preferences.putFloat("hyst", hysteresis);
        
        // Marcar como configurado si es la primera vez
        if (!sistema_configurado) {
          sistema_configurado = true;
        }
        
        Serial.print("Histeresis actual: ");
        Serial.println(hysteresis);
        
        // Actualizar inmediatamente el estado LED si no estamos en modo programada
        if (sistema_configurado && !showing_programada) {
          controlarLEDs(ultima_temp_leida, showing_programada);
        }
        break;

      case '3':
        {
          // Usar la temperatura estable que estamos mostrando
          Serial.print("Temperatura actual: ");
          Serial.print(ultima_temp_leida);
          Serial.println(" C");
        }
        break;

      case '4':
        Serial.print("Histeresis actual: ");
        Serial.println(hysteresis);
        break;

      default:
        Serial.println("Opcion invalida. Ingrese 1, 2, 3, o 4.");
        break;
    }
    
    // Volver a mostrar el menú
    mostrarMenu();
  }
}

// Función para manejar el display con sincronización de LEDs
void manejarDisplay(float temperatura_actual) {
  unsigned long currentMillis = millis();

  // Alternar la visualización: 
  // - 4 segundos mostrando temperatura actual
  // - 1 segundo mostrando temperatura programada con "P"
  if (currentMillis - previousMillis_display >= interval_display) {
    previousMillis_display = currentMillis;
    showing_programada = true; // Mostrar temperatura programada
    // Actualizar los LEDs para el modo programada
    controlarLEDs(temperatura_actual, showing_programada);
  } else if (currentMillis - previousMillis_display >= 1000 && showing_programada) {
    // Si ha pasado 1 segundo en modo programada, volver a actual
    showing_programada = false;
    // Actualizar los LEDs para el modo de temperatura actual
    controlarLEDs(temperatura_actual, showing_programada);
  }

  if (showing_programada) {
    // Mostrar "P" y la temperatura programada
    char tempStr[10];
    // Formato limpio para el display
    if (temp_programada >= 10) {
      // Para valores ≥ 10, mostramos sin espacio: "P25.0"
      sprintf(tempStr, "P%2.1f", temp_programada);
    } else {
      // Para valores < 10, añadimos un espacio: "P 5.0"
      sprintf(tempStr, "P %1.1f", temp_programada);
    }
    sevseg.setChars(tempStr);
  } else {
    // Mostrar temperatura actual
    char tempStr[10];
    
    // Aseguramos el formato correcto para temperatura actual
    if (temperatura_actual >= 10) {
      // Para valores ≥ 10, mostramos con 1 decimal: "12.3"
      sprintf(tempStr, "%2.1f", temperatura_actual);
    } else {
      // Para valores < 10, añadimos un espacio y 1 decimal: " 9.5"
      sprintf(tempStr, " %1.1f", temperatura_actual);
    }
    // Usar setChars en lugar de setNumber para mayor control
    sevseg.setChars(tempStr);
  }
}

// --- Setup ---
void setup() {
  Serial.begin(115200);
  delay(500); // Pequeño delay para estabilizar la comunicación serial

  // Configurar pines de LEDs como salida
  pinMode(PIN_LED_VERDE, OUTPUT);
  pinMode(PIN_LED_ROJO, OUTPUT);
  pinMode(PIN_LED_AZUL, OUTPUT);
  // Inicialmente apagar todos los LEDs
  digitalWrite(PIN_LED_VERDE, LOW);
  digitalWrite(PIN_LED_ROJO, LOW);
  digitalWrite(PIN_LED_AZUL, LOW);

  // Configurar ADC para mejor resolución en ESP32
  analogReadResolution(12); // 12 bits (0-4095)
  
  // --- Inicializar NVS (Almacenamiento No Volátil - Mejora 1) ---
  preferences.begin("climate_cfg", false);

  // Verificar si hay valores guardados
  if (preferences.isKey("temp_p") && preferences.isKey("hyst")) {
    temp_programada = preferences.getFloat("temp_p", 22.0);
    hysteresis = preferences.getFloat("hyst", 1.0);
    sistema_configurado = true; // Sistema ya configurado con valores previos
    Serial.println("Valores cargados de memoria NVS.");
  }

  Serial.println("Sistema iniciado.");
  Serial.print("Temp Programada inicial: "); Serial.print(temp_programada); Serial.println(" C");
  Serial.print("Histeresis inicial: "); Serial.println(hysteresis);

  // --- Inicializar Display de 7 Segmentos (Mejora 2) ---
  sevseg.begin(hardwareConfig, numDigits, digitPins, segmentPins, resistorOnSegments);
  sevseg.setBrightness(90); // Ajusta el brillo (0-100)

  // Si el sistema ya está configurado, determinar estado inicial del LED
  if (sistema_configurado) {
    // Realizar la primera lectura de temperatura
    ultima_temp_leida = leerTemperaturaLM35();
    // Inicializar los LEDs en función de la temperatura actual y el modo actual (no programada)
    controlarLEDs(ultima_temp_leida, false);
  }

  // Mostrar el menú serial para interacción del usuario
  mostrarMenu();
}

// --- Loop Principal ---
void loop() {
  unsigned long currentMillis = millis();
  
  // Actualizar la lectura de temperatura solo cada 4 segundos
  if (currentMillis - previousMillis_lectura >= interval_lectura) {
    previousMillis_lectura = currentMillis;
    ultima_temp_leida = leerTemperaturaLM35();
    
    // Si el sistema está configurado y no estamos mostrando la temperatura programada,
    // controlar los LEDs con la nueva temperatura
    if (sistema_configurado && !showing_programada) {
      controlarLEDs(ultima_temp_leida, showing_programada);
    }
  }
  
  // Si el sistema está configurado, manejar el display con la temperatura estable
  if (sistema_configurado) {
    manejarDisplay(ultima_temp_leida);
  }

  // Comprobar comandos seriales (esto permite configurar en cualquier momento)
  manejarComandoSerial();

  // Refrescar el display (multiplexado)
  sevseg.refreshDisplay();
  
  // Pequeña pausa para estabilidad
  delay(5); // Delay mínimo para no afectar el multiplexado del display
}
