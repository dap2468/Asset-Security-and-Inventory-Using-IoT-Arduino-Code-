#include <Arduino.h>
#define RXD2 16	//(RX2)
#define TXD2 17	//(TX2)
#define HC12 Serial2  //Hardware serial 2 on the ESP32
#define Siren 19

const unsigned int MAX_MESSAGE_LENGTH=5;

int reset=0;

void IRAM_ATTR reset_function() {
  reset = 1;
  digitalWrite(Siren,LOW);
}

void setup() 
{
  pinMode(5, OUTPUT);
  digitalWrite(5, LOW);           //Normally HIGH, LOW for settings
  Serial.begin(9600);           // Serial port to computer
  HC12.begin(9600, SERIAL_8N1, RXD2, TXD2);      // Serial port to HC12
  pinMode(Siren,OUTPUT);
  pinMode(23, INPUT_PULLUP);
  attachInterrupt(23, reset_function, CHANGE);
}

void loop() 
{
  if(HC12.available()>=6)
  {
    String id;
    char power,movement;
      
    char inByte = HC12.read();

    //if(inByte=='$'){
        
      inByte = HC12.read();
      id= char(inByte);
      

      inByte = HC12.read();
      id+= char(inByte);
      
      inByte = HC12.read();
      id+= char(inByte);

      inByte = HC12.read();
      movement= char(inByte);

      inByte = HC12.read();
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
    //}

    if (power=='0'|| movement=='1')
    {
      digitalWrite(Siren,HIGH);
    }

    
    if(reset==1)
    {
      HC12.print('&');
      Serial.println("yes");
      reset=0;
    }
  }
    
}