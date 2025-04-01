#define NIPLENGHT 6
#define BLUELED 44
#define REDLED 43

int NIP[NIPLENGHT] = {8,2,2,10,10,2};

void setup() 
{

  Serial.begin(115200);
  pinMode(BLUELED,OUTPUT);

  Serial.println("BEGIN");
  for (int i =0 ; i<NIPLENGHT ;i++)
  {
    for (int j = 0; j<NIP[i];j++)
    {
      digitalWrite(BLUELED,LOW);
      delay(100);
      digitalWrite(BLUELED,HIGH);
      delay(100);
    }
    delay(1000);
  }

  Serial.println("FIN");
}

void loop() 
{
  
}
