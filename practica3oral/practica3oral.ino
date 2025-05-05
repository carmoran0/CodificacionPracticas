#include <ESP32Servo.h>
#include <PID_v1_bc.h>

// ===== DEFINIÓN DE PINES =====
const int potPin = 18;      // Potenciómetro (GPIO18)
const int buttonPin = 12;   // Pulsador (GPIO12)
const int ledPin = 11;      // LED rojo (GPIO11)
const int servoPin = 13;    // Servo (GPIO13)

// ===== FILTRO POTENCIÓMETRO =====
const int numReadings = 10; // Número de lecturas para el promedio
int readings[numReadings];  // Array para almacenar lecturas
int readIndex = 0;          // Índice de la lectura actual
int total = 0;              // Suma total

// ===== VARIABLES GLOBALES =====
Servo throttleServo;        
double Setpoint, Input, Output;  
bool pitMode = false;      
unsigned long lastButtonPress = 0;
const int debounceDelay = 250;  

// Parámetros PID
double Kp = 0.8, Ki = 0.05, Kd = 0.02;

// ===== CONFIGURACIÓN DEL PID =====
PID myPID(&Input, &Output, &Setpoint, Kp, Ki, Kd, DIRECT);

// ===== DECLARACIONES DE FUNCIÓN =====
void handleButton();
void handleSerialMenu();
void adjustParameter(const char* paramName, double* param);
void displayMenu();

// ===== SETUP =====
void setup() {
  Serial.begin(115200);
 
  pinMode(potPin, INPUT);
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(ledPin, OUTPUT);
 
  throttleServo.attach(servoPin);
  
  // Inicializar filtro del potenciómetro
  for (int i = 0; i < numReadings; i++) {
    readings[i] = analogRead(potPin);
    total += readings[i];
  }
  Input = total / numReadings;
  Setpoint = Input;
  
  myPID.SetMode(AUTOMATIC);
  myPID.SetOutputLimits(0, 1023);
  digitalWrite(ledPin, LOW);
  
  displayMenu();
}

// ===== LOOP PRINCIPAL =====
void loop() {
  // Lectura filtrada del potenciómetro
  total -= readings[readIndex];
  readings[readIndex] = analogRead(potPin);
  total += readings[readIndex];
  readIndex = (readIndex + 1) % numReadings;
  Input = total / numReadings;
  
  handleButton();
 
  // Manejar estado del LED según el modo pit
  if (pitMode) {
    digitalWrite(ledPin, HIGH);  // Encender LED en modo pit/boxes
    myPID.SetOutputLimits(0, 256);  // Límite al 25%
  } else {
    digitalWrite(ledPin, LOW);   // Apagar LED fuera de modo pit/boxes
    myPID.SetOutputLimits(0, 1023); // Rango completo
  }
 
  myPID.Compute();
  
  // Mapeo con límites reducidos y zona muerta para evitar vibraciones
  int servoAngle = map(Output, 0, 1023, 5, 175);
  if (servoAngle > 170) servoAngle = 175;
  if (servoAngle < 10) servoAngle = 5;
  throttleServo.write(servoAngle);
  
  if (Serial.available() > 0) {
    handleSerialMenu();
  }
  
  delay(50);
}

// ===== Manejo del pulsador =====
void handleButton() {
  if (digitalRead(buttonPin) == LOW) {  // Botón presionado (lógica invertida con PULLUP)
    if (millis() - lastButtonPress > debounceDelay) {
      lastButtonPress = millis();
      pitMode = !pitMode;
      digitalWrite(ledPin, pitMode ? HIGH : LOW);
      Serial.print("Modo Boxes: ");
      Serial.println(pitMode ? "ACTIVADO" : "DESACTIVADO");
    }
  }
}

// ===== Menú serial =====
void handleSerialMenu() {
  char option = Serial.read();
  // Limpiar buffer serial
  while(Serial.available() > 0) Serial.read();
  
  switch(option) {
    case '1':
      adjustParameter("Kp", &Kp);
      break;
    case '2':
      adjustParameter("Ki", &Ki);
      break;
    case '3':
      adjustParameter("Kd", &Kd);
      break;
    case '4':
      Serial.print("Ángulo del servo: ");
      Serial.print(map(Output, 0, 1023, 5, 175));
      Serial.println("°");
      break;
    case '5':
      pitMode = !pitMode;
      digitalWrite(ledPin, pitMode ? HIGH : LOW);
      Serial.print("Modo Boxes cambiado manualmente a: ");
      Serial.println(pitMode ? "ACTIVADO" : "DESACTIVADO");
      break;
    case '\n':
    case '\r':
      break;
    default:
      Serial.println("Opción no válida");
  }
  displayMenu();
}

void adjustParameter(const char* paramName, double* param) {
  Serial.print("\nValor actual de ");
  Serial.print(paramName);
  Serial.print(": ");
  Serial.println(*param);
  Serial.println("Ingrese nuevo valor y presione Enter...");
  
  while(Serial.available() == 0);
  String inputStr = Serial.readStringUntil('\n');
  inputStr.trim();
  
  if (inputStr.length() > 0) {
    double newValue = inputStr.toFloat();
    *param = newValue;
    myPID.SetTunings(Kp, Ki, Kd);
    Serial.print(paramName);
    Serial.print(" actualizado a: ");
    Serial.println(*param);
  }
}

void displayMenu() {
  Serial.println("\n--- MENÚ DE CONFIGURACIÓN ---");
  Serial.println("1. Ajustar Kp (Ganancia Proporcional)");
  Serial.println("2. Ajustar Ki (Ganancia Integral)");
  Serial.println("3. Ajustar Kd (Ganancia Derivativa)");
  Serial.println("4. Mostrar ángulo actual del servo");
  Serial.println("5. Cambiar modo Boxes (pit mode)");
  Serial.println("-----------------------------");
  Serial.print("Estado actual del modo Boxes: ");
  Serial.println(pitMode ? "ACTIVADO" : "DESACTIVADO");
  Serial.print("Seleccione una opción (1-5): ");
}
