
// Import required libraries
#include <ESP8266WiFi.h>        // For Wifi support
#include <BlynkSimpleEsp8266.h> // Blynk Library


int ledPin = 13; // GPIO13


void setup(void)
{ 
  Serial.begin(115200); // Start Serial comunication

  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
  
  Blynk.begin("484ddab72f2043288475562392a10258", "RaffaelliG", "Cefalonia@740"); // Connect to WiFi network
 
  while (Blynk.connected() == false) {
    delay(500);
  }

  Blynk.notify("Garden lighting manager: device is now ready to work.");
}

void loop() 
{
  Blynk.run();
}


BLYNK_WRITE(V2) {
  int state = param.asInt();
  if (state) {
     Blynk.notify("Garden lighting manager: you enter in the home area");
      digitalWrite(ledPin, HIGH);
  } else {
      Blynk.notify("Garden lighting manager: you leave the home area");
  }
}

BLYNK_WRITE(V1) {
  int state = param.asInt();
  if (state) {
      digitalWrite(ledPin, HIGH);
  } else {
      digitalWrite(ledPin, LOW);
  }
}
