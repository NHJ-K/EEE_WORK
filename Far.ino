#define BLYNK_TEMPLATE_ID ""
#define BLYNK_DEVICE_NAME ""
#define BLYNK_AUTH_TOKEN ""

#define BLYNK_PRINT Serial

#define ON_BLYNK V3
#define OFF_BLYNK V4

int Gas_analog = 32;

#include "EmonLib.h"   
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <SPI.h>
#include <ESP32Servo.h>
Servo servo;



#define ONE_WIRE_BUS 2 //sensor pin D4
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

char auth[] = BLYNK_AUTH_TOKEN;

char ssid[] = "**********";  // type your wifi name
char pass[] = "*********";  // type your wifi password

BlynkTimer timer;

EnergyMonitor emon1;

EnergyMonitor emon;
#define vCalibration 110
#define currCalibration 5.3

void sendSensor();

void setup() {
  Serial.begin(115200);
  sensors.begin();
  //WiFi.mode(WIFI_STA);
  //WiFi.begin(ssid, pass);
  Blynk.begin(auth, ssid, pass);
  servo.attach(5);
  servo.write(40);
  emon.voltage(34, vCalibration, 1.7); // Voltage: input pin, calibration, phase_shift
  emon.current(35, currCalibration);
  timer.setInterval(100L, sendSensor);
  pinMode(Gas_analog  , INPUT);
  Blynk.virtualWrite(V5,0);
}

void loop() {
  Blynk.run();
  timer.run();
}

//servo on off
BLYNK_WRITE(V4) {
  if(param.asInt() == 1){
  servo.write(150);
  delay(1000);
  servo.write(40);
  }
}
BLYNK_WRITE(V3) {
  if(param.asInt() == 1){
   servo.write(40);
   delay(1000);
   servo.write(0);
  }
}

void sendSensor()
{
  //gas sensor
  int gassensorAnalog = analogRead(Gas_analog);

  Serial.print("Gas Sensor: ");
  Serial.print(gassensorAnalog);
  //Serial.print("\t");
  //Serial.print("Gas Class: ");
  //Serial.print(gassensorDigital);
  //Serial.print("\t");
  //Serial.print("\t");

  if (gassensorAnalog > 1000) {
    Serial.println("Gas");
    //digitalWrite (Buzzer, HIGH) ; //send tone
    Blynk.virtualWrite(V5,1);
    delay(1000);
    //digitalWrite (Buzzer, LOW) ;  //no tone
  }
  else {
    Blynk.virtualWrite(V5,0);
    Serial.println("No Gas");
  }
  delay(100);


  // Request temperature to all devices on the data line
  sensors.requestTemperatures();

  float tempC = sensors.getTempCByIndex(0);

  if (tempC != DEVICE_DISCONNECTED_C)
  {
    Serial.print("Celsius temperature: ");
    Serial.println(tempC);
  }
  else
  {
    Serial.println("Error: Could not read temperature data");
  }
  //We can connect more than one IC on the same data wire. 0 refers to the first IC on the wire
  //int tempC=sensors.getTempCByIndex(0);
  //int tempF=sensors.getTempFByIndex(0);
  delay(1000);

  // You can send any value at any time.
  // Please don't send more that 10 values per second.
  Blynk.virtualWrite(V0, tempC);
  //Blynk.virtualWrite(V1, tempF);
  delay(500);

  emon.calcVI(20, 2000);
  Serial.print("Vrms: ");
   Serial.print(emon.Vrms,2);
   Serial.print("V");
   if(emon.Vrms>30){
   Blynk.virtualWrite(V1, emon.Vrms);
   }
   else{
    Blynk.virtualWrite(V1,0);
   }
   Serial.print("\tIrms: ");
    Serial.print(emon.Irms,4);
    Serial.print("A");
   Blynk.virtualWrite(V2.#,emon.Irms);

}
