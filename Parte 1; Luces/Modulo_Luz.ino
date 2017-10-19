int boton=2,rele=3;//declaramos los pines
bool actual;//esta variable medira el pulsador
unsigned long tiempo;//esta variable almacena el tiempo

#include <SPI.h>
#include "RF24.h" //deben decargar esta libreria -> Programa -> Incluir Librerias -> Gestor de librerias -> RF24 by TMRh20
#include <EEPROM.h>//con esta libreria podemos usar la memoria EEPROM del arduino la cual no se borra si el arduino se apaga
//la usaremos para almacenar el estado de la luz y de esta forma si el arduino se apaga, al prenderse dejara la luz como estaba antes

RF24 radio(5,6); //con esto decimos los pines que van a CE y CSN

byte direccion[][6] = {"Raspi","Sensor"}; //aqui declaramos los canales de comunicacion

struct estruc{//esta estructura es la que se envia y recive
  int ID = 1; // ID es la variable que va a decir que modulo es. Cada modulo debe llevar una ID distinta
  bool estado = 0;//esta variable es la que dice si la luz esta apagada o prendida
}datos,temp;//se declaran 2 estructuras

void setup() {
  Serial.begin(9600);
  
  pinMode(rele,OUTPUT);//declaramos el rele como salida
  pinMode(boton,INPUT);//y el boton como entrada

  datos.estado = EEPROM.read(0);//leemos el estado de la luz de la memoria EEPROM
  
  radio.begin();//inicializamos el RF24

  radio.setPALevel(RF24_PA_LOW);//ponemos el RF24 en bajo consumo
  radio.setRetries(15,15);//asignamos la cantidad de intentos y de paquetes a enviar. 15 15 es lo recomendado

  radio.openWritingPipe(direccion[0]);//le decimos al RF24 que escriba los datos en el canal "Raspi"
  radio.openReadingPipe(1,direccion[1]);//y que lea datos del canal "Sensor"
  delay(50);

  radio.write(&datos, sizeof(datos));//el RF24 enviara su estado apenas se prenda

  radio.startListening();//ahora el RF24 se pone a la escucha de posibles transmiciones
}

void loop() {
  if(radio.available()){//si detecta datos en el canal
    while(radio.available()){//entrara en un blucle
      radio.read(&temp, sizeof(temp));//y almacenara los datos en la estructura temporal
      if(temp.ID == datos.ID) datos = temp;//luego compara si las ID son las mismas, de ser asi significa que los datos son para este
      delay(20);//modulo y en consecuencia igualara la estructura temporal con la de datos
    }
    if(temp.ID == datos.ID){//si las ID son las mismas
      radio.stopListening();//deja de eschuchar paquetes
      radio.write(&datos, sizeof(datos));//y envia el estado que tiene ahora la luz
      Serial.println("recivi datos");//ademas avisa por serial que recivio (el serial solo sirve si se conecta al pc, lo uso para testear)
      radio.startListening();//vuelve a escuchar
    }
    
  }

  if(!datos.estado){//si el estado de la luz es 0
    digitalWrite(rele,HIGH);//arduino activara el rele provocando que la luz se apague
  }else{//y si el estado de la luz es 1
    digitalWrite(rele,LOW);//arduino desactivara el rele provocando que la luz se prenda
  }
  actual = digitalRead(boton);//actual almacena las lectura del boton
  tiempo = millis();//y tiempo almacena el tiempo que lleva arduino encendido

  if(actual){//si el boton se presiona
    while(digitalRead(boton)){//entra en un bucle hasta que el boton se suelte
      if(tiempo+200 <= millis()){//si el boton lleva presionado mas de 200 milisegundos
        datos.estado = !datos.estado;//el estado de la luz se cambia
        radio.stopListening();//se deja de escuchar
        radio.write(&datos, sizeof(datos));//se envia el nuevo estado de la luz
        radio.startListening();//se vuelve a escuchar
        Serial.println("boton se presiono");//se avisa por serial 
        break;//se sale del bucle
      } //el boton se debe presionar por un rato debido a que si pongo que se cambie el estado de la luz apenas se presiona el boton
    }//los pulsos espontaneos que a veces arduino detecta harian que la luz se prendiera de la nada, pero con tiempo evita que una
    //medida erronea haga un cambio en el estado, esto es porque suelen ser pulsos cortos que no duran mucho
    while(digitalRead(boton));//entra en otro bucle que termina cuando el boton se suelte (asi se evita que se vuelva loco cuando se deja presionado)
    delay(200);
  }
  
  EEPROM.write(0,datos.estado);//se escribe el estado de la luz en la memoria EEPROM
  delay(100);
  //Serial.println(digitalRead(boton)); //descomentar esta si se quiere monitorear del pc el estado continuo del boton
 }
