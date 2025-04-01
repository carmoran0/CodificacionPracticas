/*
9. El programa solicita un número como entrada y proporciona los valores de la serie
de Fibonacci inferiores al valor introducido como entrada
https://es.wikipedia.org/wiki/Sucesi%C3%B3n_de_Fibonacci
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
    Serial.print("Serie de Fibonacci menor que ");
    Serial.print(numero);
    Serial.println(":");
    fibonacci(numero);
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
// Nueva función para calcular y mostrar la serie de Fibonacci
void fibonacci(int numero) {
  int a = 0, b = 1, c;
  Serial.print(a);
  Serial.print(" ");
  while (b < numero) {
    Serial.print(b);
    Serial.print(" ");
    c = a + b;
    a = b;
    b = c;
  }
  Serial.println();
}
