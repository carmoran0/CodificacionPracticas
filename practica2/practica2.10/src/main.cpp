/*
 * Práctica 2.10 - Clase HammingRepetition
 * 
 * Esta clase implementa un codificador y decodificador que combina los códigos Hamming y Repetición
 * en serie. Primero aplica el código Hamming (7,4) y luego el código de repetición de grado Rn.
 * Esta combinación aprovecha la capacidad de corrección de errores de un bit de Hamming junto con
 * la robustez adicional que proporciona la repetición.
 */
#include <Arduino.h>

// Función auxiliar global para imprimir un vector de bytes en formato binario
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

/**
 * Clase que implementa un codificador y decodificador Hamming (7,4).
 * Permite codificar mensajes usando el código Hamming (7,4) y decodificarlos
 * con capacidad de corrección de errores de un solo bit.
 */
class HammingCode {
public:
  /**
   * Constructor de la clase
   */
  HammingCode() {
    // No se requieren parámetros para Hamming (7,4)
  }
  
  /**
   * Método para calcular la longitud del mensaje codificado en bytes
   * @param originalLength Longitud del mensaje original en bytes
   * @return Longitud del mensaje codificado en bytes
   */
  int getEncodedLength(int originalLength) {
    // Cada byte tiene 2 bloques de 4 bits, y cada bloque genera 7 bits
    int codedBits = originalLength * 2 * 7; // Número total de bits en la salida codificada
    return (codedBits + 7) / 8; // Redondeo hacia arriba para obtener bytes
  }
  
  /**
   * Método para codificar un mensaje utilizando Hamming (7,4)
   * @param in Vector binario de entrada empaquetado en unsigned char
   * @param out Vector binario de salida empaquetado en unsigned char
   * @param length Longitud del vector de entrada en bytes
   */
  void encode(unsigned char *in, unsigned char *out, int length) {
    int outBitIndex = 0; // Índice del bit actual en el vector de salida
    
    // Recorrer cada byte del vector de entrada
    for (int i = 0; i < length; i++) {
      // Procesar cada bloque de 4 bits del byte actual (primero los 4 bits más significativos)
      for (int nibbleIndex = 0; nibbleIndex < 2; nibbleIndex++) {
        // Extraer el bloque de 4 bits
        unsigned char nibble;
        if (nibbleIndex == 0) {
          // Extraer los 4 bits más significativos (posiciones 7-4)
          nibble = (in[i] >> 4) & 0x0F;
        } else {
          // Extraer los 4 bits menos significativos (posiciones 3-0)
          nibble = in[i] & 0x0F;
        }
        
        // Extraer los bits de datos individuales (d1, d2, d3, d4)
        unsigned char d1 = (nibble >> 3) & 1; // Bit más significativo
        unsigned char d2 = (nibble >> 2) & 1;
        unsigned char d3 = (nibble >> 1) & 1;
        unsigned char d4 = nibble & 1;        // Bit menos significativo
        
        // Calcular los bits de paridad según las ecuaciones de Hamming (7,4)
        unsigned char p1 = d1 ^ d2 ^ d4;      // p1 = d1 + d2 + d4 (XOR)
        unsigned char p2 = d1 ^ d3 ^ d4;      // p2 = d1 + d3 + d4 (XOR)
        unsigned char p3 = d2 ^ d3 ^ d4;      // p3 = d2 + d3 + d4 (XOR)
        
        // Formar la palabra código de 7 bits: p1, p2, d1, p3, d2, d3, d4
        unsigned char resultado[7] = {p1, p2, d1, p3, d2, d3, d4};
        
        // Colocar los 7 bits de la palabra código en el vector de salida
        for (int j = 0; j < 7; j++) {
          // Calcular el índice del byte de salida donde se colocará el bit
          int outByteIndex = outBitIndex / 8;
          // Calcular la posición del bit dentro del byte de salida
          int outBitPosition = 7 - (outBitIndex % 8);
          
          // Si estamos en un nuevo byte, inicializarlo a 0
          if (outBitPosition == 7) {
            out[outByteIndex] = 0;
          }
          
          // Colocar el bit en la posición correspondiente del byte de salida
          out[outByteIndex] |= (resultado[j] << outBitPosition);
          
          // Incrementar el índice del bit de salida
          outBitIndex++;
        }
      }
    }
  }
  
