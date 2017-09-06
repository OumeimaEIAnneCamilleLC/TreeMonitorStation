//Author : Oumeima El Isbihani
//Title : Arduino side



//----------------------------------------------------------------------------------------------//
//                                      USED LIBRARIES                                          //
//----------------------------------------------------------------------------------------------//
#include<SPI.h>
#include<RF24.h>                                                            //radio transmitter
#include<Adafruit_Sensor.h>                                                 //humidity + temp sensor
#include<DHT.h>                                                             //humidity + temp sensor
#include<DHT_U.h>                                                           //humidity + temp sensor
#include "LowPower.h"                                                       //power consumption
//----------------------------------------------------------------------------------------------//
//                                      CONNECTION PINS                                         //
//----------------------------------------------------------------------------------------------//
#define DHTPIN            7                                                 //pin for to the DHT sensor
const int sensor_light = A0;                                                //pin for the light sensor

//----------------------------------------------------------------------------------------------//
//                                      DHT DEFINITION                                          //
//----------------------------------------------------------------------------------------------//
#define DHTTYPE           DHT11                                             //DHT 11 type
DHT_Unified dht(DHTPIN, DHTTYPE);                                           //DHT definition
uint32_t delayMS;                                                           //delay for the two data transmition
String resultat;
String hum;
String temp;
String dhtsensor;
String brightness;
//----------------------------------------------------------------------------------------------//
//                                      RADIO DEFINITION                                        //
//----------------------------------------------------------------------------------------------//
RF24 radio(9,10);                                                           //creation of the NRF24L01
                      //----------------------------------------------------------------------------------------------//
                      //                                      SETUP NRF24L01                                          //
                      //----------------------------------------------------------------------------------------------//
void setup_NRF24(void){
  radio.begin();
  radio.setPALevel(RF24_PA_MIN);
  radio.setChannel(0x50);
  radio.openWritingPipe(0xF0F0F0F0E1LL);
  radio.enableDynamicPayloads();
  radio.powerUp();
}

void setup_DHT11(void){
// Initialize device.
  dht.begin();

  sensor_t sensor;
  dht.temperature().getSensor(&sensor);
  dht.humidity().getSensor(&sensor);
  // Set delay between sensor readings based on sensor details.
  delayMS = sensor.min_delay / 1000;;
}
                      //----------------------------------------------------------------------------------------------//
                      //                                      GET DATA FROM DHT11                                     //
                      //----------------------------------------------------------------------------------------------//
void get_DHT11(void){
  delay(delayMS);                                                           // Delay between measurements.
  sensors_event_t event; 
  dht.humidity().getEvent(&event);                                          // Get humidity event and print its value.
  if (isnan(event.relative_humidity)) {
    Serial.println("Error reading humidity!");
  }
  else {
    //Serial.print("Humidity: ");
    //Serial.print(event.relative_humidity);
    //Serial.println("%");
    hum += event.relative_humidity;
    //Serial.println(hum);
  }
  dht.temperature().getEvent(&event);
  if (isnan(event.temperature)) {
    Serial.println("Error reading temperature!");
  }
  else {
    //Serial.print("Temperature: ");
    //Serial.print(event.temperature);
    //Serial.println(" *C");
    temp += event.temperature;
    //Serial.println(temp);
  }

}

                      //----------------------------------------------------------------------------------------------//
                      //                                      GET LIGHT                                               //
                      //----------------------------------------------------------------------------------------------//
void get_light(void) {
  int sensorValue = analogRead(sensor_light);                               //create a var to store the value of the sensor
  //Serial.println("the analog read data is ");                               //print on the serial monitor what's in the ""
  //Serial.println(sensorValue);                                              // print the value of the sensor on the serial monitor
  brightness += sensorValue;
  //Serial.println(brightness);
}

                      //----------------------------------------------------------------------------------------------//
                      //                                      GET STRING RESULT                                       //
                      //----------------------------------------------------------------------------------------------//
void get_result(void) {
  resultat += temp;
  resultat += ";";
  resultat += hum;
  resultat += ";";
  resultat += brightness;
  //Serial.println(resultat);
}

                      //----------------------------------------------------------------------------------------------//
                      //                                      CLEANING FUNCTION                                       //
                      //----------------------------------------------------------------------------------------------//
void clean(void){
  resultat.remove(0);
  hum.remove(0);
  temp.remove(0);
  brightness.remove(0);
  dhtsensor.remove(0);
}
                      //----------------------------------------------------------------------------------------------//
                      //                                      SETUP FUNCTION                                          //
                      //----------------------------------------------------------------------------------------------//
void setup(void){
  Serial.begin(9600);                                                       //initialize the serial monitor at 9600 baud rate
  setup_NRF24();
  setup_DHT11();
}

                      //----------------------------------------------------------------------------------------------//
                      //                                     POWER SAVING FUNCTION                                    //
                      //----------------------------------------------------------------------------------------------//
void sleep(void){
  //sleep for 4 minutes
  for(int i=0; i <30; i++){       // (4*60)/8
    // Enter power down state for 8 s with ADC and BOD module disabled
    LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);  
  }                  
}
                      //----------------------------------------------------------------------------------------------//
                      //                                      LOOP FUNCTION                                           //
                      //----------------------------------------------------------------------------------------------//
void loop(void){
  sleep();
  get_light();
  get_DHT11();
  get_result();
  char result[resultat.length()+1];
  resultat.toCharArray(result,resultat.length()+1);
  Serial.println(resultat);
  radio.write(&result, sizeof(result));
  clean();
  //delay(1000);
  
}

