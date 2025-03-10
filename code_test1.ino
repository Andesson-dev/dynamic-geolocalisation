#include <SoftwareSerial.h>
#include <stdio.h>
#include <string.h>

#define DEBUG true
int pon = 9;

SoftwareSerial GSM(7, 8);  // RX, TX for SIM808 module

// GPS Correction ratio
float latCorrectionRatio = 4.0552726 / 40.055133;  // Ajustement latitude
float lonCorrectionRatio = 9.7322720 / 94.065482;  // Ajustement longitude

// Function to send SMS
void sendSMS(String text) {
  GSM.print("\r");
  delay(1000); // Wait for modem response
  GSM.print("AT+CMGF=1\r"); // Set SMS mode to text
  delay(1000);
  GSM.print("AT+CMGS=\"+237688544825\"\r"); // Replace with recipient number
  delay(1000);
  GSM.print(text); // Send message text
  delay(1000);
  GSM.write(0x1A); // Send Ctrl+Z to finalize message
}

// Function to initialize GPS on SIM808
//
// NOTICE: Often we need to be outdoor to get a good result.
String getGPSInfo() {
  // Clear the GSM response buffer
  // to be able to receive only what we want
  GSM.flush();

  GSM.println("AT+CGPSINF=0"); // Get GPS information
  delay(2000);
  String gpsData = GSM.readString();

  // 403.308000,943.928900
  // Extract Latitude and Longitude from the GPS response
  int latIndex = gpsData.indexOf(",") + 1;
  int lonIndex = gpsData.indexOf(",", latIndex + 1);

  String latStr = gpsData.substring(latIndex, lonIndex);
  String lonStr = gpsData.substring(lonIndex + 1, gpsData.indexOf(",", lonIndex + 1));

  float lat = latStr.toFloat();
  float lon = lonStr.toFloat();

  // Convert GPS coordinates to decimal format
  int latDeg = int(lat / 100);
  float latMin = lat - (latDeg * 100);
  float latDecimal = latDeg + (latMin / 60);

  int lonDeg = int(lon / 100);
  float lonMin = lon - (lonDeg * 100);
  float lonDecimal = lonDeg + (lonMin / 60);

  // Calibrate the GPS coordinate base on trust data.
  latDecimal = latDecimal * latCorrectionRatio;
  lonDecimal = lonDecimal * lonCorrectionRatio;
 
  return "https://www.google.com/maps?q=" + String(latDecimal) + "," + String(lonDecimal);
}

String getGPSInfoOfTheNearestAntena(){
  // Clear the GSM response buffer
  // to be able to receive only what we want
  GSM.flush();

  GSM.println("AT+CLBS=1,1"); //Get current longitude and latitude precision
  delay(2000);
  String gpsData = GSM.readString();

  //Get Latitude and Longitude from the GPS respose
  int skip = gpsData.indexOf(",") + 1;
  int lonIndex = gpsData.indexOf(",", skip) + 1;
  int latIndex = gpsData.indexOf(",", lonIndex + 1);

  if (latIndex < lonIndex) {
    return "";
  }
    
  String latStr = gpsData.substring(lonIndex, latIndex); 
  String lonStr = gpsData.substring(latIndex + 1, gpsData.indexOf(",", latIndex + 1));

  float lat = latStr.toFloat();
  float lon = lonStr.toFloat();
  
  return "https://www.google.com/maps?q=" + String(lat) + "," + String(lon);
}


void setup() {
  // Configure the bandwidth for the communication
  Serial.begin(9600);
  GSM.begin(9600);

  Serial.println("GPRS GPS GSM Shield AT Test Start!");

  Serial.println("Initializing GPS...");
  GSM.println("AT+CGPSPWR=1"); // Turn on GPS
  delay(2000);
  GSM.print("AT+SAPBR=1,1"); // Activate bearer context
  delay(2000);
  

  sendSMS("Device has started!");
}

void loop() {
  // Simulating GPS retrieval every 10 seconds
  delay(5000);
  Serial.println(getGPSInfoOfTheNearestAntena());
}
