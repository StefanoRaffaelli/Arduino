// Hardware connections:
// NodeMCU     BMP180
// 3.3V         VIN
// GND          GND
// D1           SCL          
// D2           SDA 
// D4           DHT          
//
// Article: https://circuits4you.com/2019/03/23/esp8266-bmp180-pressure-sensor-interface/

#include "DHT.h"                // Library for use DHT22 humidity/temperature sensor
#include "EspMQTTClient.h"      // Library for MQTT client comunication
#include <SFE_BMP180.h>         // Library for use BMP180 pressure sensor
#include <Wire.h>

#define DHTPIN 2                                // DHT pin sensor  
#define DHTTYPE DHT22                           // DHT sensor type
#define humidity_topic "sensor/humidity"        // Humidity MQTT channel
#define temperature_topic "sensor/temperature"  // Temperature MQTT channel
#define pressure_topic "sensor/pressure"        // Pressure MQTT channel
#define ALTITUDE 2.0                            // Altitude in meters

SFE_BMP180 pressure;            // Pressure sensor
DHT dht(DHTPIN, DHTTYPE);       // DHT sensor initialization

long lastMsg = 0;
float temp = 0.0;
float hum = 0.0;
double currentPressure = 0.0;
float temperatureDiff = 0.5;    // Temperature thresold
float humidityDiff = 5.0;       // Humidity thresold
float pressureDiff = 10.0;      // Pressure thresold
long checkIntervall = 30000;    // Check intervall in milliseconds

EspMQTTClient client(
  "RafSasNetwork",          // WIFI username
  "Cefalonia197719742010",  // WIFI passwrod
  "192.168.178.45",         // MQTT Broker server ip
  "mqtt_user",              // Can be omitted if not needed
  "mymqttmosquito",         // Can be omitted if not needed
  "StazioneMeteoExt"        // Client name that uniquely identify your device
);

void setup() 
{
  Serial.begin(115200); // Start serial port for logging
  
  dht.begin();          // Start temperature/humidity sensor
  pressure.begin();     // Start pressure sensor
}

void onConnectionEstablished()
{
  client.subscribe(temperature_topic, [](const String & payload) 
  {
    Serial.println(payload);
  });

  client.subscribe(humidity_topic, [](const String & payload) 
  {    
    Serial.println(payload);
  });

  client.subscribe(pressure_topic, [](const String & payload) 
  {    
    Serial.println(payload);
  });
}

void loop() 
{
  client.loop();

  if(!client.isConnected())
    return;

  long now = millis();
  if (now - lastMsg > checkIntervall) 
  {
     Serial.println("Read parameters...");
    
     lastMsg = now;

     CheckTemperature();
     CheckHumidity();
     CheckPressure();
  }
}

void CheckTemperature()
{
  float newTemp = dht.readTemperature();

  if (CheckBound(newTemp, temp, temperatureDiff)) 
  {
    temp = newTemp;
      
    Serial.print("New temperature:");
    Serial.println(String(temp).c_str());
      
    client.publish(temperature_topic, String(temp).c_str(), true);
  }
}

void CheckHumidity()
{
  float newHum = dht.readHumidity();

  if (CheckBound(newHum, hum, humidityDiff)) 
  {
    hum = newHum;
      
    Serial.print("New humidity:");
    Serial.println(String(hum).c_str());
      
    client.publish(humidity_topic, String(hum).c_str(), true);
  }
}

void CheckPressure()
{ 
  char status;
  double T,P,p0;

  status = pressure.startTemperature();
  
  if (status != 0)
  {
    delay(status); // Wait for the measurement to complete:

    // Retrieve the completed temperature measurement: note that the measurement is stored in the variable T.
    // Function returns 1 if successful, 0 if failure.
    status = pressure.getTemperature(T);
    
    if (status != 0)
    {
      // Start a pressure measurement: The parameter is the oversampling setting, from 0 to 3 (highest res, longest wait).
      // If request is successful, the number of ms to wait is returned.
      // If request is unsuccessful, 0 is returned.
      status = pressure.startPressure(3);
      
      if (status != 0)
      {
        delay(status); // Wait for the measurement to complete:

        // Retrieve the completed pressure measurement: Note that the measurement is stored in the variable P.
        // Note also that the function requires the previous temperature measurement (T).
        // (If temperature is stable, you can do one temperature measurement for a number of pressure measurements.)
        // Function returns 1 if successful, 0 if failure.
        status = pressure.getPressure(P,T);
        
        if (status != 0)
        {
          // The pressure sensor returns absolute pressure, which varies with altitude.
          // To remove the effects of altitude, use the sea level function and your current altitude.
          // This number is commonly used in weather reports.
          // Parameters: P = absolute pressure in mb, ALTITUDE = current altitude in m.
          // Result: p0 = sea-level compensated pressure in mb
          p0 = pressure.sealevel(P,ALTITUDE); 
          
          if (CheckBound(p0, currentPressure, pressureDiff)) 
          {
            currentPressure = p0;
      
            Serial.print("New pressure:");
            Serial.println(String(currentPressure).c_str());
      
            client.publish(pressure_topic, String(currentPressure).c_str(), true);
          }          
        }
        else Serial.println("error retrieving pressure measurement\n");
      }
      else Serial.println("error starting pressure measurement\n");
    }
    else Serial.println("error retrieving temperature measurement\n");
  }
  else Serial.println("error starting temperature measurement\n");
}

bool CheckBound(float newValue, float prevValue, float maxDiff) 
{
  return !isnan(newValue) && (newValue < prevValue - maxDiff || newValue > prevValue + maxDiff);
}