  /**
   * Método para decodificar un mensaje codificado con Hamming (7,4)
   * @param in Vector binario de entrada empaquetado en unsigned char (mensaje codificado)
   * @param out Vector binario de salida empaquetado en unsigned char (mensaje decodificado)
   * @param length Longitud del vector de entrada en bytes
   */
  void decode(unsigned char *in, unsigned char *out, int length) {
    int inBitIndex = 0;   // Índice del bit actual en el vector de entrada
    int outBitIndex = 0;  // Índice del bit actual en el vector de salida
    
    // Inicializar el vector de salida a ceros
    int outBytes = (length * 8 * 4) / 7; // Aproximación del número de bytes necesarios
    outBytes = (outBytes + 7) / 8;  // Redondeo hacia arriba
    
    for (int i = 0; i < outBytes; i++) {
      out[i] = 0;
    }
    
    // Procesar cada palabra código de 7 bits
    while (inBitIndex + 6 < length * 8) { // Asegurarse de que hay al menos 7 bits disponibles
      // Extraer los 7 bits de la palabra código
      unsigned char codeword[7];
      
      for (int j = 0; j < 7; j++) {
        // Calcular el índice del byte de entrada y la posición del bit
        int inByteIndex = inBitIndex / 8;
        int inBitPosition = 7 - (inBitIndex % 8);
        
        // Extraer el bit de la posición correspondiente
        codeword[j] = (in[inByteIndex] >> inBitPosition) & 1;
        
        // Avanzar al siguiente bit de entrada
        inBitIndex++;
      }
      
      // Extraer bits de paridad y datos de la palabra código
      unsigned char p1 = codeword[0]; // Bit de paridad 1
      unsigned char p2 = codeword[1]; // Bit de paridad 2
      unsigned char d1 = codeword[2]; // Bit de dato 1
      unsigned char p3 = codeword[3]; // Bit de paridad 3
      unsigned char d2 = codeword[4]; // Bit de dato 2
      unsigned char d3 = codeword[5]; // Bit de dato 3
      unsigned char d4 = codeword[6]; // Bit de dato 4
      
      // Calcular el síndrome para detectar errores
      unsigned char s1 = p1 ^ d1 ^ d2 ^ d4; // Verificar paridad 1
      unsigned char s2 = p2 ^ d1 ^ d3 ^ d4; // Verificar paridad 2
      unsigned char s3 = p3 ^ d2 ^ d3 ^ d4; // Verificar paridad 3
      
      // Determinar la posición del error (si existe)
      unsigned char errorPos = (s3 << 2) | (s2 << 1) | s1;
      
      // Corregir el error si se detecta (errorPos != 0)
      if (errorPos != 0) {
        // Posiciones de error y su correspondencia en el array codeword
        // 1 -> p1 (índice 0)
        // 2 -> p2 (índice 1)
        // 3 -> d1 (índice 2)
        // 4 -> p3 (índice 3)
        // 5 -> d2 (índice 4)
        // 6 -> d3 (índice 5)
        // 7 -> d4 (índice 6)
        
        // Corregir el bit erróneo
        if (errorPos >= 1 && errorPos <= 7) {
          // Invertir el bit en la posición del error
          codeword[errorPos - 1] = codeword[errorPos - 1] ^ 1;
          
          // Actualizar los bits de datos si fueron corregidos
          if (errorPos == 3) d1 = codeword[2];
          else if (errorPos == 5) d2 = codeword[4];
          else if (errorPos == 6) d3 = codeword[5];
          else if (errorPos == 7) d4 = codeword[6];
        }
      }
      
      // Reconstruir el nibble (4 bits) original
      unsigned char nibble = (d1 << 3) | (d2 << 2) | (d3 << 1) | d4;
      
      // Determinar si este nibble va en la parte alta o baja del byte de salida
      if (outBitIndex % 8 == 0) {
        // Parte alta del byte (bits 7-4)
        out[outBitIndex / 8] |= (nibble << 4);
      } else {
        // Parte baja del byte (bits 3-0)
        out[outBitIndex / 8] |= nibble;
      }
      
      // Avanzar 4 bits en el índice de salida (un nibble)
      outBitIndex += 4;
    }
  }
};

/**
 * Clase que implementa un codificador y decodificador de repetición.
 * Permite codificar mensajes repitiendo cada bit n veces y decodificarlos
 * mediante un sistema de votación por mayoría.
 */
class RepetitionCode {
private:
  int repetitionDegree; // Grado de repetición (número de veces que se repite cada bit)
  
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
};

