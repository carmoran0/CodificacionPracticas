/*
 * Práctica 2.7 - Clase RepetitionCode
 * 
 * Esta clase implementa un codificador y decodificador de repetición de grado Rn donde n es el número
 * de bits que se repiten en cada bloque. Encapsula la funcionalidad de codificación y decodificación
 * por repetición en una clase reutilizable.
 */
#include <Arduino.h>

/**
 * Clase que implementa un codificador y decodificador de repetición.
 * Permite codificar mensajes repitiendo cada bit n veces y decodificarlos
 * mediante un sistema de votación por mayoría.
 */
class RepetitionCode {
private:
  int repetitionDegree; // Grado de repetición (número de veces que se repite cada bit)
  
  /**
   * Método auxiliar para imprimir un vector de bytes en formato binario
   * @param vec Vector a imprimir
   * @param length Longitud del vector en bytes
   */
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
  
public:
  /**
   * Constructor de la clase
   * @param n Grado de repetición (número de veces que se repite cada bit)
   */
  RepetitionCode(int n) {
    repetitionDegree = n;
  }
  
  /**
   * Método para establecer el grado de repetición
   * @param n Nuevo grado de repetición
   */
  void setRepetitionDegree(int n) {
    repetitionDegree = n;
  }
  
  /**
   * Método para obtener el grado de repetición actual
   * @return Grado de repetición
   */
  int getRepetitionDegree() {
    return repetitionDegree;
  }
  
  /**
   * Método para calcular la longitud del mensaje codificado en bytes
   * @param originalLength Longitud del mensaje original en bytes
   * @return Longitud del mensaje codificado en bytes
   */
  int getEncodedLength(int originalLength) {
    return (originalLength * 8 * repetitionDegree + 7) / 8; // Redondeo hacia arriba
  }
  
  /**
   * Método para codificar un mensaje utilizando repetición
   * @param in Vector binario de entrada empaquetado en unsigned char
   * @param out Vector binario de salida empaquetado en unsigned char
   * @param length Longitud del vector de entrada en bytes
   */
  void encode(unsigned char *in, unsigned char *out, int length) {
    int outBitIndex = 0; // Índice del bit actual en el vector de salida
    
    // Recorrer cada byte del vector de entrada
    for (int i = 0; i < length; i++) {
      // Procesar cada bit del byte actual
      for (int j = 7; j >= 0; j--) {
        // Extraer el bit j-ésimo del byte i-ésimo
        unsigned char bit = (in[i] >> j) & 1;
        
        // Repetir el bit n veces
        for (int k = 0; k < repetitionDegree; k++) {
          // Calcular el índice del byte de salida donde se colocará el bit repetido
          int outByteIndex = outBitIndex / 8;
          // Calcular la posición del bit dentro del byte de salida
          int outBitPosition = 7 - (outBitIndex % 8);
          
          // Si estamos en un nuevo byte, inicializarlo a 0
          if (outBitPosition == 7) {
            out[outByteIndex] = 0;
          }
          
          // Colocar el bit en la posición correspondiente del byte de salida
          out[outByteIndex] |= (bit << outBitPosition);
          
          // Incrementar el índice del bit de salida
          outBitIndex++;
        }
      }
    }
  }
  
  /**
   * Método para decodificar un mensaje codificado con repetición
   * @param in Vector binario de entrada empaquetado en unsigned char (mensaje codificado)
   * @param out Vector binario de salida empaquetado en unsigned char (mensaje decodificado)
   * @param length Longitud del vector de entrada en bytes
   */
  void decode(unsigned char *in, unsigned char *out, int length) {
    int inBitIndex = 0;  // Índice del bit actual en el vector de entrada
    int outBitIndex = 0; // Índice del bit actual en el vector de salida
    
    // Inicializar el vector de salida a ceros
    int outLength = (length * 8) / repetitionDegree; // Número total de bits en el mensaje original
    int outBytes = (outLength + 7) / 8; // Número de bytes necesarios para almacenar el mensaje original
    
    for (int i = 0; i < outBytes; i++) {
      out[i] = 0;
    }
    
    // Procesar cada grupo de n bits repetidos
    while (inBitIndex < length * 8) {
      int countOnes = 0; // Contador de unos en el grupo actual
      
      // Contar cuántos unos hay en el grupo de n bits
      for (int k = 0; k < repetitionDegree && inBitIndex < length * 8; k++) {
        // Calcular el índice del byte de entrada y la posición del bit
        int inByteIndex = inBitIndex / 8;
        int inBitPosition = 7 - (inBitIndex % 8);
        
        // Extraer el bit de la posición correspondiente
        unsigned char bit = (in[inByteIndex] >> inBitPosition) & 1;
        
        // Incrementar el contador si el bit es 1
        if (bit == 1) {
          countOnes++;
        }
        
        // Avanzar al siguiente bit de entrada
        inBitIndex++;
      }
      
      // Determinar el bit original mediante votación por mayoría
      unsigned char decodedBit = (countOnes > repetitionDegree / 2) ? 1 : 0;
      
      // Calcular el índice del byte de salida y la posición del bit
      int outByteIndex = outBitIndex / 8;
      int outBitPosition = 7 - (outBitIndex % 8);
      
      // Colocar el bit decodificado en la posición correspondiente del byte de salida
      out[outByteIndex] |= (decodedBit << outBitPosition);
      
      // Avanzar al siguiente bit de salida
      outBitIndex++;
    }
  }
  
