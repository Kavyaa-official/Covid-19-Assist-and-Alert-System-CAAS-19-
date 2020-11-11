// *****************************************************************************
//                        SOCIAL DISTANCE MODULE    
// *****************************************************************************

//pin declarations
int ldrpin = A0;
int led1 = 13;
int led2 = 12;
int led3 = 8;
int us1 = 7;
int us2 = 11;
int us3 = 10;
int us4 = 9;
int buzz1 = 2;
int buzz2 = 3;
int buzz3 = 4;

//This function is used to transmit ultrasonic wave and returns the duration of echo signal
long readUltrasonicDistance(int triggerPin, int echoPin)
{
  pinMode(triggerPin, OUTPUT);  // Clear the trigger
  digitalWrite(triggerPin, LOW);
  delayMicroseconds(2);
  // Sets the trigger pin to HIGH state for 10 microseconds
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin, LOW);
  pinMode(echoPin, INPUT);
  // Reads the echo pin, and returns the sound wave travel time in microseconds
  return pulseIn(echoPin, HIGH);
}

void setup()
{
  pinMode(led1, OUTPUT); //setting up in output mode
  pinMode(led2, OUTPUT); //setting up in output mode
  pinMode(led3, OUTPUT); //setting up in output mode
  Serial.begin(9600); //set baud rate for Serial Monitor
}

void loop()
{
  
  //Calculating the distance of person from four sensors
  int distance_p1_s1 = 0.01723 * readUltrasonicDistance(us1, us1);
  int distance_p2_s2 = 0.01723 * readUltrasonicDistance(us2, us2);
  int distance_p3_s3 = 0.01723 * readUltrasonicDistance(us3, us3);
  int distance_p4_s4 = 0.01723 * readUltrasonicDistance(us4, us4);

  //Calculating the distance between two people
  int dist_p1_p2 = distance_p1_s1 + distance_p2_s2;
  int dist_p2_p3 = distance_p3_s3 + (182.88 - distance_p2_s2);
  int dist_p3_p4 = distance_p4_s4 + (182.88 - distance_p3_s3);
  
  //Following logic is to sense social distance violation, i.e., less than 6ft
  if(dist_p1_p2<183)
  {
    Serial.println("Distance Violation between person 1 and 2!!");
    Serial.print("Distance: ");
    Serial.print(dist_p1_p2);
    Serial.println("cm");
    tone(buzz1,500,500);
    delay(510);
    noTone(buzz1);
  }
  
  if(dist_p2_p3<183)
  {
    Serial.println("Distance Violation between person 2 and 3!!");
    Serial.print("Distance: ");
    Serial.print(dist_p2_p3);
    Serial.println("cm");
    tone(buzz2,500,500);
    delay(510);
    noTone(buzz2);
  }
  
  if(dist_p3_p4<183)
  {
    Serial.println("Distance Violation between person 3 and 4!!");
    Serial.print("Distance: ");
    Serial.print(dist_p3_p4);
    Serial.println("cm");
    tone(buzz3,500,500);
    delay(510);
    noTone(buzz3);
  }

  //Reading LDR value
  int ldrValue = analogRead(ldrpin);  
  //If LDR value greater than 488(that is dark) we turn on the LEDs else we turn them off
  if(ldrValue>488)
  {
    digitalWrite(led1,HIGH);
    digitalWrite(led2,HIGH);
    digitalWrite(led3,HIGH);
  }
  else
  {
    digitalWrite(led1,LOW);
    digitalWrite(led2,LOW);
    digitalWrite(led3,LOW);
  }
  
}
