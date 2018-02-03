#include <AFMotor.h>
#include <Servo.h>              // Add library
Servo name_servo;               // Define any servo name

int servo_position = 0 ;
const int triggerPort = 22;
const int echoPort = 24;

void setup() {
  
name_servo.attach (10);          // Define the servo signal pins

pinMode(triggerPort, OUTPUT);
pinMode(echoPort, INPUT);

Serial.begin(9600);
Serial.print( "Sensore Ultrasuoni: ");

}

void loop() {

  /*
 for (servo_position = 70; servo_position <=180; servo_position +=1){

    name_servo.write(servo_position);
    delay(10);
  }
*/

  //porta bassa l'uscita del trigger
  digitalWrite( triggerPort, LOW );
  //invia un impulso di 10microsec su trigger
  digitalWrite( triggerPort, HIGH );
  delayMicroseconds( 10 );
  digitalWrite( triggerPort, LOW );
 
  long durata = pulseIn( echoPort, HIGH );
 
  long distanza = 0.034 * durata / 2;
 
  Serial.print("distanza: ");
 
  //dopo 38ms è fuori dalla portata del sensore
  if( durata > 38000 ){
    Serial.println("Fuori portata   ");
  }
  else{ 
  Serial.print(distanza); 
  Serial.println(" cm     ");
  }
  /*
  for (servo_position=180; servo_position >= 70; servo_position -=1){

    name_servo.write(servo_position);
    delay(10);
  }*/
}