  /**
   * Método para mostrar información sobre un mensaje y su versión codificada
   * @param original Mensaje original
   * @param coded Mensaje codificado
   * @param originalLength Longitud del mensaje original en bytes
   * @param codedLength Longitud del mensaje codificado en bytes
   */
  void printInfo(unsigned char *original, unsigned char *coded, int originalLength, int codedLength) {
    Serial.println("Mensaje original:");
    printBinaryVector(original, originalLength);
    
    Serial.println("Mensaje codificado:");
    printBinaryVector(coded, codedLength);
    
    Serial.print("Grado de repetición: ");
    Serial.println(repetitionDegree);
    Serial.print("Bits del mensaje original: ");
    Serial.println(originalLength * 8);
    Serial.print("Bits del mensaje codificado: ");
    Serial.println(codedLength * 8);
  }
};

void setup() {
  // Inicializar comunicación serial
  Serial.begin(9600);
  while (!Serial) {
    ; // Esperar a que el puerto serial se conecte
  }
  
  // Crear una instancia de RepetitionCode con grado 3
  RepetitionCode repCode(3);
  
  // Mensaje original para demostración
  const int originalLength = 2; // Longitud del mensaje original en bytes
  unsigned char original[originalLength] = {0b10101010, 0b11110000};
  
  // Calcular el tamaño necesario para el mensaje codificado
  int codedLength = repCode.getEncodedLength(originalLength);
  unsigned char coded[codedLength];
  
  // Codificar el mensaje
  repCode.encode(original, coded, originalLength);
  
  // Mensaje decodificado
  unsigned char decoded[originalLength];
  
  // Mostrar información sobre el mensaje original y codificado
  repCode.printInfo(original, coded, originalLength, codedLength);
  
  // Decodificar el mensaje
  repCode.decode(coded, decoded, codedLength);
  
  Serial.println("Mensaje decodificado:");
  for (int i = 0; i < originalLength; i++) {
    for (int j = 7; j >= 0; j--) {
      Serial.print((decoded[i] >> j) & 1);
    }
    Serial.print(" ");
  }
  Serial.println();
  
  // Verificar si la decodificación fue correcta
  bool correct = true;
  for (int i = 0; i < originalLength; i++) {
    if (original[i] != decoded[i]) {
      correct = false;
      break;
    }
  }
  
  Serial.print("Decodificación correcta: ");
  Serial.println(correct ? "Sí" : "No");
  
  // Ejemplo con otro grado de repetición
  Serial.println("\nCambiando el grado de repetición a 5:");
  repCode.setRepetitionDegree(5);
  
  // Recalcular el tamaño del mensaje codificado
  codedLength = repCode.getEncodedLength(originalLength);
  unsigned char coded2[codedLength];
  
  // Codificar y decodificar con el nuevo grado
  repCode.encode(original, coded2, originalLength);
  repCode.decode(coded2, decoded, codedLength);
  
  // Mostrar información
  repCode.printInfo(original, coded2, originalLength, codedLength);
  
  Serial.println("Mensaje decodificado:");
  for (int i = 0; i < originalLength; i++) {
    for (int j = 7; j >= 0; j--) {
      Serial.print((decoded[i] >> j) & 1);
    }
    Serial.print(" ");
  }
  Serial.println();
}

void loop() {
  // No se requiere código en el loop para esta práctica
}