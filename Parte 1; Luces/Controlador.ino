#include<SPI.h>
#include "RF24.h"

RF24 radio(5,6);

byte direccion[][6] = {"Raspi","Sensor"};

struct estruc{
  int ID;
  bool estado;
  String canal;
  int j = 2;
}datos;

void setup() {
  Serial.begin(9600);
  radio.begin();

  radio.setPALevel(RF24_PA_LOW);
  radio.setRetries(15,15);

  radio.openWritingPipe(direccion[1]);
  radio.openReadingPipe(1,direccion[0]);

  radio.startListening();
}

void loop() {
  if(radio.available()){
    while(radio.available()){
      radio.read(&datos, sizeof(datos));
      delay(20);

      if(datos.ID == 1){
        if(datos.estado){
          Serial.println("luz prendida");
        }else{
          Serial.println("luz apagada");
        }
      }
      
    }
  }

  
  
  if(Serial.available()){
    String c = Serial.readString();
    if(!c.compareTo("luz on")){
      Serial.println("prendiendo luces");
      datos.ID = 1;
      datos.estado = 1;
      radio.stopListening();
      if(!radio.write(&datos, sizeof(datos))) Serial.println("fallo");
      radio.startListening();
    }else if(!c.compareTo("luz off")){
      Serial.println("apagando luces");
      datos.ID = 1;
      datos.estado = 0;
      radio.stopListening();
      if(!radio.write(&datos, sizeof(datos))) Serial.println("fallo");
      radio.startListening();
    }
  }
 }