/**
 * Clase que implementa un codificador y decodificador que combina Hamming y Repetición en serie.
 * Primero aplica el código Hamming (7,4) y luego el código de repetición de grado Rn.
 */
class HammingRepetition {
private:
  HammingCode hammingCoder; // Codificador/decodificador Hamming
  RepetitionCode repetitionCoder; // Codificador/decodificador de repetición
  
public:
  /**
   * Constructor de la clase
   * @param repetitionDegree Grado de repetición (número de veces que se repite cada bit)
   */
  HammingRepetition(int repetitionDegree) : repetitionCoder(repetitionDegree) {
    // Inicialización de objetos en la lista de inicialización
  }
  
  /**
   * Método para establecer el grado de repetición
   * @param n Nuevo grado de repetición
   */
  void setRepetitionDegree(int n) {
    repetitionCoder.setRepetitionDegree(n);
  }
  
  /**
   * Método para obtener el grado de repetición actual
   * @return Grado de repetición
   */
  int getRepetitionDegree() {
    return repetitionCoder.getRepetitionDegree();
  }
  
  /**
   * Método para calcular la longitud del mensaje codificado en bytes
   * @param originalLength Longitud del mensaje original en bytes
   * @return Longitud del mensaje codificado en bytes
   */
  int getEncodedLength(int originalLength) {
    // Primero calculamos la longitud después de aplicar Hamming
    int hammingLength = hammingCoder.getEncodedLength(originalLength);
    // Luego calculamos la longitud después de aplicar repetición
    return repetitionCoder.getEncodedLength(hammingLength);
  }
  
  /**
   * Método para codificar un mensaje utilizando Hamming seguido de repetición
   * @param in Vector binario de entrada empaquetado en unsigned char
   * @param out Vector binario de salida empaquetado en unsigned char
   * @param length Longitud del vector de entrada en bytes
   */
  void encode(unsigned char *in, unsigned char *out, int length) {
    // Calcular la longitud del mensaje codificado con Hamming
    int hammingLength = hammingCoder.getEncodedLength(length);
    
    // Buffer temporal para almacenar el resultado de la codificación Hamming
    unsigned char *hammingOut = new unsigned char[hammingLength];
    
    // Aplicar codificación Hamming
    hammingCoder.encode(in, hammingOut, length);
    
    // Aplicar codificación por repetición al resultado de Hamming
    repetitionCoder.encode(hammingOut, out, hammingLength);
    
    // Liberar memoria del buffer temporal
    delete[] hammingOut;
  }
  
  /**
   * Método para decodificar un mensaje codificado con repetición seguido de Hamming
   * @param in Vector binario de entrada empaquetado en unsigned char (mensaje codificado)
   * @param out Vector binario de salida empaquetado en unsigned char (mensaje decodificado)
   * @param length Longitud del vector de entrada en bytes
   */
  void decode(unsigned char *in, unsigned char *out, int length) {
    // Calcular la longitud del mensaje después de decodificar la repetición
    int hammingLength = (length * 8) / repetitionCoder.getRepetitionDegree();
    hammingLength = (hammingLength + 7) / 8; // Redondeo hacia arriba para obtener bytes
    
    // Buffer temporal para almacenar el resultado de la decodificación de repetición
    unsigned char *repetitionOut = new unsigned char[hammingLength];
    
    // Aplicar decodificación de repetición
    repetitionCoder.decode(in, repetitionOut, length);
    
    // Aplicar decodificación Hamming al resultado
    hammingCoder.decode(repetitionOut, out, hammingLength);
    
    // Liberar memoria del buffer temporal
    delete[] repetitionOut;
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
    
    Serial.println("Mensaje codificado con Hamming+Repetición:");
    printBinaryVector(coded, codedLength);
    
    Serial.print("Grado de repetición: ");
    Serial.println(getRepetitionDegree());
    Serial.print("Bits del mensaje original: ");
    Serial.println(originalLength * 8);
    Serial.print("Bits del mensaje codificado: ");
    Serial.println(codedLength * 8);
    
    // Calcular y mostrar la tasa de código
    float rate = (float)(originalLength * 8) / (codedLength * 8);
    Serial.print("Tasa de código: ");
    Serial.println(rate, 4);
  }
};

/**
 * Función que simula un canal ruidoso.
 * @param in Vector binario de entrada empaquetado en unsigned char
 * @param out Vector binario de salida empaquetado en unsigned char
 * @param l Longitud del vector en bytes
 * @param f Probabilidad de que un bit cambie de valor (entre 0 y 1)
 */
