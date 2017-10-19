import serial, time#importamos las librerias
arduino = serial.Serial("COM4", 9600)#cambiar COM4 por el COM en el que esta tu arduino
time.sleep(2)#espera 2 segundos para conectarse con el arduino
arduino.write(b'a')#le envia por serial una a, y si vemos el codigo del arduino sabemos que con eso se apaga
arduino.close()#cerramos la comunicacion del serial
