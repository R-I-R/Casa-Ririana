import serial, time#importamos las librerias
arduino = serial.Serial("COM4", 9600)#cambiar COM4 por el COM en el que esta tu arduino
time.sleep(2)#espera 2 segundos para conectarse con el arduino
arduino.write(b'p')#le envia por serial una p, y si vemos el codigo del arduino sabemos que con eso prende
arduino.close()#cerramos la comunicacion del serial
