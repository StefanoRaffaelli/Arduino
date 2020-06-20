// Hardware connections:
// NodeMCU     BMP180
// D4           DHT          

#include "DHT.h"                // Library for use DHT22 humidity/temperature sensor
#include "EspMQTTClient.h"      // Library for MQTT client comunication
#include <Effortless_SPIFFS.h>

#define DHTPIN 4                                // DHT pin sensor  
#define DHTTYPE DHT22                           // DHT sensor type
#define humidity_topic "sensor/my-shellyHT1/humidity"        // Humidity MQTT channel
#define temperature_topic "sensor//my-shellyHT1/temperature"  // Temperature MQTT channel

eSPIFFS fileSystem;

DHT dht(DHTPIN, DHTTYPE);       // DHT sensor initialization

float temp = 0.0;
float hum = 0.0;
float temperatureDiff = 0.5;    // Temperature thresold
float humidityDiff = 1.0;       // Humidity thresold

EspMQTTClient client(
  "RafSasNetwork",          // WIFI username
  "Cefalonia197719742010",  // WIFI passwrod
  "192.168.178.45",         // MQTT Broker server ip
  "mqtt_user",              // Can be omitted if not needed
  "mymqttmosquito",         // Can be omitted if not needed
  "MyShellyHT1"             // Client name that uniquely identify your device
);

void setup() 
{
  Serial.begin(115200); // Start serial port for logging
  
  dht.begin();          // Start temperature/humidity sensor
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
}

void loop() 
{
  fileSystem.openFromFile("/LastTemp.txt", temp);
  fileSystem.openFromFile("/LastHumi.txt", hum);

  Serial.print("Last saved temperature:");
  Serial.println(String(temp).c_str());

  Serial.print("Last saved humidity:");
  Serial.println(String(hum).c_str());

  float newTemp = dht.readTemperature();
  float newHum = dht.readHumidity();

  if (CheckBound(newTemp, temp, temperatureDiff) || CheckBound(newHum, hum, humidityDiff)) 
  {
     client.loop();
     if(!client.isConnected())
       return;
    
     temp = newTemp;
     hum = newHum;

     Serial.print("New temperature:");
     Serial.println(String(temp).c_str());

     Serial.print("New humidity:");
     Serial.println(String(hum).c_str());

     client.publish(temperature_topic, String(temp).c_str(), true);
     client.publish(humidity_topic, String(hum).c_str(), true);

     fileSystem.saveToFile("/LastTemp.txt", temp);
     fileSystem.saveToFile("/LastHumi.txt", hum);
  }

  // 30e7 corresponds to 300000000 microseconds which is equal to 300 seconds => 5 minutes.
  ESP.deepSleep(30e7); 
}

bool CheckBound(float newValue, float prevValue, float maxDiff) 
{
  return !isnan(newValue) && (newValue < prevValue - maxDiff || newValue > prevValue + maxDiff);
}
