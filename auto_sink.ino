int pinPompa = 8;
const int echo = 3;
const int trig = 5;
long duration; // variable for the duration of sound wave travel
int distance; // variable for the distance measurement

void setup() {
pinMode(trig, OUTPUT);
pinMode(echo, INPUT);
pinMode(pinPompa, OUTPUT);
Serial.begin(9600);
}

void loop() {
  bacaPing();
//Serial.println(distance);
 if (distance<20){
    digitalWrite(pinPompa,LOW);
    Serial.println("nyala");
    delay(1000);
  }
 else{
    digitalWrite(pinPompa,HIGH);
    delay(200);
  }
}
void bacaPing(){
  // Clears the trigPin condition
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  // Sets the trigPin HIGH (ACTIVE) for 10 microseconds
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echo, HIGH);
  // Calculating the distance
  //sound speed = 340 m/s = 0.034 cm/us
  distance = duration * 0.034 / 2; // Speed of sound wave divided by 2 (go and back)
  
  // Displays the distance on the Serial Monitor
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");
  }
