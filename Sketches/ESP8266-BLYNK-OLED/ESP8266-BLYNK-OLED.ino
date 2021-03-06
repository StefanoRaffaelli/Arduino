
// Import required libraries
#include <ESP_SSD1306.h>        // Modification of Adafruit_SSD1306 for ESP8266 compatibility
#include <Adafruit_GFX.h>       // Needs a little change in original Adafruit library (See README.txt file)
#include <SPI.h>                // For SPI comm (needed for not getting compile error)
#include <Wire.h>               // For I2C comm, but needed for not getting compile error
#include <ESP8266WiFi.h>        // For Wifi support
#include <BlynkSimpleEsp8266.h> // Blynk Library
#include "DHT.h"                // Library for use DHT22 humidity/temperature sensor
#include <SPI.h>                // Library to use SD card reader
#include <SD.h>                 // Library to use SD card reader

#define DHTPIN 2                // DHT pin sensor  
#define DHTTYPE DHT22           // DHT sensor type
#define OLED_RESET  16          // Pin 15 -RESET digital signal

File settingsFile;              // Settings file
DHT dht(DHTPIN, DHTTYPE);       // DHT sensor initialization
ESP_SSD1306 display(OLED_RESET);// For I2C oled display
BlynkTimer timerRefreshData;    // Timer for refresh data

String ipStatus = "";
bool justNotifyTemperatureAlert = false;

float invalidData = -10000;
float lastReadTemperatureValid = invalidData;
float lastReadHumidityValid = invalidData;

String ssid;
String password;
String auth; //token generated by Blynk
int temperatureToAlert;
int refreshTimerData;


void setup(void)
{ 
  Serial.begin(115200); // Start Serial comunication

  InitializeOledDisplayAndShowSplashScreen();

  ReadSettingsFromSDCard();

  ipStatus = "Connecting to WiFi...";
  DrawInitializationIpStatus();
  
  Blynk.begin(auth.c_str(), ssid.c_str(), password.c_str()); // Connect to WiFi network
 
  while (Blynk.connected() == false) {
    delay(500);
  }
  
  ipStatus = WiFi.localIP().toString();

  Blynk.notify("Temperature/Humidity Device is now ready to work.");

  timerRefreshData.setInterval(refreshTimerData * 1000, OnTimerRefreshData);
  OnTimerRefreshData();
}

void InitializeOledDisplayAndShowSplashScreen()
{
  display.begin(SSD1306_SWITCHCAPVCC);  // Switch OLED initialize
  display.display();
  delay(2000);
  display.clearDisplay();
}

void ReadSettingsFromSDCard()
{
  if (!SD.begin(15)) 
  {
    Serial.println("initialization failed!");
    return;
  }
  
  Serial.println("initialization done.");

  settingsFile = SD.open("Settings.dat");
  if (settingsFile) 
  {
    Serial.println("Settings.dat:");
    
    String settingsData = "";

    while (settingsFile.available()) 
      settingsData=settingsData + (char)settingsFile.read();

    Serial.println(settingsData);

    LoadSettingsFromString(settingsData);

    settingsFile.close();
  } 
  else 
    Serial.println("error opening Settings.dat");
}

void LoadSettingsFromString(String settingsData)
{
   int ind1 = settingsData.indexOf(';');                 
    ssid = settingsData.substring(0, ind1);      
   
    int ind2 = settingsData.indexOf(';', ind1+1 );        
    password = settingsData.substring(ind1+1, ind2);     
    
    int ind3 = settingsData.indexOf(';', ind2+1 );
    auth = settingsData.substring(ind2+1, ind3);
    
    int ind4 = settingsData.indexOf(';', ind3+1 );
    temperatureToAlert = settingsData.substring(ind3+1).toInt();

    int ind5 = settingsData.indexOf(';', ind4+1 );
    refreshTimerData = settingsData.substring(ind4+1).toInt();
  
    Serial.println(ssid);
    Serial.println(password);
    Serial.println(auth);
    Serial.println(String(temperatureToAlert));
    Serial.println(String(refreshTimerData));
}

void loop() 
{
  Blynk.run();
  timerRefreshData.run();
}

