/*
8. Solicite un número entero como entrada y responda indicando si el número es primo
o no
Dame un número: 4
El número 4 no es primo
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
    if (esPrimo(numero)) {
      Serial.print("El número ");
      Serial.print(numero);
      Serial.println(" es primo");
    } else {
      Serial.print("El número ");
      Serial.print(numero);
      Serial.println(" no es primo");
    }
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
// Nueva función para determinar si un número es primo
boolean esPrimo(int numero) {
  if (numero <= 1) return false;
  for (int i = 2; i <= sqrt(numero); i++) {
    if (numero % i == 0) return false;
  }
  return true;
}
