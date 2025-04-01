/*
6 Diseñe un programa que solicita al usuario su nombre y responda saludando al usuario por su nombre  
Hola 
¿Cómo te llamas?   
Julia  
Salida: Hola Julia que tengas un gran día
*/
/* IO with Serial clases */
#define MAXBUF 20
char buffer[MAXBUF];
boolean myReadString(int timeout);
void setup() {
  delay(700);
  Serial.begin(9600);
  delay(700);
  Serial.print("¿Cómo te llamas? \n");
}
void loop(){
  if(myReadString(50)){
    Serial.print("Hola ");
    Serial.print(buffer);
    Serial.println(" que tengas un gran día");
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
