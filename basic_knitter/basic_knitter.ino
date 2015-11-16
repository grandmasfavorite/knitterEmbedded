#include <Wire.h>
#include <Adafruit_MotorShield.h>
Adafruit_MotorShield AFMS = Adafruit_MotorShield(); 
Adafruit_DCMotor *Motor = AFMS.getMotor(1);
int motorSpeed = 45;

//serial input (one byte at a time)
byte byteRead;

//denotes wheter current serial input byte 
//is the parameter's name (true) or its value(false)
boolean firstVal = true;

//translated serial input 
String param = "";
String paramVal = "";

int width = 0;
int length = 0;
boolean knitting;
int current_pos = 0;
int current_row = 0;
int current_color;
int sensorValue;

void setup() {                
  Serial.begin(9600);
  AFMS.begin();
}

// the loop routine runs over and over again forever:
int sensor_pos(int pos) {
  // read the input on analog pin 0:
  int sensorValue = analogRead(A0);
  Serial.println(sensorValue);
  // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V):
  float voltage = sensorValue * (5.0 / 1023.0);
  int color;
  if (voltage <= 0.18)
  {
    color = 0 ;  // the it returns zero when ur not on a mark
  }
  else if (voltage > 0.18)
  {
    color = 1;   // returns 1 when you're on a needle
  }
  if (color != current_color)
  {
    return pos+1;
    current_color = color;
    Serial.println(pos);
  }
  else
  {
    return pos;
  }
}

void check_serial(){
  if (Serial.available()) {   //  check if data has been sent from the computer: 
    byteRead = Serial.read(); // read the most recent byte
  if(byteRead==59){//semicolon, ASCII 61, denotes end of command
      //print parameter and converted parameter value
      if(param=="wid"){
        Serial.println(paramVal.toInt()); 
        width = paramVal.toInt();
      }else if(param=="len"){
        Serial.println(paramVal.toInt()); 
        length = paramVal.toInt();
      }else{
        Serial.println("Invalid Parameter");
      }
      
      //reset paramters for next input command
      firstVal = true;
      param="";
      paramVal="";
   }
   else if(byteRead==61){//equal sign, ASCII 61, denotes end of parameter name
        firstVal = false;
   }
   else{
      //append the input byte to its appropriate string
      if(firstVal){
        param += String(char(byteRead));
        Serial.println(param);
      }else{
        paramVal += String(char(byteRead));
        Serial.println(paramVal);
      }   
   }
  }
}

void wait_for_design(){
  while(length == 0)
  {
    check_serial();
    //Serial.println("Hey its working");
  }
}

void knit_right(){
  Motor->setSpeed(motorSpeed);
  Motor->run(BACKWARD);
  while (current_pos != 10)
  {
    current_pos = sensor_pos(current_pos);
  } //Wait to reach position 
  Motor->run(RELEASE);
}

void knit_left(){
  Motor->setSpeed(motorSpeed);
  Motor->run(FORWARD);
  while (current_pos != 0)
  {
    current_pos = sensor_pos(current_pos);
  } //Wait to reach position 
  Motor->run(RELEASE);
}

void loop(){
  //wait_for_design();
  Serial.println('Starting_Scarf');
  while (current_row < 10)
  {
    if (current_row %2 ==0)
    {
      knit_right();
    }
    else
    {
      knit_left();
    }
    current_row += 1;
  }
}