void noisyChannel(unsigned char *in, unsigned char *out, int l, float f) {
  // Inicializar la semilla para la generación de números aleatorios
  randomSeed(analogRead(0));
  
  // Recorrer cada byte del vector de entrada
  for (int i = 0; i < l; i++) {
    // Inicializar el byte de salida con el valor del byte de entrada
    out[i] = in[i];
    
    // Procesar cada bit del byte actual
    for (int j = 0; j < 8; j++) {
      // Generar un número aleatorio entre 0 y 1
      float r = random(0, 100) / 100.0;
      
      // Si el número aleatorio es menor que la probabilidad de error f,
      // invertir el bit correspondiente en el byte de salida
      if (r < f) {
        // Invertir el bit j-ésimo usando XOR con una máscara
        out[i] ^= (1 << j);
      }
    }
  }
}

void setup() {
  // Inicializar comunicación serial
  Serial.begin(9600);
  while (!Serial) {
    ; // Esperar a que el puerto serial se conecte
  }
  
  // Mensaje original para demostración
  const int originalLength = 2; // Longitud del mensaje original en bytes
  unsigned char original[originalLength] = {0b10101010, 0b11110000};
  
  // Probabilidad de error del canal ruidoso
  const float ERROR_PROBABILITY = 0.05;
  
  // Crear una instancia de HammingRepetition con grado de repetición 3
  HammingRepetition hammingRepCode(3);
  
  // Calcular el tamaño necesario para el mensaje codificado
  int codedLength = hammingRepCode.getEncodedLength(originalLength);
  
  // Crear vectores para almacenar el mensaje codificado y el mensaje recibido con ruido
  unsigned char coded[codedLength];
  unsigned char received[codedLength];
  
  // Codificar el mensaje original
  hammingRepCode.encode(original, coded, originalLength);
  
  // Mostrar información sobre el mensaje original y el codificado
  Serial.println("=== Demostración de HammingRepetition ===");
  hammingRepCode.printInfo(original, coded, originalLength, codedLength);
  
  // Simular la transmisión a través de un canal ruidoso
  Serial.println("\nSimulando transmisión a través de un canal ruidoso...");
  Serial.print("Probabilidad de error del canal: ");
  Serial.println(ERROR_PROBABILITY);
  
  noisyChannel(coded, received, codedLength, ERROR_PROBABILITY);
  
  Serial.println("\nMensaje recibido (con ruido):");
  printBinaryVector(received, codedLength);
  
  // Decodificar el mensaje recibido
  unsigned char decoded[originalLength];
  hammingRepCode.decode(received, decoded, codedLength);
  
  // Mostrar el mensaje decodificado
  Serial.println("\nMensaje decodificado:");
  printBinaryVector(decoded, originalLength);
  
  // Verificar si la decodificación fue exitosa
  bool success = true;
  for (int i = 0; i < originalLength; i++) {
    if (original[i] != decoded[i]) {
      success = false;
      break;
    }
  }
  
  Serial.print("\nDecodificación ");
  if (success) {
    Serial.println("exitosa! El mensaje se recuperó correctamente.");
  } else {
    Serial.println("fallida. El mensaje contiene errores.");
  }
  
  // Comparar con solo Hamming y solo Repetición
  Serial.println("\n=== Comparación con otros códigos ===");
  
  // Crear instancias de los codificadores individuales
  HammingCode hammingCode;
  RepetitionCode repetitionCode(3);
  
  // Calcular longitudes codificadas
  int hammingLength = hammingCode.getEncodedLength(originalLength);
  int repetitionLength = repetitionCode.getEncodedLength(originalLength);
  
  Serial.println("Longitud en bytes del mensaje original: " + String(originalLength));
  Serial.println("Longitud en bytes con Hamming (7,4): " + String(hammingLength));
  Serial.println("Longitud en bytes con Repetición (R3): " + String(repetitionLength));
  Serial.println("Longitud en bytes con Hamming+Repetición: " + String(codedLength));
  
  Serial.println("\nTasa de código Hamming (7,4): 4/7 = " + String((float)4/7, 4));
  Serial.println("Tasa de código Repetición (R3): 1/3 = " + String((float)1/3, 4));
  Serial.println("Tasa de código Hamming+Repetición: " + String((float)(originalLength * 8) / (codedLength * 8), 4));
}

void loop() {
  // No se requiere funcionalidad en el bucle principal
}