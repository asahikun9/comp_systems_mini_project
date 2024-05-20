#include <WiFi.h>
#include "file.h"
#include "camera.h"
#include "lapse.h"
#include <HTTPClient.h>
#include "time.h"

// Global Variables
HTTPClient http;
bool sent = false;
unsigned long tmp_millis = millis();
int val = 1;


// NTP server to request epoch time
const char* ntpServer = "pool.ntp.org";

// WiFi Credentials 
const char* ssid = "Zi Xu’s iPhone";
const char* password = "chipichipichapachapa";

// Firebase server
String firebase_server = "https://miniproject-fc16f-default-rtdb.firebaseio.com//Trial2.json";

// Function that gets current epoch time
unsigned long getTime() {
  time_t now;
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    //Serial.println("Failed to obtain time");
    return(0);
  }
  time(&now);
  return now;
}

void startCameraServer();

void setup()
{
	Serial.begin(9600);
	Serial.setDebugOutput(true);
	Serial.println();
	initFileSystem();
	initCamera();

	WiFi.begin(ssid, password);
	while (WiFi.status() != WL_CONNECTED)
	{
		delay(500);
		Serial.print(".");
	}
	Serial.println("");
	Serial.println("WiFi connected");
	startCameraServer();
	Serial.print("Camera Ready! Use 'http://");
	Serial.print(WiFi.localIP());
	Serial.println("' to connect");
  configTime(0, 0, ntpServer); // add for epoch to work
}

void loop()
{
	unsigned long t = millis();
	static unsigned long ot = 0;
	unsigned long dt = t - ot;
	ot = t;
	processLapse(dt);

  // // To run the code with intervals, you cant use delay. We call this nonblocking.
  if (millis() >= tmp_millis + 1000) {
    String incomingByte;
    if (Serial.available() > 0) {
    // Read the incoming byte from DE-10 Lite
    // incomingByte = Serial.read();
    do{
      incomingByte = Serial.readStringUntil('\0');
    }while(incomingByte.length() <= 1);
    
    Serial.print("Received from DE-10 Lite: ");
    Serial.println(incomingByte);

    http.begin(firebase_server);
    http.addHeader("Content-Type", "application/json");
    //String data = "{\"Header1\":" + String(val) +"}";
    String data = "{\"ETime\":" + String(getTime()) + ",\"Number\": \"" + String(incomingByte) +"\"}";
    // char data[100];
    // sprintf(data, "{\"ETime\":%d,\"Number\": \"%c\"}", getTime(), incomingByte);
    

    Serial.println(data); // check whether your data is being sent using serial monitor
    int responsecode = http.POST(data); 
    // Checking the response code 
    Serial.print("Code: ");
    Serial.println(responsecode);
    tmp_millis = millis();
    val++;
    }
  }
}
