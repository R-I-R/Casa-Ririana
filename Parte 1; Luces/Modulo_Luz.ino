int boton=2,rele=3;
bool actual;
unsigned long tiempo;

#include <SPI.h>
#include "RF24.h"
#include <EEPROM.h>

RF24 radio(5,6);

byte direccion[][6] = {"Raspi","Sensor"};

struct estruc{
  int ID = 1;
  bool estado = 0;
  //String canal = "LuzP1";
}datos,temp;

void setup() {
  Serial.begin(9600);
  
  pinMode(rele,OUTPUT);
  pinMode(boton,INPUT);

  datos.estado = EEPROM.read(0);
  
  radio.begin();

  radio.setPALevel(RF24_PA_LOW);
  radio.setRetries(15,15);

  radio.openWritingPipe(direccion[0]);
  radio.openReadingPipe(1,direccion[1]);
  delay(50);

  radio.write(&datos, sizeof(datos));

  radio.startListening();
}

void loop() {
  if(radio.available()){
    while(radio.available()){
      radio.read(&temp, sizeof(temp));
      if(temp.ID == datos.ID) datos = temp;
      delay(20);
    }
    if(temp.ID == datos.ID){
      radio.stopListening();
      radio.write(&datos, sizeof(datos));
      radio.startListening();
    }
    
  }

  if(datos.estado){
    digitalWrite(rele,HIGH);
  }else{
    digitalWrite(rele,LOW);
  }
  actual = digitalRead(boton);
  tiempo = millis();

  if(actual){
    while(digitalRead(boton)){
      if(tiempo+200 <= millis()){
        datos.estado = !datos.estado;
        radio.stopListening();
        radio.write(&datos, sizeof(datos));
        radio.startListening();
        Serial.println("solto");
        break;
      } 
    }
    while(digitalRead(boton));
    delay(200);
  }
  
  EEPROM.write(0,datos.estado);
  delay(100);
  //Serial.println(digitalRead(boton));
 }
