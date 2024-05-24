#include <Arduino.h>
#include <SoftwareSerial.h>

#define xPin A1
#define yPin A2
#define zPin A3
#define inPin 7 
#define BUZZER 4
#define RESET 2

// HC-12 TX Pin, HC-12 RX Pin
SoftwareSerial HC12(11, 10);

//initializing variables
float last_x, last_y,last_z;
char movement,power='0';
String id= "123";
volatile int r_state;
int m_trig=0, p_trig=0;

//calibrated minimum and maximum Raw Ranges for each axis
int xMin = 267;
int xMax = 403;

int yMin = 263;
int yMax = 399;

int zMin = 285;
int zMax = 421;


// Multiple samples to reduce noise
const int samples = 10;

//Sensitivity value
const int sensitivity = 200;

void r_function (){
  r_state= digitalRead(RESET);

  if (r_state==LOW){
    power='1';
    movement='0';
    p_trig=0;
    m_trig=0;
  }
 
}

void setup() 
{
  p_trig=0;
  m_trig=0;
  
  pinMode(inPin,INPUT);
  pinMode(RESET, INPUT_PULLUP);
  pinMode(BUZZER,OUTPUT);

  attachInterrupt(digitalPinToInterrupt(RESET),r_function,CHANGE);
  Serial.begin(115200);
  HC12.begin(9600); 

  //Read ADC values
  int xRaw=0,yRaw=0,zRaw=0;
  for(int i=0;i<samples;i++)
  {
    xRaw+=analogRead(xPin);
    yRaw+=analogRead(yPin);
    zRaw+=analogRead(zPin);
  }

  //Avg. ADC values
  xRaw/=samples;
  yRaw/=samples;
  zRaw/=samples;

  //Convert ADC values to 'milli-Gs"
  long xMilliG = map(xRaw, xMin, xMax, -1000, 1000);
  long yMilliG = map(yRaw, yMin, yMax, -1000, 1000);
  long zMilliG = map(zRaw, zMin, zMax, -1000, 1000);

  //Store initial values for x,y and z
  last_x = xMilliG;
  last_y = yMilliG;
  last_z = zMilliG;

}


void loop() 
{
  //Read ADC values
  int xRaw=0,yRaw=0,zRaw=0;
  for(int i=0;i<samples;i++)
  {
    xRaw+=analogRead(xPin);
    yRaw+=analogRead(yPin);
    zRaw+=analogRead(zPin);
  }

  //Avg. ADC values
  xRaw/=samples;
  yRaw/=samples;
  zRaw/=samples;

  //Convert ADC values to 'milli-Gs"
  long xMilliG = map(xRaw, xMin, xMax, -1000, 1000);
  long yMilliG = map(yRaw, yMin, yMax, -1000, 1000);
  long zMilliG = map(zRaw, zMin, zMax, -1000, 1000);

  //Convert long values to float
  float x_g_value = xMilliG;
  float y_g_value = yMilliG;
  float z_g_value = zMilliG;

   //Finding the diffrnce betwwen previous mG values
  if (((x_g_value-last_x)>sensitivity) || ((last_x-x_g_value)>sensitivity) || ((y_g_value-last_y)>sensitivity) || ((last_y-y_g_value)>sensitivity) || ((z_g_value-last_z)>sensitivity) || ((last_z-z_g_value)>sensitivity))
  {
    movement='1';
  }
  
  //Storing values for next ireteration
  last_x = x_g_value;
  last_y = y_g_value;
  last_z = z_g_value;



  //Power Status
  if (digitalRead(inPin)==LOW)
  {
    power='0';
  }


  if(power=='0' || movement=='1'|| power=='*'|| movement=='*'){
    digitalWrite(BUZZER, HIGH); 
  }
  else{
    digitalWrite(BUZZER, LOW); 
  }

  //Putting data together
  if(p_trig==1){
    power ='*';
  }
  if(m_trig==1){
    movement ='*';
  }

  String data= '$'+id+movement+power;
  
  // Send that data to HC-12
  HC12.print(data);
  
  if(power=='0'){
    p_trig=1;
  }

 if(movement=='1'){
    m_trig=1;
  }


  
  //Printing

  //MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
  Serial.print(x_g_value,0);
  Serial.print("mG");
  Serial.print("\t");
  //MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
  Serial.print(y_g_value,0);
  Serial.print("mG");
  Serial.print("\t");
  //MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
  Serial.print(z_g_value,0);
  Serial.print("mG");
  Serial.print("\t");
  //MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
  Serial.print("power: ");
  Serial.print(power);
  Serial.print("\t");
  //MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
  Serial.print("movement: ");
  Serial.print(movement);
  Serial.print("\t");
  //MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
  Serial.print("id: ");
  Serial.print(id);
  Serial.print("\t");
  Serial.print("\t");
  Serial.print("\t");
  //MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
  Serial.print("data: ");
  Serial.print(data);
  Serial.println();
  

  delay(200);
}
