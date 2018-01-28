String message; //string that stores the incoming message
 
void setup()
{
  Serial.begin(9600); //set baud rate
  Serial3.begin(9600); //set baud rate
}
 
void loop()
{
  while(Serial3.available()){
    message+=char(Serial3.read());
  }
  if(!Serial3.available())
  {
    if(message!="")
    {//if data is available
      Serial.println(message); //show the data
      message=""; //clear the data
    }
  }
  delay(100); //delay
}
