#include <Arduino.h>
#define RXD2 16	//(RX2)
#define TXD2 17	//(TX2)
#define HC12 Serial2  //Hardware serial 2 on the ESP32

const unsigned int MAX_MESSAGE_LENGTH=5;

void setup() 
{
  pinMode(5, OUTPUT);
  digitalWrite(5, LOW);           //Normally HIGH, LOW for settings
  Serial.begin(9600);           // Serial port to computer
  HC12.begin(9600, SERIAL_8N1, RXD2, TXD2);      // Serial port to HC12
}

void loop() 
{
  if(HC12.available()>=5)
  {
    String id;
    char power,movement;

    for (int i = 1; i < 6;)
    {
      
      char inByte = HC12.read();

      if(inByte!='\n'){

        if (i==1)
        {
          id= char(inByte);
        }

        if (i==2)
        {
          id+= char(inByte);
        }

        if (i==3)
        {
          id+= char(inByte);
        }

        if (i==4)
        {
          movement= char(inByte);
        }

        if (i==5)
        {
          power= char(inByte);

          //MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
          Serial.print("id: ");
          Serial.print(id);
          Serial.print("\t");
          //MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
          Serial.print("movement: ");
          Serial.print(movement);
          Serial.print("\t");
          //MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
          Serial.print("power: ");
          Serial.print(power);
          Serial.println();
        }

        i++;
      }
    }
    
    
  }
    
}