#define BLYNK_TEMPLATE_ID "TMPL_KfnHFk1"
#define BLYNK_DEVICE_NAME "DeltaTMPS"
#define BLYNK_AUTH_TOKEN "mxlMDqA2Jq3vZVlToPYGQ5X7v0oGn_LK"

#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp8266.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <SPI.h>

#include <EmonLib.h> 
 
#define ANALOG_INPUT_SENSOR D2

#define ONE_WIRE_BUS D1
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

char auth[] = BLYNK_AUTH_TOKEN;

char ssid[] = "NIHAJ_HOME";  // type your wifi name
char pass[] = "aeueyoaz@1";  // type your wifi password

BlynkTimer timer;

EnergyMonitor emon1;

long wattsum = 0;
double kwh = 0;
double wh = 0;
long lastmillis = 0;
int minutes = 0;

double sensorValue1 = 0;
double sensorValue2 = 0;
int crosscount = 0;
int climb_flag = 0;
int val[100];
int max_v = 0;
double VmaxD = 0;
double VeffD = 0;
double Veff = 0;

void sendSensor()
{
  // Request temperature to all devices on the data line
  sensors.requestTemperatures(); 

  float tempC = sensors.getTempCByIndex(0);
  
  if(tempC != DEVICE_DISCONNECTED_C) 
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


  for ( int i = 0; i < 100; i++ ) {
    sensorValue1 = analogRead(A0);
    if (analogRead(A0) > 511) {
      val[i] = sensorValue1;
    }
    else {
      val[i] = 0;
    }
    delay(1);
  }

  max_v = 0;

  for ( int i = 0; i < 100; i++ )
  {
    if ( val[i] > max_v )
    {
      max_v = val[i];
    }
    val[i] = 0;
  }
  if (max_v != 0) {


    VmaxD = max_v;
    VeffD = VmaxD / sqrt(2);
    Veff = (((VeffD - 903.76) / -644.24) * -415.2) + 415.2;
  }
  else {
    Veff = 0;
  }
  Serial.print("Voltage: ");
  Serial.println(Veff);
  if(Veff>185){
  Blynk.virtualWrite(V1,Veff);
  }
  else{
    Blynk.virtualWrite(V1,0);
  }
  VmaxD = 0;


  if (millis()-lastmillis > 5000) {
    double Irms = emon1.calcIrms(1480);  // Calculate Irms only
    if (Irms < 0.3) Irms = 0;
    long watt = Irms*240.0;
    wattsum = wattsum+watt;
    minutes = minutes + 1;
    lastmillis = millis();
    Serial.print(watt);         // Apparent power
    Serial.print(" ");
    Serial.println(Irms);     
    Blynk.virtualWrite(V2,Irms);// Irms
  
  }
  if (minutes == 10) {
    wh = wh + wattsum/10;
    kwh = wh/1000;
    
    wattsum = 0;
    minutes = 0;
  }
    
}

void setup() {
  Serial.begin(115200);
  sensors.begin();
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);
  Blynk.begin(auth, ssid, pass);
  emon1.current(ANALOG_INPUT_SENSOR, 25); 
  timer.setInterval(100L, sendSensor);
}

void loop() {
    Blynk.run();
    timer.run();
    delay(100);
}
