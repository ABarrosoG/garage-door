# Control board garage door.
Title: Control board garage door.
Título: Placa de control para puerta de garaje.
Creación 07/08/2016   Revisado:    Autor: AntonioBG        Location: Seville - Spain

Material: Placa WeMos D1 R2, FA 9V, Módulo 2 relés optoaclopados, una pequeña placa con conectores y 4 
              resistencias 10k a Pull-Down, miniFA 12v para alimentar la barrera infrarojos.
              Ya se disponía de motor con finales de carrera magnéticos.
              
  Versión 1.1:  
            - Final de carrera magnético en apertura y cierre.
            - Mando a distancia por smartphone with Blynk, control led de puerta abierta y aviso vía email en
                caso de que se encuentre abierta más de cinco minutos.
            - Mando a distancia RF.
            - Falta implementar barrera infrarojos
            
  Objetivo: -Primero.- Realizar la apertura y cierre de puerta garaje controlando los sensores, debido a que la
                      anterior placa electrónica de control murió por desgaste electrónico.
            -Segundo.- Al hacer uso de un dispositivo WeMos. se le introducirá implementación en IOT sobre el 
                        servidor Blynk.
