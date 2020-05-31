#include "DHT.h"                // Library for use DHT22 humidity/temperature sensor
#include "EspMQTTClient.h"      // Library for MQTT client comunication

#define DHTPIN 2                                // DHT pin sensor  
#define DHTTYPE DHT22                           // DHT sensor type
#define humidity_topic "sensor/humidity"        // Humidity MQTT channel
#define temperature_topic "sensor/temperature"  // Temperature MQTT channel

DHT dht(DHTPIN, DHTTYPE);       // DHT sensor initialization

long lastMsg = 0;
float temp = 0.0;
float hum = 0.0;
float temperatureDiff = 0.5;    // Temperature thresold
float humidityDiff = 5.0;       // Humidity thresold
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
  
  dht.begin();          // Start sensor
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
  client.loop();

  if(!client.isConnected())
    return;

  long now = millis();
  if (now - lastMsg > checkIntervall) 
  {
     Serial.println("Read parameters");
    
     lastMsg = now;

     CheckTemperature();
     CheckHumidity();
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

bool CheckBound(float newValue, float prevValue, float maxDiff) 
{
  return !isnan(newValue) && (newValue < prevValue - maxDiff || newValue > prevValue + maxDiff);
}
