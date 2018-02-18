#include <AFMotor.h>
#include <NewPing.h>
#include <Servo.h>

#define TRIGGER_PIN 22    // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN 24       // Arduino pin tied to echo pin on the ultrasonic sensor.
#define MAX_DISTANCE 400  // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.

enum CarStatusIs
{
  starting,
  normal,
  obstacleFound  
};

enum TurnCarOnThe
{
  right,
  left
};

AF_DCMotor motorFL(1);
AF_DCMotor motorRL(2);
AF_DCMotor motorFR(3);
AF_DCMotor motorRR(4);

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);
Servo name_servo;
CarStatusIs currentStatus;

void setup() 
{
  Serial.begin(115200);

  name_servo.attach (10);
 
  Brake();

  LookAhead();

  currentStatus = starting;

  delay(3000);
}

void loop() 
{
    if(GetDistance() <= 20)
      currentStatus = obstacleFound;
  
    switch(currentStatus)
    {
      case starting:
        currentStatus = normal;
        GoFarward(100);
        break;
      
      case normal:
        currentStatus = normal;
        GoFarward(100);
        break;
  
      case obstacleFound:
        HandleObstacle();
        break;
    }
}

void Brake()
{
  motorFL.run(RELEASE);
  motorRL.run(RELEASE);
  motorFR.run(RELEASE);
  motorRR.run(RELEASE);
}

void GoFarward(int wait, int maxSpeed)
{
  uint8_t i;
  
  motorFL.run(BACKWARD);
  motorRL.run(BACKWARD);
  motorFR.run(FORWARD);
  motorRR.run(FORWARD);
  for (i=0; i<maxSpeed; i++) 
  {
    SetMotorsSpeed(i);  
    delay(wait);
  } 
}

void GoFarward(int maxSpeed)
{
  SetMotorsSpeed(maxSpeed);  
  motorFL.run(BACKWARD);
  motorRL.run(BACKWARD);
  motorFR.run(FORWARD);
  motorRR.run(FORWARD);
}


void GoBackward(int wait, int maxSpeed)
{
  uint8_t i;
  
  motorFL.run(FORWARD);
  motorRL.run(FORWARD);
  motorFR.run(BACKWARD);
  motorRR.run(BACKWARD);
  for (i=maxSpeed; i!= 0; i--) 
  {
    SetMotorsSpeed(i);
    delay(wait);
  } 
}

void GoBackward(int maxSpeed)
{
  SetMotorsSpeed(maxSpeed);
  motorFL.run(FORWARD);
  motorRL.run(FORWARD);
  motorFR.run(BACKWARD);
  motorRR.run(BACKWARD);
}

void TurnRight(int wait, int maxSpeed)
{  
  SetMotorsSpeed(maxSpeed);
  motorFL.run(BACKWARD);
  motorRL.run(BACKWARD);
  motorFR.run(BACKWARD);
  motorRR.run(BACKWARD);
    
  delay(wait);

  Brake();
}

void TurnLeft(int wait, int maxSpeed)
{  
  SetMotorsSpeed(maxSpeed);
  motorFL.run(FORWARD);
  motorRL.run(FORWARD);
  motorFR.run(FORWARD);
  motorRR.run(FORWARD);
    
  delay(wait);

  Brake();
}

void SetMotorsSpeed(int maxSpeed)
{
  motorFL.setSpeed(maxSpeed);  
  motorRL.setSpeed(maxSpeed);  
  motorFR.setSpeed(maxSpeed);  
  motorRR.setSpeed(maxSpeed);  
}

void HandleObstacle()
{
  Brake();
  
  delay(1000);

  GoBackward(200);
  delay(1000);

  Brake();

  delay(1000);

  if(WhereDoITurn() == left)
    TurnLeft(500, 200);
  else
    TurnRight(500, 200);  

  delay(2000);

  currentStatus = starting;
}
 
unsigned int GetDistance()
{
  delay(50);
  unsigned int uS = sonar.ping(); // Send ping, get ping time in microseconds (uS).
  unsigned int valueDist = uS / US_ROUNDTRIP_CM;    // Convert ping time to distance in cm

  Serial.print("Ping: ");
  Serial.print(valueDist);
  Serial.println("cm");

  return valueDist;
}
 
TurnCarOnThe WhereDoITurn()
{
  LookOnTheLeft();
  delay(2000);
  unsigned int leftDistance = GetDistance();
  
  LookOnTheRight();
  delay(2000);
  unsigned int rightDistance = GetDistance();

  LookAhead();
  delay(2000);

  if(leftDistance >= rightDistance) 
    return left;

  return right;
}

void LookAhead()
{
   name_servo.write(80);
}

void LookOnTheLeft()
{
    name_servo.write(160);
}

void LookOnTheRight()
{
  name_servo.write(0);
}



