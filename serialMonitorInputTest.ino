//serial input (one byte at a time)
byte byteRead;

//denotes wheter current serial input byte 
//is the parameter's name (true) or its value(false)
boolean firstVal = true;

//translated serial input 
String param = "";
String paramVal = "";

void setup() {                
  Serial.begin(9600);
}

void loop() {
  if (Serial.available()) {   //  check if data has been sent from the computer: 
    byteRead = Serial.read(); // read the most recent byte 
      
    if(byteRead==59){//semicolon, ASCII 61, denotes end of command
      //print parameter and converted parameter value
      Serial.print(param); 
      Serial.print(", "); 
      
      //spd is 0-255(int); kp is proportionality (float)
      if(param=="spd"){
        Serial.println(paramVal.toInt()); 
      }else if(param=="kp"){
        Serial.println(paramVal.toFloat()); 
      }else{
        Serial.println("Invalid Parameter");
      }
      
      //reset paramters for next input command
      firstVal = true;
      param="";
      paramVal="";
      
    }else if(byteRead==61){//equal sign, ASCII 61, denotes end of parameter name
      firstVal = false;
    }else{
      //append the input byte to its appropriate string
      if(firstVal){
        param += String(char(byteRead));
      }else{
        paramVal += String(char(byteRead));
      }   
    }
  }
}
