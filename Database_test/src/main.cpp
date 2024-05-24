#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include "env.h"


String device_id, power_status, movement_status, postData;

void setup() {

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
  // put your main code here, to run repeatedly:
  if (WiFi.status() == WL_CONNECTED){

    WiFiClient client;
    HTTPClient http;

    int power = random(18,38);// you can get the actual value from attached sensor
    int movement = random(60,90);// you can get the actual value from attached sensor
    device_id="123";
    power_status = String(power);
    movement_status = "True";

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

