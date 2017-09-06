#include "DHT.h"
#include <SPI.h>
#include "RF24.h"
#define DHTPIN 2     // what digital pin we're connected to
#define DHTTYPE DHT11   // DHT 11

RF24 radio(9,10);

unsigned long count = 0;
int sensor1 = 2;
String hum;
String temp;
String lum;
const int pinLight = 5;
int thresholdvalue = 300;

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(9600);
  radio.begin();
  dht.begin();
  radio.setPALevel(RF24_PA_MAX);
  radio.setChannel(0x76);
  radio.openWritingPipe(0xF0F0F0F0E1LL);
  radio.enableDynamicPayloads();
  radio.powerUp();
  pinMode(sensor1, INPUT);
  pinMode(2, OUTPUT);
}

void loop() {
  // Wait a few seconds between measurements.
  delay(2000);

  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
    
  // light sensor
  delay(1000);
  
  //int sensorValue = analogRead(pinLight);
  
  }

  Serial.print("\n Humidity: ");
  Serial.print(h);
  Serial.print(" %\t");
  Serial.print("\n Temperature: ");
  Serial.print(t);
  Serial.print(" *C ");

  int sensorValue = analogRead(pinLight); // le capteur de luminosité envoie des int
  Serial.print("\n indice de luminosite");
  Serial.print(sensorValue);

  /*if(sensorValue < thresholdvalue)
    {
       float d = sensorValue;
      Serial.print("\n Dark");
      Serial.print(d);
  
    }
  else
  {
    float b = sensorValue;
    Serial.print("\n Bright");
    Serial.print(b);
   
  }*/
  
    char outBuffer[32]= "";

    char buffer[10];
    //String temp = dtostrf(t,3,1,buffer);
    //String hum = dtostrf(h,3,1,buffer);
    temp += t;
    hum += h;
    //dark += d;
    //bright += b;
    //sprintf(outBuffer, "la valeur est %f",sensorValue);// transformer un int en string
    lum += sensorValue;
    String out = temp + "; " + hum + ";" + lum + " ; "; 
    out.toCharArray(outBuffer, 32); // permet de convertir les string en char
    //char b[3];
    //lum += String(sensorValue);
    //lum.toCharArray(b,3);
    radio.write(outBuffer, 32);
    //radio.write(b,3);

    
}
