#include <DS3232RTC.h>
#include <TimeLib.h>
#include <OneWire.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,20,4);  // set the LCD address to 0x27 for a 16 chars and 2 line display

#if defined(ARDUINO) && ARDUINO >= 100
#define printByte(args)  write(args);
#else
#define printByte(args)  print(args,BYTE);
#endif

// Il terminale data del sensore è connesso
// alla porta 2 di Arduino
int DS18B20_Pin = 2;
// Imposta la comunicazione oneWire per comunicare
// con un dispositivo compatibile
OneWire ds(DS18B20_Pin);

#define BUTTON 7              // pin di input dove è collegato il pulsante  
int val = 0;                  // si userà val per conservare lo stato del pin di input  
int vecchio_val = 0;          // si userà vecchio_val per conservare lo stato del pin di input al passo precedente  
int stato = 0;                // ricorda lo stato in cui si trova il led, stato = 0 led spento, stato = 1 led acceso  

#define BUTTON_INC 6              // pin di input dove è collegato il pulsante  
int valInc = 0;                  // si userà val per conservare lo stato del pin di input  
int vecchio_valInc = 0;          // si userà vecchio_val per conservare lo stato del pin di input al passo precedente  
int statoInc = 0;                // ricorda lo stato in cui si trova il led, stato = 0 led spento, stato = 1 led acceso


#define BUTTON_DEC 5              // pin di input dove è collegato il pulsante  
int valDec = 0;                  // si userà val per conservare lo stato del pin di input  
int vecchio_valDec = 0;          // si userà vecchio_val per conservare lo stato del pin di input al passo precedente  
int statoDec = 0;                // ricorda lo stato in cui si trova il led, stato = 0 led spento, stato = 1 led acceso


void setup()
{
  lcd.init();  //initialize the lcd
  lcd.backlight();  //open the backlight 
  pinMode(BUTTON, INPUT);     // imposta il pin digitale come input  
  pinMode(BUTTON_INC, INPUT);     // imposta il pin digitale come input  
  pinMode(BUTTON_DEC, INPUT);     // imposta il pin digitale come input  
   
  lcd.setCursor ( 0, 0 );            // go to the top left corner
  lcd.print("Termometro Digitale"); // write this string on the top row
  lcd.setCursor ( 0, 2 );            // go to the third row
  lcd.print("  Loading........   "); // pad with spaces for centering


   Serial.begin(9600);
    setSyncProvider(RTC.get);   // the function to get the time from the RTC
    if(timeStatus() != timeSet) 
        Serial.println("Unable to sync with the RTC");
    else
        Serial.println("RTC has set the system time");      

  delay(5000);
 
  lcd.clear();
}

void loop() 
{
  stato = GetCurrentStatus();

  if(stato > 0)
  {
    UpdateCarretPositionAccordingWithStatus();

    int hours = hour();
    int minutes = minute();
    int seconds = second();
  
    int canIncrement = GetIncStatus();
    int canDecrement = GetDecStatus();

    if(canIncrement == 1)
    {
        if(stato == 1) hours = (hours + 1) % 24;
        if(stato == 2) minutes = (minutes + 1) % 60;
        if(stato == 3) seconds = (seconds + 1) % 60;

        setTime(hours, minutes, seconds, day(), month(), year());
        RTC.set(now());  

        UpdateTime();
    }

    if(canDecrement == 1)
    {
        if(stato == 1) 
        {
          if(hours == 0) hours = 24;
          hours = (hours - 1) % 24;
        }
        if(stato == 2) 
        {
          if(minutes == 0) minutes = 60;
          minutes = (minutes - 1) % 60;
        }
        if(stato == 3)
        {
          if(seconds == 0) seconds = 60;
          seconds = (seconds - 1) % 60;
        }

        setTime(hours, minutes, seconds, day(), month(), year());
        RTC.set(now());  

        UpdateTime();
    }

    statoInc = 0;
    statoDec = 0;
  }
  else
  {
    lcd.noBlink();
    
    UpdateTemperatureValue();
  
    UpdateTime();
    
    UpdateDate();
  }
}

int GetIncStatus() {

  int result = statoInc;
    
  valInc = digitalRead(BUTTON_INC);  // legge il valore dell'input e lo conserva  
  
  // controlla se è accaduto qualcosa  
  if ((valInc == HIGH) && (vecchio_valInc == LOW)){  
    result = 1 - result;  
    delay(15);                // attesa di 15 millisecondi  
  }   
  
  vecchio_valInc = valInc;            // ricordiamo il valore precedente di val  
  
  return result;
}  


