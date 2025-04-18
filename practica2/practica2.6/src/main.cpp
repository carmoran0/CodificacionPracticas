/*
 * Práctica 2.6 - Clase NoisyChannel
 * 
 * Esta clase simula un canal ruidoso para comunicaciones binarias.
 * Permite enviar y recibir paquetes a través de un canal con ruido aleatorio.
 */

#include <Arduino.h>

/**
 * Clase que simula un canal ruidoso para comunicaciones binarias.
 * Permite configurar el porcentaje de ruido que se introducirá en los mensajes.
 */
class NoisyChannel {
private:
  float noisePercentage; // Porcentaje de ruido (entre 0 y 1)
  
  // Inicializa la semilla para la generación de números aleatorios
  void initRandomSeed() {
    randomSeed(analogRead(0));
  }
  
public:
  /**
   * Constructor de la clase NoisyChannel.
   * @param noise Porcentaje de ruido que se introducirá en los mensajes (entre 0 y 1)
   */
  NoisyChannel(float noise) {
    // Asegurar que el porcentaje de ruido esté entre 0 y 1
    noisePercentage = constrain(noise, 0.0, 1.0);
    initRandomSeed();
  }
  
  /**
   * Envía un paquete a través del canal ruidoso.
   * @param input Vector binario de entrada empaquetado en unsigned char
   * @param output Vector binario de salida empaquetado en unsigned char
   * @param length Longitud del vector en bytes
   */
  void sendPacket(unsigned char *input, unsigned char *output, int length) {
    // Recorrer cada byte del vector de entrada
    for (int i = 0; i < length; i++) {
      // Inicializar el byte de salida con el valor del byte de entrada
      output[i] = input[i];
      
      // Procesar cada bit del byte actual
      for (int j = 0; j < 8; j++) {
        // Generar un número aleatorio entre 0 y 1
        float r = random(0, 100) / 100.0;
        
        // Si el número aleatorio es menor que la probabilidad de error,
        // invertir el bit correspondiente en el byte de salida
        if (r < noisePercentage) {
          // Invertir el bit j-ésimo usando XOR con una máscara
          output[i] ^= (1 << j);
        }
      }
    }
  }
  
  /**
   * Recibe un paquete a través del canal ruidoso.
   * Esta función es idéntica a sendPacket, ya que el canal es simétrico.
   * @param input Vector binario de entrada empaquetado en unsigned char
   * @param output Vector binario de salida empaquetado en unsigned char
   * @param length Longitud del vector en bytes
   */
  void receivePacket(unsigned char *input, unsigned char *output, int length) {
    // En un canal simétrico, enviar y recibir son operaciones equivalentes
    sendPacket(input, output, length);
  }
  
  /**
   * Obtiene el porcentaje de ruido configurado en el canal.
   * @return Porcentaje de ruido (entre 0 y 1)
   */
  float getNoisePercentage() {
    return noisePercentage;
  }
  
  /**
   * Establece un nuevo porcentaje de ruido para el canal.
   * @param noise Nuevo porcentaje de ruido (entre 0 y 1)
   */
  void setNoisePercentage(float noise) {
    noisePercentage = constrain(noise, 0.0, 1.0);
  }
};

// Función para imprimir un vector de bytes en formato binario
void printBinaryVector(unsigned char *vec, int length) {
  for (int i = 0; i < length; i++) {
    for (int j = 7; j >= 0; j--) {
      // Extraer y mostrar cada bit, empezando por el más significativo
      Serial.print((vec[i] >> j) & 1);
    }
    Serial.print(" ");
  }
  Serial.println();
}

void setup() {
  // Inicializar comunicación serial
  Serial.begin(9600);
  while (!Serial) {
    ; // Esperar a que el puerto serial se conecte
  }
  
  // Ejemplo de uso de la clase NoisyChannel
  const int length = 3; // Longitud del vector en bytes
  unsigned char input[length] = {0b10101010, 0b11110000, 0b00001111};
  unsigned char output[length];
  
  // Crear un canal con 20% de ruido
  NoisyChannel channel(0.2);
  
  Serial.println("Vector de entrada:");
  printBinaryVector(input, length);
  
  // Enviar el paquete a través del canal ruidoso
  channel.sendPacket(input, output, length);
  
  Serial.println("Vector de salida (después del canal ruidoso):");
  printBinaryVector(output, length);
  
  // Contar bits que han cambiado
  int changedBits = 0;
  for (int i = 0; i < length; i++) {
    unsigned char diff = input[i] ^ output[i]; // XOR para detectar bits diferentes
    for (int j = 0; j < 8; j++) {
      if ((diff >> j) & 1) {
        changedBits++;
      }
    }
  }
  
  Serial.print("Bits cambiados: ");
  Serial.print(changedBits);
  Serial.print(" de ");
  Serial.print(length * 8);
  Serial.print(" (");
  Serial.print((float)changedBits / (length * 8) * 100);
  Serial.println("%)");
  
  // Ejemplo de cambio de porcentaje de ruido
  Serial.println("\nCambiando porcentaje de ruido a 50%");
  channel.setNoisePercentage(0.5);
  
  // Reiniciar el vector de entrada
  unsigned char input2[length] = {0b10101010, 0b11110000, 0b00001111};
  unsigned char output2[length];
  
  Serial.println("Vector de entrada:");
  printBinaryVector(input2, length);
  
  // Enviar el paquete a través del canal ruidoso con el nuevo porcentaje
  channel.sendPacket(input2, output2, length);
  
  Serial.println("Vector de salida (después del canal ruidoso):");
  printBinaryVector(output2, length);
  
  // Contar bits que han cambiado
  changedBits = 0;
  for (int i = 0; i < length; i++) {
    unsigned char diff = input2[i] ^ output2[i]; // XOR para detectar bits diferentes
    for (int j = 0; j < 8; j++) {
      if ((diff >> j) & 1) {
        changedBits++;
      }
    }
  }
  
  Serial.print("Bits cambiados: ");
  Serial.print(changedBits);
  Serial.print(" de ");
  Serial.print(length * 8);
  Serial.print(" (");
  Serial.print((float)changedBits / (length * 8) * 100);
  Serial.println("%)");
}

void loop() {
  // No se requiere código en el loop para esta práctica
}