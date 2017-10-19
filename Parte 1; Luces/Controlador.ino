#include<SPI.h>
#include "RF24.h"

RF24 radio(5,6);//se declaran los pines conectados a CE y CSN del RF24

byte direccion[][6] = {"Raspi","Sensor"};//se declaran los canales de comunicacion

struct estruc{//se crea la estructura que almacena los datos
  int ID; //esta variable dira a que modulo estamos hablando
  bool estado;
}datos;

void setup() {
  Serial.begin(9600);
  radio.begin();//se inicia el RF24

  radio.setPALevel(RF24_PA_LOW);//se pone el RF24 en bajo consumo
  radio.setRetries(15,15);//se configuran el numero de paquetes e intentos

  radio.openWritingPipe(direccion[1]);//decimos al RF24 que envie datos a el canal "Sensor"
  radio.openReadingPipe(1,direccion[0]);//y que reciva datos en el canal "Raspi"

  radio.startListening();//se pone al RF24 a la escucha de paquetes
}

void loop() {
  if(radio.available()){//si detecta paquetes
    while(radio.available()){//entra en un bucle
      radio.read(&datos, sizeof(datos));//almacena los datos recibidos en la estructura
      delay(20);

      if(datos.ID == 1){//si recibe los datos del modulo con ID 1 (en este caso esa ID la asignamos al modulo de la luz)
        if(datos.estado){//si el estado de la luz es 1
          Serial.println("luz prendida");
        }else{//si el estado de la luz es 0
          Serial.println("luz apagada");
        }}}}

  
  if(Serial.available()){//ve si hay datos en el puerto serial
    char c = Serial.read();//los almacena en una variable char
    if(c == 'p'){//si recibio una p
      Serial.println("prendiendo luces");//imprime por serial
      datos.ID = 1;//pone la ID en 1 (para enviar al modulo de la luz)
      datos.estado = 1;//pone el estado en 1 (para que la luz se prenda)
      radio.stopListening();//deja de escuchar el RF24
      if(!radio.write(&datos, sizeof(datos))) Serial.println("fallo");//envia los datos al modulo y si el envio falla avisa por serial
      radio.startListening();//vuelve a escuchar
    }else if(c == 'a'){//si recibe una a
      Serial.println("apagando luces");//imprime por serial
      datos.ID = 1;//se comunica con el modulo de la luz
      datos.estado = 0;//le envia un 0 para que la luz se apague
      radio.stopListening();//deja de escuchar el RF24
      if(!radio.write(&datos, sizeof(datos))) Serial.println("fallo");//envia los datos al modulo y si falla avisa por serial
      radio.startListening();//vuelve a escuchar
    }
  }
}
