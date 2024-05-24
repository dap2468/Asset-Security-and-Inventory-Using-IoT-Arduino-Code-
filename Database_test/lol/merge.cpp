#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include "env.h"
#define RXD2 16	//(RX2)
#define TXD2 17	//(TX2)
#define HC12 Serial2  //Hardware serial 2 on the ESP32
#define Siren 19

String device_id, power_status, movement_status, postData;
String id;
char power,movement;

const unsigned int MAX_MESSAGE_LENGTH=5;

int reset=0;

void IRAM_ATTR reset_function() {
  reset = 1;
  digitalWrite(Siren,LOW);
}

void setup() {

  pinMode(5, OUTPUT);
  digitalWrite(5, LOW);           //Normally HIGH, LOW for settings
  Serial.begin(9600);           // Serial port to computer
  HC12.begin(9600, SERIAL_8N1, RXD2, TXD2);      // Serial port to HC12
  pinMode(Siren,OUTPUT);
  pinMode(23, INPUT_PULLUP);
  attachInterrupt(23, reset_function, CHANGE);
  

  Serial.begin(9600);
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID,WIFI_PASS);

  Serial.println("Connecting");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  
  if(HC12.available()>=6)
  {
    
      
    char inByte = HC12.read();

    if(inByte=='$'){
        
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
    }

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
  
  
  
  
  
  
  // put your main code here, to run repeatedly:
  if (WiFi.status() == WL_CONNECTED){

    WiFiClient client;
    HTTPClient http;

    device_id=id;

    if (power=='0'||power=='*')
    {
      power_status="False";
    }
    else
    {
      power_status="True";
    }

    if (movement=='1'||movement=='*')
    {
      movement_status="True";
    }
    else
    {
      movement_status="False";
    }

    postData = "device_id=" + device_id + "&power_status=" + power_status + "&movement_status="+ movement_status;
    Serial.print("Sending data: ");
    Serial.println(postData);

   //Update databse
    http.begin(client, "http://172.16.190.118/posttest/sensor.php");
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    int httpCode = http.POST(postData);

    if (httpCode == HTTP_CODE_OK) 
    {
      String response = http.getString();
      Serial.println("Server response: " + response);
    } 
    
    else
    {
      Serial.print("HTTP POST request failed with error code: ");
      Serial.println(httpCode);
      if (httpCode == HTTPC_ERROR_CONNECTION_REFUSED)
      {
        Serial.println("Connection refused by the server.");
      }
      else if (httpCode == HTTP_CODE_NOT_FOUND) 
      {
        Serial.println("Server resource not found.");
      }
      else {
        Serial.println("Unknown error occurred.");
      }
    }

    http.end(); 



    //Email Alert
    http.begin(client, "http://172.16.190.118/posttest/emailalert.php");
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    
    httpCode = http.POST(postData);

    if (httpCode == HTTP_CODE_OK) 
    {
      String response = http.getString();
      Serial.println("Server response: " + response);
    } 
    
    else
    {
      Serial.print("HTTP POST request failed with error code: ");
      Serial.println(httpCode);
      if (httpCode == HTTPC_ERROR_CONNECTION_REFUSED)
      {
        Serial.println("Connection refused by the server.");
      }
      else if (httpCode == HTTP_CODE_NOT_FOUND) 
      {
        Serial.println("Server resource not found.");
      }
      else {
        Serial.println("Unknown error occurred.");
      }
    }

    http.end();
    
    delay(5000);
  }
}

