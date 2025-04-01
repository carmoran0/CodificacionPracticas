/*
12. bool strinit(char *s, char *t) es una función que devuelve 1 si la cadena t
representa el principio de la cadena s o 0 en caso contrario. Implemente dicha función.
*/

// Implementación de la función strinit
bool strinit(char *s, char *t) {
  // Si t es una cadena vacía, siempre es el inicio de cualquier cadena
  if (*t == '\0') {
    return true;
  }
  
  // Comparamos caracter por caracter
  while (*t != '\0') {
    // Si s termina antes que t o los caracteres no coinciden
    if (*s == '\0' || *s != *t) {
      return false;
    }
    // Avanzamos ambos punteros
    s++;
    t++;
  }
  
  // Si llegamos aquí, significa que recorrimos toda la cadena t
  // y todos los caracteres coincidieron con el inicio de s
  return true;
}

// Definimos el tamaño máximo del buffer
#define MAX_BUFFER 100

void setup() {
  // Inicializamos la comunicación serial
  Serial.begin(9600);
  while (!Serial) {
    ; // Esperamos a que el puerto serial se conecte
  }
  
  Serial.println("Prueba de implementación de strinit");
  
  // Caso 1: t es el inicio de s
  char s1[] = "Hola mundo";
  char t1[] = "Hola";
  Serial.print("¿'" + String(t1) + "' es el inicio de '" + String(s1) + "'? ");
  Serial.println(strinit(s1, t1) ? "Sí" : "No");
  
  // Caso 2: t no es el inicio de s
  char s2[] = "Hola mundo";
  char t2[] = "mundo";
  Serial.print("¿'" + String(t2) + "' es el inicio de '" + String(s2) + "'? ");
  Serial.println(strinit(s2, t2) ? "Sí" : "No");
  
  // Caso 3: t es más largo que s
  char s3[] = "Hola";
  char t3[] = "Hola mundo";
  Serial.print("¿'" + String(t3) + "' es el inicio de '" + String(s3) + "'? ");
  Serial.println(strinit(s3, t3) ? "Sí" : "No");
  
  // Caso 4: t es una cadena vacía
  char s4[] = "Hola";
  char t4[] = "";
  Serial.print("¿'" + String(t4) + "' es el inicio de '" + String(s4) + "'? ");
  Serial.println(strinit(s4, t4) ? "Sí" : "No");
  
  // Caso 5: s y t son iguales
  char s5[] = "Hola";
  char t5[] = "Hola";
  Serial.print("¿'" + String(t5) + "' es el inicio de '" + String(s5) + "'? ");
  Serial.println(strinit(s5, t5) ? "Sí" : "No");
}

void loop() {
  // No hacemos nada en el loop
}