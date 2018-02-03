//Giuseppe Caccavale
 
#include <NewPing.h>
#define TRIGGER_PIN 22 // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN 24 // Arduino pin tied to echo pin on the ultrasonic sensor.
#define MAX_DISTANCE 400 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.
 
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);
 
int SetDistance = 0;
int ValueDist = 0;
 
void setup() {
 Serial.begin(115200);
}
 
void loop() {
 delay(50); // Wait 50ms between pings (about 20 pings/sec). 29ms should be the shortest delay between pings.
 unsigned int uS = sonar.ping(); // Send ping, get ping time in microseconds (uS).
 ValueDist = uS / US_ROUNDTRIP_CM;
 
 Serial.print("Ping: ");
 Serial.print(ValueDist); // Convert ping time to distance in cm and print result (0 = outside set distance range)
 Serial.println("cm");
 
}
