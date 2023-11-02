// Para mas informacion sobre las librerias utilizadas consultar 
// https://learn.adafruit.com/adafruit-motor-shield-v2-for-arduino/overview, 
// libreria oficial del Stepper Motor Shield proporcionado por Adafruit.

#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_MS_PWMServoDriver.h"

Adafruit_MotorShield AFMS1(0x60);  // Creamos el shield de los motores de las slits, bottom shield.
Adafruit_MotorShield AFMS2(0x61);  // Creamos el shield de los motores de las slits, top shield.

Adafruit_StepperMotor *myMotor_SIRF = AFMS1.getStepper(200, 2);   // Creamos el objeto para mover la slit SIRF
Adafruit_StepperMotor *myMotor_SELF = AFMS2.getStepper(200, 2);   // Creamos el objeto para mover la slit SELF

Adafruit_StepperMotor *myMotor_SILF = AFMS1.getStepper(200, 1);   // Creamos el objeto para mover la slit SILF
Adafruit_StepperMotor *myMotor_SELL = AFMS2.getStepper(200, 1);   // Creamos el objeto para mover la slit SELL

void setup() {
  AFMS1.begin();    //  Inicializamos ambas shields que controlan las slits del monocromador.
  AFMS2.begin();  
  
  //  Definiremos las entradas y salidas logicas para el control del motor.

    pinMode(2, OUTPUT); //  pin de salida para el control de la direccion del motor, DIR
    pinMode(3, OUTPUT); //  pin de salida para el control del movimiento del motor, STEP 
    pinMode(4, OUTPUT); //  pin de salida para el control del paso del monocromador, MS1
    pinMode(5, OUTPUT); //  pin de salida para el control del paso del monocromador, MS2
    pinMode(6, OUTPUT); //  pin de salida para el control del paso del monocromador, MS3

    pinMode(7, INPUT);  //  pin de entrada para el limit switch en el motor del espejo.

    // Definiremos la configuracion del motor en la fraccion de paso utilizado, para esto,
    // debemos colocar los pines como se muestra a continuacion,
    // siendo H - High y L - Low en el estado digital.

    // MS1 MS2 MS3 Microstep Resolution
    // L   L   L   Full Step
    // H   L   L   Half Step          Estamos trabajando en esta resolucion!
    // L   H   L   Quarter step
    // H   H   L   Eighth step
    // H   H   H   Sixteenth step

    digitalWrite(4, HIGH);
    digitalWrite(5, LOW);
    digitalWrite(6, LOW);

  myMotor_SIRF->setSpeed(20);     //  Definimos la velocidad de los motores que controlan las slits, esta esta en revoluciones
  myMotor_SELF->setSpeed(20);     //  por minuto, lo que equivaldria a 4000 pasos por minuto.
  myMotor_SILF->setSpeed(20);
  myMotor_SELL->setSpeed(20);
  
  //  Iniciaremos la comunicacion serial a 9600 bits/s
    Serial.begin(9600);
}

  
void loop() {
 //Definimos el tamaño del paso que tomara el motor.
 
 //  Primero debemos checar comunicacion entre el Arduino y la PC
 String lectura;
 float lecturai;
 int i;
 int j;
 int f_r = 10;
 
 if (Serial.available() != 0){
  switch(Serial.read()){
    case 'F':                                     //Moviemiento del monocromador, este tiene una resolucion de Full step ~0.1 nm
          lectura = Serial.readString();
          lectura.replace(String(char(13)), "");
          lectura.trim();
          
          lecturai = 0;
          lecturai = lectura.toFloat();

          Serial.print('M');    //  Indicamos que el monocromador esta en movimiento.
          
          // Definimos la direccion en la que el monocromador se movera.
            if(lecturai > 0){
              lecturai = abs(lecturai);
                digitalWrite(2, HIGH);
            }
            else{
              lecturai = abs(lecturai);
                digitalWrite(2, LOW);
            }
          //  Hacemos los pasos en el motor del monocromador, hasta que cumplamos 
          //  el numero de pasos requerido o topemos con el limit switch.
            i = 1;
            while(f_r*lecturai >= i){
              j=0;
              while(j < 2){
                digitalWrite(3, HIGH);
                delayMicroseconds(450);
                digitalWrite(3, LOW);
                delayMicroseconds(450);
                delay(5);
                Serial.print(j);
                Serial.write(';');
                j++;
              }
              i++;
              Serial.print(i);
              Serial.print('\n');
            }      
        Serial.write('o');
   break;

   case 'A':                          //  Inicializacion del monocromador, lo mandamos hasta el extremo y vemos cuando topa con
                                      //  el limit switch.
        Serial.print('M');    //  Indicamos que el motor esta en movimiento
          i = 1;
          while(digitalRead(7) == LOW){
            digitalWrite(2, HIGH);
     
            digitalWrite(3, HIGH);
            delayMicroseconds(500);
            digitalWrite(3, LOW);
            delayMicroseconds(500);
            delay(5);

            i++;              //  Verificar el numero de pasos dados durante la iteracion.
            Serial.print(i);
            Serial.print('\n');
            
          }
        Serial.write('o');  //  Mandamos la señal que el motor termino su movimiento y esta listo para el siguiente.
  break;

  // Codigo para el movimiento de las slits. Dependiendo cuales se busca mover, utilizaremos:
  // SILR: 'G', SIFR: 'H', SEFL: 'K', SELL: 'J', 

  case 'H':     // SIFR: 'H'
          lectura = Serial.readString();
          lectura.replace(String(char(13)), "");
          lectura.trim();
          
          lecturai = 0;
          lecturai = (lectura.toInt())/5;
          
          Serial.print('S');    //  Indicamos que las slits esta en movimiento
          
          // Definiremos el movimiento de las slits y su direccion.
          if (lecturai > 0){
            myMotor_SIRF->step(lecturai, FORWARD, SINGLE);
          }
          else{
            myMotor_SIRF->step(abs(lecturai), BACKWARD, SINGLE);
            }
      Serial.write('o');
  break;

  case 'K':     // SEFL: 'K'
          lectura = Serial.readString();
          lectura.replace(String(char(13)), "");
          lectura.trim();
          
          lecturai = 0;
          lecturai = (lectura.toInt())/5;
          
          Serial.print('S');    //  Indicamos que las slits esta en movimiento
          
          // Definiremos el movimiento de las slits y su direccion.
          if (lecturai > 0){
            myMotor_SELF->step(lecturai, FORWARD, SINGLE);
          }
          else{
            myMotor_SELF->step(abs(lecturai), BACKWARD, SINGLE);
            }
      Serial.write('o');
  break;

  case 'G':     // SILR: 'G'
          lectura = Serial.readString();
          lectura.replace(String(char(13)), "");
          lectura.trim();
          
          lecturai = 0;
          lecturai = (lectura.toInt())/5;
          
          Serial.print('S');    //  Indicamos que las slits esta en movimiento
          
          // Definiremos el movimiento de las slits y su direccion.
          if (lecturai > 0){
            myMotor_SILF->step(lecturai, FORWARD, SINGLE);
          }
          else{
            myMotor_SILF->step(abs(lecturai), BACKWARD, SINGLE);
            }
    Serial.write('o');
  break;

  case 'J':     // SELL: 'J'
          lectura = Serial.readString();
          lectura.replace(String(char(13)), "");
          lectura.trim();
          
          lecturai = 0;
          lecturai = (lectura.toInt())/5;
          
          Serial.print('S');    //  Indicamos que las slits esta en movimiento
        // Definiremos el movimiento de las slits y su direccion.
          if (lecturai > 0){
            myMotor_SELL->step(lecturai, FORWARD, SINGLE);
          }
          else{
            myMotor_SELL->step(abs(lecturai), BACKWARD, SINGLE);
            }
    Serial.write('o');
  break;
  
  case 'R':      // La resolucion del paso del monocromador es 0.1 nm, usando half step estamos dando dos pasos de 0.05 nm
          lectura = Serial.readString();
          lectura.replace(String(char(13)), "");
          lectura.trim();

          if(lectura.toInt() == 1){ // Es el caso para la resolucion de 1 nm, repetiremos el paso 10 veces.
            f_r = 10;
          }
          if(lectura.toInt() == 2){ // Es el caso para la resolucion de 0.5 nm, repetiremos el paso 5 veces.
            f_r = 5;
          }
          if(lectura.toInt() == 3){ // Es el caso para la resolucion de 0.2 nm, repetiremos el paso 2 veces.
            f_r = 2;
          }
          if(lectura.toInt() == 4){ // Es el caso para la resolucion de 0.1 nm, repetiremos el paso 1 veces.
            f_r = 1;
          }
    Serial.write('o');
  break;
  }
 }

}
