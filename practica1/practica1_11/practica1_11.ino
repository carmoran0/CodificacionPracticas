/*
11. strncat es una función de librería que copia una cadena al final de otra
https://linux.die.net/man/3/strcat char *strncat(char *dest, char *src, size_t n)
Escriba el código que debe contener la función strncat para que se realice la copia,
tenga en cuenta que dest debe tener espacio suficiente para contener la combinación
de ambas cadenas.
*/

// Implementación de la función strncat
char *my_strncat(char *dest, const char *src, size_t n) {
  // Guardamos el puntero original para devolverlo al final
  char *original_dest = dest;
  
  // Encontramos el final de la cadena destino
  while (*dest != '\0') {
    dest++;
  }
  
  // Copiamos hasta n caracteres de src a dest
  size_t i = 0;
  while (i < n && *src != '\0') {
    *dest = *src;
    dest++;
    src++;
    i++;
  }
  
  // Aseguramos que la cadena resultante termine con '\0'
  *dest = '\0';
  
  // Devolvemos el puntero original
  return original_dest;
}

// Definimos el tamaño máximo del buffer
#define MAX_BUFFER 100

void setup() {
  // Inicializamos la comunicación serial
  Serial.begin(9600);
  while (!Serial) {
    ; // Esperamos a que el puerto serial se conecte
  }
  
  Serial.println("Prueba de implementación de strncat");
  
  // Caso 1: Concatenación normal
  char buffer1[MAX_BUFFER] = "Hola ";
  Serial.print("Cadena original: ");
  Serial.println(buffer1);
  my_strncat(buffer1, "mundo!", 6);
  Serial.print("Después de strncat: ");
  Serial.println(buffer1);
  Serial.println();
  
  // Caso 2: Límite de caracteres
  char buffer2[MAX_BUFFER] = "Prueba: ";
  Serial.print("Cadena original: ");
  Serial.println(buffer2);
  my_strncat(buffer2, "123456789abcd", 10);
  Serial.print("Después de strncat con límite de 10 caracteres: ");
  Serial.println(buffer2);
  Serial.println();
  
  // Caso 3: Cadena vacía como destino
  char buffer3[MAX_BUFFER] = "";
  Serial.print("Cadena original (vacía): ");
  Serial.println(buffer3);
  my_strncat(buffer3, "Añadiendo a cadena vacía", 15);
  Serial.print("Después de strncat: ");
  Serial.println(buffer3);
  Serial.println();
  
  // Caso 4: n = 0 (no debería copiar nada)
  char buffer4[MAX_BUFFER] = "Test ";
  Serial.print("Cadena original: ");
  Serial.println(buffer4);
  my_strncat(buffer4, "no se copia nada", 0);
  Serial.print("Después de strncat con n=0: ");
  Serial.println(buffer4);
  Serial.println();
}

void loop() {
  // No hacemos nada en el loop
}