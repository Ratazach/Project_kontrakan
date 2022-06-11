
//RFID
#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>
// #define Relay 4
#define SS_PIN 10
#define RST_PIN 9
#define  btnLamp 3
//servo nya pin 4
//#define inBtnLamp 3
int LampCounter = 0;
int LampState = 0;
int lastLampState = 0;
int data = 0;

//int LampCounterin = 0;
//int LampStatein = 0;
//int lastLampStatein = 1;
//int datain = 0;

MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.
Servo myServo; //define servo name`
 
void setup() 
{
  Serial.begin(9600);   // Initiate a serial communication
  SPI.begin();      // Initiate  SPI bus
  mfrc522.PCD_Init();   // Initiate MFRC522
  myServo.attach(4); //servo pin
  myServo.write(0);
  Serial.println("Put your card to the reader...");
  Serial.println();
  pinMode(btnLamp, INPUT);
}
void loop() 
{
  //LAMP BUTTON CONFIGURATION
//LampState = digitalRead(btnLamp);
LampState = digitalRead(btnLamp);
if (LampState != lastLampState) {
    if (LampState == HIGH) {
      data++;
      Serial.println("on");
      Serial.print("LAMP BUTTON PUSH: ");
      Serial.println(LampCounter);
    } else {
      Serial.println("off");
    }
    delay(50);
  
  lastLampState = LampState;
  if (data > 1){data=data-2;}
    if(data==1){
      Serial.println (data); 
      delay(100);
      myServo.write(90);
      delay(500);}
   if(data == 0){
        Serial.println (data); 
        delay(100);
        myServo.write(0);
        delay(500);}
}
//
//LampStatein = digitalRead(inBtnLamp);
//if (LampStatein != lastLampStatein) {
//    if (LampStatein == HIGH) {
//      data++;
//      Serial.println("on");
//      Serial.print("LAMP BUTTON PUSH: ");
//      Serial.println(LampCounterin);
//    } else {
//      Serial.println("off");
//    }
//    delay(50);
//  
//  lastLampStatein = LampStatein;
//  if (data > 1){data=data-2;}
//    if(data==1){
//      Serial.println (data); 
//      delay(100);
//      myServo.write(90);
//      delay(500);}
//   if(data == 0){
//        Serial.println (data); 
//        delay(100);
//        myServo.write(0);
//        delay(500);}
//}

//  if (LampCounter % 2 == 0) {
//    digitalWrite(Relay1, LOW);
//    digitalWrite(Relay2, LOW);
//  } else {
//    digitalWrite(Relay1, HIGH);
//    digitalWrite(Relay2, HIGH);
//  }
  // Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent()) 
  {
    return;
  }
  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) 
  {
    return;
  }
  //Show UID on serial monitor
  Serial.print("UID tag :");
  String content= "";
  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++) 
  {
     Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
     Serial.print(mfrc522.uid.uidByte[i], HEX);
     content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
     content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  Serial.println();
  Serial.print("Message : ");
  content.toUpperCase();
  if (content.substring(1) == "73 D1 92 2E"||content.substring(1) == "A6 0A 60 83") //change here the UID of the card/cards that you want to give access
  {
    Serial.println("Authorized access");
   Serial.println();
    delay(100);
    data++;
    if (data > 1){data=data-2;}
    if(data==1){
      Serial.println (data); 
      delay(100);
      myServo.write(90);
      delay(500);}
   if(data == 0){
        Serial.println (data); 
        delay(100);
        myServo.write(0);
        delay(500);}
    }
//     if (content.substring(1) == "A6 0A 60 83") //change here the UID of the card/cards that you want to give access
//  {
//    Serial.println("Authorized access");
//   Serial.println();
//    delay(100);
//    data++;
//    if (data > 1){data=data-2;}
//    if(data==1){
//      Serial.println (data); 
//      delay(100);
//      myServo.write(90);
//      delay(500);}
//   if(data == 0){
//        Serial.println (data); 
//        delay(100);
//        myServo.write(0);
//        delay(500);}
//    }
//    
 else   {
    Serial.println(" Access denied");
    delay(1000);

  }
} 
