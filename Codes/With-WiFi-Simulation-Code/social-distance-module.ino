// *****************************************************************************
//                        SOCIAL DISTANCE MODULE    
// *****************************************************************************

#include "SoftwareSerial.h"

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

//connection info for WiFi router... change this to your wifi's ssid and password
String ssid = "my_SSID";
String password = "my_password";

//create SoftSerial object for communication with ESP8266
SoftwareSerial esp(5, 6); // RX, TX

String data;          // data for the POST request
String server = "http://caas19.loca.lt";  // Server hostname
String uri = "/updateMessage";      // API route

//reset function to reset the esp8266 module
void reset() {
  esp.println("AT+RST");
  delay(3000);

  if (esp.find("OK"))
    Serial.println("Module Reset");
}

//connectWifi function to connect ESP8266 to local wifi router
void connectWifi() 
{
  String cmd = "AT+CWJAP=\"" + ssid + "\",\"" + password + "\"";
  esp.println(cmd);
  delay(4000);

  if (esp.find("OK")) {
    Serial.println("Connected!");
  }
  else {
    connectWifi();
    Serial.println("Cannot connect to wifi");
  }
}

// httppost function to make a POST request to the server
void httppost() 
{
  //start a TCP connection.
  esp.println("AT+CIPSTART=\"TCP\",\"" + server); 

  //connected with esp successfully
  if (esp.find("OK")) {
    Serial.println("TCP connection ready");
  }
  delay(1000);

  // making postrequest string 
  String postRequest = String("POST ");
  postRequest = postRequest + uri + " HTTP/1.1\r\n" +
  "Host: " + server + "\r\n" +
  "Accept: *" + "/" + "*\r\n" +
  "Content-Length: " + data.length() + "\r\n" +
  "Content-Type: application/json\n" +
  "\r\n" + data;

  //see what the fully built POST request looks like
  Serial.println("Post Request text: "); 
  Serial.println(postRequest);

  //determine the number of caracters to be sent.
  String sendCmd = "AT+CIPSEND="; 
  esp.print(sendCmd);
  esp.println(postRequest.length());
  delay(500);

  
  if(esp.find(">")) 
  {
    // sending the post request to the API on the server
    Serial.println("Sending..");
    esp.print(postRequest);

    // printing the data to be sent 
    Serial.println("Data to send: ");
    Serial.println(data);
    // printing the length of data that is to be sent 
    Serial.print("Data length: ");
    Serial.println(data.length());
    // printing the length of the postrequest string
    Serial.print("Request length: ");
    Serial.println(postRequest.length());
    // printing the postrequest
    Serial.println("Request Sent:");
    Serial.println(postRequest);

    // if the request packet is sent, then read the response from esp
    if(esp.find("SEND OK")) 
    {
      Serial.println("Packet sent");
      while (esp.available()) 
      {
        String tmpResp = esp.readString();
        Serial.println(tmpResp);
      }
      // close the connection
      esp.println("AT+CIPCLOSE");
    }
  }
}

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
  esp.begin(9600);  //start SoftSerial for ESP comms
  reset();    //call the ESP reset function
  connectWifi();    //call the ESP connection function
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
    data = "{\"SYS_ID\":\"GUJ01\",\"PASSWORD\":\"GUJ-1@coronafighter\",\"MESSAGE\":\"DISTANCE:1\"}";
    httppost();
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
    data = "{\"SYS_ID\":\"GUJ01\",\"PASSWORD\":\"GUJ-1@coronafighter\",\"MESSAGE\":\"DISTANCE:2\"}";
    httppost();
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
    data = "{\"SYS_ID\":\"GUJ01\",\"PASSWORD\":\"GUJ-1@coronafighter\",\"MESSAGE\":\"DISTANCE:3\"}";
    httppost();
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
