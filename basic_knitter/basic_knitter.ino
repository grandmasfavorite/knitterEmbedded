#include <Wire.h>
#include <Adafruit_MotorShield.h>
Adafruit_MotorShield AFMS = Adafruit_MotorShield(); 
Adafruit_DCMotor *Motor = AFMS.getMotor(1);
int motorSpeed = 100;
int cutoff = 150;

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
int dir = 1; // the direction of the carriage
int sensorValue;

void setup() {                
  Serial.begin(9600);
  AFMS.begin();
}


long lastdebouncetime = 0;
long debouncedelay = 50;
int lastval = 0; // variable used to help with debounce
int valswitch = 0; // used to help debounce
int color;
int pos = 0; // the initial position of the carriage
int maxpos = 5; // the number of needles in the bed. once pos reaches maxpos, it counts down again

// the loop routine runs over and over again forever:
void sensor_pos() {
  // read the input on analog pin 0:
  int sensorValue = analogRead(A0);
  Serial.println(sensorValue);
  if (sensorValue <= cutoff)
  {
    color = 0 ;  // the it returns zero when ur not on a mark
  }
  else if (sensorValue > cutoff)
  {
    color = 1;   // returns 1 when you're on a needle
  }
  if (color != lastval)
  {
    lastdebouncetime = millis();
  }
 if ((millis() - lastdebouncetime) > debouncedelay) {
    // whatever the reading is at, it's been there for longer
    // than the debounce delay, so take it as the actual curren
    if(valswitch != color){
      valswitch = color;
        if (dir == 1)
        {
          current_pos += 1;
          Serial.println(current_pos);
        }
        else
        {
          current_pos -= 1;
          Serial.println(current_pos);
        }
      
    }

  }

    lastval = color;
  // print out the value you read:
//  Serial.println(color);
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
  }
}

void knit_right(){
  dir = 1;
  Motor->setSpeed(motorSpeed);
  Motor->run(BACKWARD);
  while (current_pos != width)
  {
    sensor_pos();
  } //Wait to reach position 
  Motor->run(RELEASE);
}

void knit_left(){
  dir = -1;
  Motor->setSpeed(motorSpeed);
  Motor->run(FORWARD);
  while (current_pos != 0)
  {
    sensor_pos();
  } //Wait to reach position 
  Motor->run(RELEASE);
}

void loop(){
  wait_for_design();
  Serial.println('Starting_Scarf');
  while (current_row < length)
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
