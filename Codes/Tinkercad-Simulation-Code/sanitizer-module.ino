//  *********************************************************************************************************
//					SANITIZER MODULE
// **********************************************************************************************************
// importing necessary libraries
#include "SoftwareSerial.h"
#include "Arduino.h"
#include <IRremote.h>
#include <Servo.h>

// initializing system states as 0,1,2,3 for the variable flag
#define initial_state 0  // initial_state (before a person entering/going out)
#define pir_one_detected 1  // last sensor which was detected was PIR 1
#define ir_detected 2	  // last sensor which was detected was IR
#define pir_two_detected 3  // last sensor which was detected was PIR 2

//connection info for WiFi router
String ssid = "my_SSID";
String password = "my_password";

//create SoftSerial object for communication with ESP8266
SoftwareSerial esp(2, 3); // RX, TX

String data; 					// data for the POST request
String server = "http://caas19.loca.lt"; 	// Server hostname
String uri = "/updateMessage"; 			// API route

// initializing variables
Servo servo;
int pirSensor1 = 12;
int pirSensor2 = 8;
int buzzer = 9;
int irSensor = 4;
int pirSensorState1 = 0;
int pirSensorState2 = 0;
int irSensorState = 0;
int flag = initial_state;

// initializing IR remote
IRrecv irrecv(irSensor);
decode_results results;

//reset function to reset the esp8266 module
void reset() {
	esp.println("AT+RST");
	delay(3000);
	if (esp.find("OK"))
		Serial.println("Module Reset");
}

//connectWifi function to connect ESP8266 to local wifi router
void connectWifi() {
	String cmd = "AT+CWJAP=\"" + ssid + "\",\"" + password + "\"";
	esp.println(cmd);
	delay(4000);

	if (esp.find("OK")) {
		Serial.println("Connected!");
	} else {
		connectWifi();
		Serial.println("Cannot connect to wifi");
	}
}

// SetUp function to define baud rates, pinModes and setting up Wifi connection
void setup() {
	esp.begin(9600); 	//start SoftSerial for ESP comms
	Serial.begin(9600); 	//start regular serial for PC -> Arduino comms
	reset(); 		//call the ESP reset function
	connectWifi(); 		//call the ESP connection function

	pinMode(pirSensor1, INPUT);  //defining PIR1 sensor as input
	pinMode(pirSensor2, INPUT);  //defining PIR2 sensor as input
	pinMode(irSensor, INPUT);    //defining IR sensor as input
	pinMode(buzzer, OUTPUT);     //defining buzzer sensor as output
	servo.attach(5);            // attaching servo to pin 5
	irrecv.enableIRIn();	   // enabling IR remote
}

void loop() {
	// first PIR sensor detection part
	pirSensorState1 = digitalRead(pirSensor1);  

	  // if the system (flag) is in initial state and PIR1 is high, then the 
       // person is walking in. Setting flag to the current state
	if((flag==initial_state) && (pirSensorState1==HIGH)) 
	{
		Serial.println("--------------------------------");
		Serial.println("Outside PIR SENSOR detected, \nPerson walking in.");
		flag = pir_one_detected;
	} 
      // if the system (flag) is in pir_two_detected state and PIR1 is high,   
      // then the person is walking out. Reset flag to the initial_state
	else if((flag == pir_two_detected) && (pirSensorState1==HIGH))
	{
		Serial.println("Outside PIR SENSOR detected, \nPerson walked out.");
		Serial.println("Wait for 2 seconds...");
		flag = initial_state;
		delay(2000);
		Serial.println("Go! System is Ready..");	      
            Serial.println("-------------------------------\n");
	}
	// if the system (flag) is in ir_detected state and PIR1 is high, then the person went in, used sanitizer and was again moving out. 
	// Reset flag to the initial_state (corner case)
	else if((flag==ir_detected) && (pirSensorState1==HIGH))  
	{
	Serial.println("\nOutside PIR SENSOR detected, \nperson walking in..");
		flag = initial_state;
	}

	// IR sensor detection part
	if(irrecv.decode(&results))
	{            
		// if IR remote value is 1, meaning IR sensor detects motion and 
            // person is using sanitizer (assumption for simulation)
		if(results.value == 0xFD08F7)
		{
			Serial.println("\nIR sensor detected,");
			// case where flag is in initial state and ir detects motion 
                  // is neglected as it is not possible in real life
			if((flag==pir_one_detected) || (flag==ir_detected) || (flag==pir_two_detected))
			{
				// if flag was in pir_one_detected state, then change to 
                        // ir_detected state as IR sensor detected
				// if flag was in ir_detected state, means that person 
                        // is using sanitizer multiple times, hence change 
				// to ir_detected state
				 if((flag==pir_one_detected) || (flag==ir_detected)) {
				 	flag=ir_detected;
				 }

				 int pos;
				// Servo motor placed on sanitizer bottle to open and   
                        // close automatically according to IR detection
				// running servo from 0 to 90 degrees 
				 for(pos = 0; pos <= 90; pos += 1) 
				 {                                  
				    servo.write(pos);      
				    delay(3);                    
				 } 

				// running servo from 90 to 0 degrees
				 for(pos = 90; pos>=0; pos-=1) 
				 {                                
				    servo.write(pos);              
				    delay(3);                     
				 } 
			}
			Serial.println("Person used Sanitizer.");
		}
		irrecv.resume();
	}

	// PIR2 Sensor detection part
	pirSensorState2 = digitalRead(pirSensor2);
	if(pirSensorState2==HIGH) 
	{
		// case where pir_two_detected is repeated is discarded
		if((!(flag==pir_two_detected)) || (flag==pir_one_detected) || (flag==ir_detected) || (flag==initial_state))
		{
			if(flag==initial_state) {
				Serial.println("---------------------");  
			} else {
				Serial.println('\t');
			}

			Serial.println("Inside PIR SENSOR detected,");
		}

		// if flag is in initail_state, meaning person is moving out.
		if(flag == initial_state)
		{
			Serial.println("Person going out!\n");
			flag = pir_two_detected;
		}
		else if(flag == pir_one_detected)
		{
			Serial.println("Person walked in. ");
			Serial.println("ALERT! A person who just entered did not use Sanitizer..");
			Serial.println("Alert sent to owner's phone. \nWait for 2 seconds..");
			data = "{\"SYS_ID\":\"GUJ01\",\"PASSWORD\":\"GUJ-1@coronafighter\",\"MESSAGE\":\"SANITIZER\"}";
			httppost();
			digitalWrite(buzzer, HIGH);
			delay(1000);
			digitalWrite(buzzer, LOW); 
			flag = initial_state;
			Serial.println("Go! System is ready..");
			Serial.println("-------------------------------------------------------\n");
		}
		else if(flag == ir_detected)
		{
			Serial.println("Person walking in and has used Sanitizer, everything is fine!");
			Serial.println("Wait for 2 seconds...");
			delay(2000);
			flag = initial_state;
			Serial.println("Go! System is Ready.");
			Serial.println("-------------------------------------------------------\n");
		}
	}
}

// httppost function to make a POST request to the server
void httppost() {
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


