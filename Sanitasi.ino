 #include <Servo.h>
Servo servow;
int data = 0;
int nowState = 1;
int lastState = 1;
const int sensor = 5;
int nilai = 0;

void setup() {
servow.attach(3);
pinMode(sensor,INPUT);
Serial.begin(9600);
servow.write(70);
}

void loop() {  
nowState = digitalRead(sensor);
if(nowState != lastState){
    if(nowState == LOW){  //sensor tertrigger
      nilai++;
      }else{              //sensor no trigger
  lastState = nowState;
        }
  }
  lastState = nowState;
  if(nilai%2 == 0){
    servow.write(60);  
    }else{
    servow.write(150);
    delay(500);
    servow.write(60);
    delay(200);
    servow.write(150);
    delay(500);
    servow.write(60);
    nilai = 0;
    delay(1000);
    }
}