void OnTimerRefreshData()
{
  ReadTemperatureAndHumidityDataFromSensor();

  RefreshBlinkData();

  RefreshDisplayData();

  CheckIfNotifyTemperatureAlert();
}

void ReadTemperatureAndHumidityDataFromSensor()
{
  dht.begin(); //Initialize comunication with temperature sensor
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t))
    return;

  lastReadTemperatureValid = t;
  lastReadHumidityValid = h;
}

bool IsSensorDataValid()
{
  if(lastReadTemperatureValid == invalidData || lastReadHumidityValid == invalidData)
    return false;

  return true;
}

void RefreshBlinkData()
{
  if(!IsSensorDataValid())
    return;
  
  Blynk.virtualWrite(V5, lastReadTemperatureValid);
  Blynk.virtualWrite(V6, lastReadHumidityValid);
}

void CheckIfNotifyTemperatureAlert()
{
  if(!IsSensorDataValid())
    return;
  
  if(lastReadTemperatureValid < temperatureToAlert && !justNotifyTemperatureAlert)
  {
    justNotifyTemperatureAlert = true;
    Blynk.notify("Temperatura inferiore ai " + String(temperatureToAlert) + "°");
  }
  
  if(lastReadTemperatureValid > temperatureToAlert && justNotifyTemperatureAlert)
    justNotifyTemperatureAlert = false;
}

void RefreshDisplayData()
{
  display.setTextSize(1);
  display.setTextColor(WHITE);
  
  display.clearDisplay();
   
  DrawIpStatus();

  long signalValue = WiFi.RSSI();
  int bars = getBarsSignal(signalValue);
  DrawBarsSignal(bars);

  DrawTemperatureData();
  DrawHumidityData();
   
  display.display();
}

int getBarsSignal(long rssi)
{
  // 5. High quality: 90% ~= -55db
  // 4. Good quality: 75% ~= -65db
  // 3. Medium quality: 50% ~= -75db
  // 2. Low quality: 30% ~= -85db
  // 1. Unusable quality: 8% ~= -96db
  // 0. No signal
  int bars;
  
  if (rssi > -55) { 
    bars = 5;
  } else if (rssi < -55 & rssi > -65) {
    bars = 4;
  } else if (rssi < -65 & rssi > -75) {
    bars = 3;
  } else if (rssi < -75 & rssi > -85) {
    bars = 2;
  } else if (rssi < -85 & rssi > -96) {
    bars = 1;
  } else {
    bars = 0;
  }
  return bars;
}

void DrawBarsSignal(int barNumber)
{
  if(barNumber >= 1)
    display.fillRect(90, 0, 6, 8, WHITE);
  else
    display.drawRect(90, 0, 6, 8, WHITE);

  if(barNumber >= 2)  
    display.fillRect(98, 0, 6, 8, WHITE);
  else
    display.drawRect(98, 0, 6, 8, WHITE);

  if(barNumber >= 3)  
    display.fillRect(106, 0, 6, 8, WHITE);
  else
     display.drawRect(106, 0, 6, 8, WHITE);
  
  if(barNumber >= 4)  
    display.fillRect(114, 0, 6, 8, WHITE);
  else
    display.drawRect(114, 0, 6, 8, WHITE);

  if(barNumber >= 5)  
    display.fillRect(122, 0, 6, 8, WHITE);
  else
    display.drawRect(122, 0, 6, 8, WHITE);  
}

void DrawInitializationIpStatus()
{
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.clearDisplay();
   
  DrawIpStatus();

  display.display();
}

void DrawIpStatus()
{
   display.setCursor(2,2);
   display.println(ipStatus);
}

void DrawTemperatureData()
{
  String temperature = "-";
  if(IsSensorDataValid())
    temperature = String(lastReadTemperatureValid, 1) + " *C";

  display.setCursor(2, 20);
  display.println("Temperature: " + temperature);
}

void DrawHumidityData()
{
 String humidity = "-";
  if(IsSensorDataValid())
    humidity = String(lastReadHumidityValid, 1) + " %";
  
  display.setCursor(2, 38);
  display.println("Humidity: " + humidity);
}






