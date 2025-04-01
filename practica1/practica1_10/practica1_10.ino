/*
10. El programa solicita un número entero como entrada y responde indicando todos
los factores primos del número. Por ejemplo, si el número es 8 -> 2^3, si el número es
24 -> 2^3; 3^1
*/
/* IO with Serial clases */
#define MAXBUF 20
char buffer[MAXBUF];
boolean myReadString(int timeout);
void setup() {
  delay(700);
  Serial.begin(9600);
  delay(700);
  Serial.print("Dame un número: \n");
}
void loop(){
  if(myReadString(50)){
    int numero = atoi(buffer);
    Serial.print("Factores primos de ");
    Serial.print(numero);
    Serial.println(":");
    factoresPrimos(numero);
  }
} 
//Working on Arduino Uno
boolean myReadString(int timeout) { //ReadString with timeout limit, return true if a string is readed within timeout limits 
  static int i=0; //Número de caracteres leidos
  static long tLimit; //Reloj interno de timeout
  static boolean reading=false; //Marca si hay una lectura en curso
  while((Serial.available()>0) && (i<(MAXBUF-1))){ //Hay caracteres disponibles y no hemos desbordado el buffer
    if(!reading){ //First character
      reading=true; //Estamos en un proceso de lectura
      tLimit = millis(); //Ajustamos el reloj de timeout
      i=0; //Iniciamos el índice del buffer
    }
    buffer[i]=Serial.read(); //read next caracter from buffer
    if((buffer[i]=='\n') || (buffer[i]=='\r')){ //Fin de mensaje
      reading = false; //Fin de lectura
      if(i>0){ //No se trata de una linea vacía
        buffer[i] = 0; //Pon la marca de fin de cadena
        return true; //Lectura exitosa
      }
    }
    i=i+1;
    if(millis()-tLimit > timeout){ //Superamos el timeout de la lectura 86uS por caracter a 115200
      Serial.print("TimeOut"); //Mensaje de error
      reading = false; //Fin de lectura
      return false; //Fallo en la lectura
    }
  }
  return false;
}
// Nueva función para calcular y mostrar los factores primos
void factoresPrimos(int numero) {
  for (int i = 2; i <= numero; i++) { // Itera desde 2 hasta el número ingresado
    int exponente = 0; // Inicializa el exponente
    while (numero % i == 0) { // Mientras el número sea divisible por i
      numero /= i; // Divide el número por i
      exponente++; // Incrementa el exponente
    }
    if (exponente > 0) { // Si el exponente es mayor que 0
      Serial.print(i); // Imprime el factor primo
      Serial.print("^"); // Imprime el símbolo de exponente
      Serial.print(exponente); // Imprime el exponente
      if (numero > 1) { // Si el número aún no es 1
        Serial.print("; "); // Imprime un separador
      }
    }
  }
  Serial.println();
}
