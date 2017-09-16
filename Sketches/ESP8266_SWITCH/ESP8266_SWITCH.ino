
// Import required libraries
#include <ESP_SSD1306.h>    // Modification of Adafruit_SSD1306 for ESP8266 compatibility
#include <Adafruit_GFX.h>   // Needs a little change in original Adafruit library (See README.txt file)
#include <SPI.h>            // For SPI comm (needed for not getting compile error)
#include <Wire.h>           // For I2C comm, but needed for not getting compile error
#include <ESP8266WiFi.h>    // For Wifi support

// Pin definitions
#define OLED_RESET  16            // Pin 15 -RESET digital signal
ESP_SSD1306 display(OLED_RESET);  // FOR I2C
const char* ssid = "RaffaelliG";
const char* password = "Cefalonia@740";
WiFiServer server(80);
int ledPin = 13; // GPIO13

String ipStatus = "";
String switchStatus = "";
String signalStrengths = "Signal: - ";

int delaySignalStrengths = 5000;
int delaySignalStrengthsCounter = delaySignalStrengths;

int valuePin = LOW;

void setup(void)
{
  // Start Serial
  Serial.begin(115200);

  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  // SSD1306 Init
  display.begin(SSD1306_SWITCHCAPVCC);  // Switch OLED

  // Show image buffer on the display hardware.
  // Since the buffer is intialized with an Adafruit splashscreen
  // internally, this will display the splashscreen.
  display.display();
  delay(2000);
  // Clear the buffer.
  display.clearDisplay();

  ipStatus = "IP: Connecting...";
  switchStatus = "Status: OFF";
  UpdateOled();
  
  // Connect to WiFi network
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  
  ipStatus = "IP: Acquiring...";
  UpdateOled();
 
  // Start the server
  server.begin();

  UpdateRSSI(true);

  ipStatus = "IP: " + WiFi.localIP().toString();
  UpdateOled();
}

void UpdateRSSI(bool forceRefresh)
{
  if(delaySignalStrengthsCounter > 0 && !forceRefresh)
  {
    delaySignalStrengthsCounter = delaySignalStrengthsCounter -1;
    return;
  }

  delaySignalStrengthsCounter = delaySignalStrengths;
  
  long value = WiFi.RSSI();

  if( value >= -30)
   signalStrengths = "Signal: Amazing";

  if( value < -30 && value >= -67)
   signalStrengths = "Signal: Very Good";

  if( value < -67 && value >= -70)
   signalStrengths = "Signal: Okay";

  if( value < -70 && value >= -80)
   signalStrengths = "Signal: Not Good";

  if( value < -80 && value >= -90)
   signalStrengths = "Signal: Not Good";  
}

void UpdateOled()
{
  display.setTextSize(1);
  display.setTextColor(WHITE);
  
  display.clearDisplay();
   
  display.setCursor(2,2);
  display.println(ipStatus);

  display.setCursor(2,20);
  display.println(signalStrengths);
  
  display.setCursor(2,40);
  display.println(switchStatus);
  
  display.display();
}


void loop() {
  
  UpdateRSSI(false);
  
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
 
  // Wait until the client sends some data
  Serial.println("new client");
  while(!client.available()){
    delay(1);
  }
 
  // Read the first line of the request
  String request = client.readStringUntil('\r');
  Serial.println(request);
  client.flush();
 
  // Match the request 
  if (request.indexOf("/LED=ON") != -1)  {
    valuePin = HIGH;
  }
  if (request.indexOf("/LED=OFF") != -1)  {
    valuePin = LOW;
  }
 
  // Set ledPin according to the request
  digitalWrite(ledPin, valuePin);
 
  // Return the response
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println(""); //  do not forget this one
  client.println("<!DOCTYPE HTML>");
  client.println("<html>");
 
  client.print("Led pin is now: ");
 
  if(valuePin == HIGH) 
  {
    client.print("On");
    switchStatus = "Status: ON";
  } 
  else 
  {
    client.print("Off");
    switchStatus = "Status: OFF";
  }

  UpdateOled();
  
  client.println("<br><br>");
  client.println("<a href=\"/LED=ON\"\"><button>Turn On </button></a>");
  client.println("<a href=\"/LED=OFF\"\"><button>Turn Off </button></a><br />");  
  client.println("</html>");
 
  delay(1);
  Serial.println("Client disonnected");
  Serial.println("");
 
}







