
// Import required libraries
#include <ESP_SSD1306.h>    // Modification of Adafruit_SSD1306 for ESP8266 compatibility
#include <Adafruit_GFX.h>   // Needs a little change in original Adafruit library (See README.txt file)
#include <SPI.h>            // For SPI comm (needed for not getting compile error)
#include <Wire.h>           // For I2C comm, but needed for not getting compile error

// Pin definitions
#define OLED_RESET  16            // Pin 15 -RESET digital signal
ESP_SSD1306 display(OLED_RESET);  // FOR I2C

void setup(void)
{
  // Start Serial
  Serial.begin(115200);

  // SSD1306 Init
  display.begin(SSD1306_SWITCHCAPVCC);  // Switch OLED

  // Show image buffer on the display hardware.
  // Since the buffer is intialized with an Adafruit splashscreen
  // internally, this will display the splashscreen.
  display.display();
  delay(2000);
  // Clear the buffer.
  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(2,2);
  display.println("IP: 192.168.4.167");
  display.display();
}


void loop() {

}







