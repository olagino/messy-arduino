#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include "ClosedCube_HDC1080.h"

#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_BME280 bme; // I2C
Adafruit_BME280 bme_aussen; // I2C

ClosedCube_HDC1080 hdc1080; 
unsigned long delayTime;

void setup() {
    Serial.begin(115200);
    while(!Serial);    // time to get serial running
    Serial.println(F("BME280 test"));

    unsigned status;
    
    // default settings
    //status = bme.begin();  
    // You can also pass in a Wire library object like &Wire2
    
    status = bme.begin(0x77);
    status = bme_aussen.begin(0x76);

    status = hdc1080.begin(0x40);

    
   /* if (!status) {
        Serial.println("Could not find a valid BME280 sensor, check wiring, address, sensor ID!");
        Serial.print("SensorID was: 0x"); Serial.println(bme.sensorID(),16);
        Serial.print("        ID of 0xFF probably means a bad address, a BMP 180 or BMP 085\n");
        Serial.print("   ID of 0x56-0x58 represents a BMP 280,\n");
        Serial.print("        ID of 0x60 represents a BME 280.\n");
        Serial.print("        ID of 0x61 represents a BME 680.\n");
        while (1) delay(10);
    }
    */
    Serial.println("-- Default Test --");
    delayTime = 1000;

    Serial.println();

/*
  Serial.print("Manufacturer ID=0x");
  Serial.println(hdc1080.readManufacturerId(), HEX); // 0x5449 ID of Texas Instruments
  Serial.print("Device ID=0x");
  Serial.println(hdc1080.readDeviceId(), HEX); // 0x1050 ID of the device
  
  printSerialNumber();

*/




}
int i = 0;
    static int q = 8;

    //BME
    float temp_i[q];
    float temp_a[q];

    float hum_i[q];
    float hum_a[q];

    float pres_i[q];
    float pres_a[q];

    //HDC1080
    float temp_hdc[q];
    float hum_hdc[q];

  //Mittelwert außen - innen 

    float m_temp_i;
    float m_hum_i;

    float m_temp_a;
    float m_hum_a;

    float m_temp_hdc;
    float m_hum_hdc;
    



void loop() { 
    printValues();
    delay(delayTime);
    
    temp_hdc[i] = (hdc1080.readTemperature());
    hum_hdc[i]  = (hdc1080.readHumidity());
    
    temp_i[i] = (bme.readTemperature());
    temp_a[i] = (bme_aussen.readTemperature());
    
    pres_i[i] = (bme.readPressure() / 100.0F);
    pres_a[i] = (bme_aussen.readPressure() / 100.0F);
   
    hum_i[i] = (bme.readHumidity());    
    hum_a[i] = (bme_aussen.readHumidity());
    i++;
    if(i==q){
      m_temp_i = 0;
      m_temp_a = 0;
      m_hum_a  = 0;
      m_hum_i  = 0;
      m_temp_hdc = 0;
      m_hum_hdc = 0;
      
      for(int j = 0; j <= q-1;j++){
        m_temp_i += temp_i[j];
        m_temp_a += hum_i[j];
        m_hum_a += hum_a[j];
        m_hum_i += hum_i[j];
        m_temp_hdc += temp_hdc[j];
        m_hum_hdc += hum_hdc[j];
      }
      m_temp_i = m_temp_i/q;
      m_temp_a = m_temp_a/q;
      m_hum_a =  m_hum_a/q;
      m_hum_i =  m_hum_i/q;
      m_temp_hdc = m_temp_hdc/q;
      m_hum_hdc = m_hum_hdc/q;
      
    i = 0;
    }





}
