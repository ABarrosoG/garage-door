/****************************************************************************************************************
 * **************************************************************************************************************
 *  Title: Control board garage door.
 *  Título: Placa de control para puerta de garaje.
 *  Creación 07/08/2016   Revisado:    Autor: AntonioBG        Location: Seville - Spain
 *  Material: Placa WeMos D1 R2, FA 9V, Módulo 2 relés optoaclopados, una pequeña placa con conectores y 4 
 *              resistencias 10k a Pull-Down, miniFA 12v para alimentar la barrera infrarojos.
 *              Ya se disponía de motor con finales de carrera magnéticos.
 *  Ver 1.1:  - Final de carrera magnético en apertura y cierre.
 *            - Mando a distancia por smartphone with Blynk, control led de puerta abierta y aviso vía email en
 *                caso de que se encuentre abierta más de cinco minutos.
 *            - Mando a distancia RF.
 * Ver 1.2:   - Implementar sensor barrera infrarojos.
 *  Objetivo: -Primero.- Realizar la apertura y cierre de puerta garaje controlando los sensores, debido a que la
 *                        anterior placa electrónica de control murió por desgaste electrónico.
 *            -Segundo.- Al hacer uso de un dispositivo WeMos. se le introducirá implementación en IOT sobre el 
 *                        servidor Blynk.
 *   
* ***************************************************************************************************************** */

//#define BLYNK_PRINT Serial    // Comentar esto para desactivar las impresiones y ahorrar espacio
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <Wire.h>
#include <SimpleTimer.h>
unsigned long currentTime;
unsigned long lastRunTime;
unsigned int interval = (1000 * 6) * 3;  // 3 MINUTOS  tiempo de espera
unsigned int intervalParaliza = (1000 * 3);  // 30 segundos  tiempo de espera

WidgetLED led1(V1); //led rojo si puerta abierta
WidgetLED led2(V2); //led verde si puerta cerrada
WidgetTerminal terminal(V3);

bool puertaabierta = 0; //0 cerrada; 1 abierta
bool mandoSWnow;// Estado de lectura del pin mando distancia
bool paraliza;
bool conectado = false;
bool stateNow;
bool statePrev;

//CONSTANTES
const char auth[] = "xxxxxxxxxxxxxxxxxxxxxxxx";
const char* ssid = "xxxxxxxx";
const char* pass = "xxxxxxxxxxxxxxxx";
#define FCC D1
#define FCA D2
#define Infra D5
#define Mando D3
#define MotorA D6
#define MotorC D7

void setup() {
  Serial.begin(115200);
  Blynk.begin(auth, ssid, pass); //conexión
  delay(100);
  // Intentará conectarse 3 veces al servidor blynk
  int con = 0;
   do {
  conectado = Blynk.connect();
  con = con+1;
   } while (!conectado && con <=2);

  pinMode(FCC, INPUT);   // CLOSE FCC Final Carrera Puerta Cerrada //Sensor Magnético
  pinMode(FCA, INPUT);   // OPEN FCA Final Carrera Puerta Abierta //Sensor Magnético
  pinMode(Infra, INPUT);   // Sensor barrera infrarojos
  pinMode(MotorA, OUTPUT);  // Pin conectado a relé para giro motor apertura
  pinMode(MotorC, OUTPUT);  // Pin conectado a relé para giro motor cierre
  pinMode(Mando, INPUT_PULLUP);   // Mando distancia

  digitalWrite(FCC, LOW);
  digitalWrite(FCA, LOW);
  digitalWrite(Infra, LOW);
  digitalWrite(MotorA, HIGH);
  digitalWrite(MotorC, HIGH);
  digitalWrite(Mando, HIGH);

  terminal.println(F("Blynk v" BLYNK_VERSION ": Device started"));
  terminal.flush();  // y limpia caché
  Serial.println(F("Blynk v" BLYNK_VERSION ": Device started"));
}

// =================={ Virtual LEDs }=========
void checkLeds() {
  if (puertaabierta == 0) // 
  {
    led1.off();
    led2.on();
  } else {
    led1.on();
    led2.off();
  }
}

// ========{ Sensor InfraRojos }==================
void sensorIR (){

}

// ========{ Abre Puerta  }==================
void abrePuerta () {
  bool fcaSW = digitalRead(FCA); // FCA Final carrera puerta abierta
    terminal.println("Abriendo puerta");
      do {
      digitalWrite(MotorA, LOW); // MOTOR ABRE
      delay(100);
      fcaSW = digitalRead(FCA);//actualiza el final de carrera
      } while ( fcaSW == LOW );
    digitalWrite(MotorA, HIGH); // STOP MOTOR
    terminal.println("Puerta abierta fca");
    puertaabierta = 1;
    delay(300);
}
// ========{ Cierra Puerta }==================
void cierraPuerta () {
  bool fccSW = digitalRead(FCC); // FCC Final carrera puerta cerrada
    terminal.println("Cerrando puerta");
      do {
      digitalWrite(MotorC, LOW); // MOTOR CIERRA
      delay(100);
      fccSW = digitalRead(FCC); //actualiza el final de carrera
      } while ( fccSW == LOW );
    digitalWrite(MotorC, HIGH); // STOP MOTOR
    terminal.println("Puerta cerrada fcc");
    puertaabierta = 0;
    delay(300);
}
// ---------[ Widget Virtual Pin Abrir ]--------------------
BLYNK_WRITE(V9) { //Abrir
  paraliza = 1;
  stateNow = param.asInt();
      if  ((statePrev != stateNow) && (puertaabierta == 0)){
            terminal.println("Phone_Open");
            abrePuerta();
      }
  statePrev = stateNow;
  delay(40);
}
// ---------[ Widget Virtual Pin cerrar ]--------------------
BLYNK_WRITE(V10) { //Cerrar
  stateNow = param.asInt();
      if  ((statePrev != stateNow) && (puertaabierta == 1)){
            terminal.println("Phone_Close");
            cierraPuerta();
      }
  statePrev = stateNow;
  delay(40);
  paraliza = 0;
}
// ========{ Mando distancia }==================
void mandoDistancia () {
  mandoSWnow = digitalRead(Mando); // Lee estado mando
  if ((mandoSWnow == LOW) && (puertaabierta == 1) && (paraliza == 0)) {
    terminal.println("Mando cierra");
    cierraPuerta();
  }
  mandoSWnow = digitalRead(Mando); // Lee estado mando
  if ((mandoSWnow == LOW) && (puertaabierta == 0) && (paraliza == 0)) {
    terminal.println("Mando abre");
    abrePuerta();
  }
}
// ==============( Void Loop  ) ====================================
void loop(){
  if (conectado) {Blynk.run();}
    currentTime = millis();
  if (puertaabierta == 1){
      if (currentTime - lastRunTime >= interval){
        lastRunTime = currentTime;
        Blynk.email("xxxxxxx@mail.com", "GARAJE ABIERTO!", "La puerta de garaje está abierta.");
      }
  }
  if ( paraliza == 0){
        mandoDistancia();
      }
  delay(40);
  checkLeds();
}