int GetDecStatus() {

  int result = statoDec;
    
  valDec = digitalRead(BUTTON_DEC);  // legge il valore dell'input e lo conserva  
  
  // controlla se è accaduto qualcosa  
  if ((valDec == HIGH) && (vecchio_valDec == LOW)){  
    result = 1 - result;  
    delay(15);                // attesa di 15 millisecondi  
  }   
  
  vecchio_valDec = valDec;            // ricordiamo il valore precedente di val  
  
  return result;
} 


int GetCurrentStatus()
{
  int result = stato;
  val = digitalRead(BUTTON);  // legge il valore dell'input e lo conserva  
  
  // controlla se è accaduto qualcosa  
  if ((val == HIGH) && (vecchio_val == LOW))
  {  
    result = 1 + result;  
  
    if (result > 3) result = 0;
  
    delay(15);                // attesa di 15 millisecondi  
  }   
  
  vecchio_val = val;            // ricordiamo il valore precedente di val  
  return result;
}

void UpdateCarretPositionAccordingWithStatus()
{
  int cusrsorPos;
  if(stato == 1) cusrsorPos = 1; 
  if(stato == 2) cusrsorPos = 4;
  if(stato == 3) cusrsorPos = 7;

  lcd.setCursor ( cusrsorPos, 1 );
  lcd.blink();
}

void UpdateTemperatureValue()
{
    float temperature = getTemp();
    
    uint8_t i = 223;
    lcd.setCursor ( 0, 0 );
    lcd.print("Temperatura: ");
    lcd.setCursor ( 13, 0 ); 
    lcd.print(temperature);
    lcd.setCursor ( 18, 0 ); 
    lcd.printByte(i);
}

void UpdateTime()
{
    int hours = hour();
    int minutes = minute();
    int seconds = second();
  
    lcd.setCursor ( 0, 1 );
    if(hours < 10)
    {
      lcd.print("0");
      lcd.setCursor ( 1, 1 );
      lcd.print(hours);
    }
    else
      lcd.print(hours);
  
    lcd.setCursor ( 2, 1 );
    lcd.print(":");
  
    lcd.setCursor ( 3, 1 );
    if(minutes < 10)
    {
      lcd.print("0");
      lcd.setCursor ( 4, 1 );
      lcd.print(minutes);
    }
    else
      lcd.print(minutes);
  
    lcd.setCursor ( 5, 1 );
    lcd.print(":");
  
    lcd.setCursor ( 6, 1 );
    if(seconds < 10)
    {
      lcd.print("0");
      lcd.setCursor ( 7, 1 );
      lcd.print(seconds);
    }
    else
      lcd.print(seconds);
}

void UpdateDate()
{
    int days = day();
    int months = month();
    int years = year();
  
    lcd.setCursor ( 0, 2 );
    if(days < 10)
    {
      lcd.print("0");
      lcd.setCursor ( 1, 2 );
      lcd.print(days);
    }
    else
      lcd.print(days);
  
    lcd.setCursor ( 2, 2 );
    lcd.print("/");
  
    lcd.setCursor ( 3, 2 );
    if(months < 10)
    {
      lcd.print("0");
      lcd.setCursor ( 4, 2 );
      lcd.print(months);
    }
    else
      lcd.print(months);
  
    lcd.setCursor ( 5, 2 );
    lcd.print("/");
  
    lcd.setCursor ( 6, 2 );
    lcd.print(years);
}
  

//returns the temperature from one DS18B20 in DEG Celsius
float getTemp(){

  byte data[12];
  byte addr[8];
 
  if ( !ds.search(addr)) {
      //no more sensors on chain, reset search
      ds.reset_search();
      return -1000;
  }
 
  if ( OneWire::crc8( addr, 7) != addr[7]) {
      Serial.println("CRC is not valid!");
      return -1000;
  }
 
  if ( addr[0] != 0x10 && addr[0] != 0x28) {
      Serial.print("Device is not recognized");
      return -1000;
  }
 
  ds.reset();
  ds.select(addr);
  ds.write(0x44,1); // start conversion, with parasite power on at the end
 
  byte present = ds.reset();
  ds.select(addr);    
  ds.write(0xBE); // Read Scratchpad
 
  for (int i = 0; i < 9; i++) { // we need 9 bytes
    data[i] = ds.read();
  }
 
  ds.reset_search();
 
  byte MSB = data[1];
  byte LSB = data[0];
 
  float tempRead = ((MSB << 8) | LSB); //using two's compliment
  float TemperatureSum = tempRead / 16;
 
  return TemperatureSum;
 
}







