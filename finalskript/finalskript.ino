#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h> // https://github.com/adafruit/Adafruit_BME280_Library
#include "ClosedCube_HDC1080.h" // https://github.com/closedcube/ClosedCube_HDC1080_Arduino/

#define MEASURECOUNT 8
#define DELAY 10

Adafruit_BME280 bme_innen; 
Adafruit_BME280 bme_aussen;

ClosedCube_HDC1080 hdc1080; 

//BME
float temp_i[MEASURECOUNT];
float temp_a[MEASURECOUNT];

float hum_i[MEASURECOUNT];
float hum_a[MEASURECOUNT];

float pres_i[MEASURECOUNT];
float pres_a[MEASURECOUNT];

//HDC1080
float temp_hdc[MEASURECOUNT];
float hum_hdc[MEASURECOUNT];

//Mittelwert
float m_temp_i = 0.0;
float m_hum_i= 0.0;

float m_temp_a = 0.0;
float m_hum_a = 0.0;

float m_temp_hdc = 0.0;
float m_hum_hdc = 0.0;

float m_pres_i = 0.0;
float m_pres_a = 0.0;

//StdDev 
float std_temp_i = 0.0;
float std_hum_i= 0.0;

float std_temp_a = 0.0;
float std_hum_a = 0.0;

float std_temp_hdc = 0.0;
float std_hum_hdc = 0.0;

float std_pres_i = 0.0;
float std_pres_a = 0.0;


int i = 0;

void setup() {
    Serial.begin(115200);
    while(!Serial);
    Serial.println("Sensorbox-Test");
    Serial.println("==============");

    unsigned status;
    bme_innen.begin(0x77);
    bme_aussen.begin(0x76);

    hdc1080.begin(0x40);

    Serial.println();
    pinMode(3,OUTPUT);
}

void loop() { 
    delay(DELAY);
    digitalWrite(3, HIGH); // DEBUG
    temp_hdc[i] = (hdc1080.readTemperature());
    hum_hdc[i]  = (hdc1080.readHumidity());
    digitalWrite(3, LOW); // DEBUG
    temp_i[i] = (bme_innen.readTemperature());
    temp_a[i] = (bme_aussen.readTemperature());
    digitalWrite(3, HIGH); // DEBUG
    hum_i[i] = (bme_innen.readHumidity());    
    hum_a[i] = (bme_aussen.readHumidity());
    digitalWrite(3, LOW); // DEBUG
    pres_i[i] = (bme_innen.readPressure() / 100.0F);
    pres_a[i] = (bme_aussen.readPressure() / 100.0F);
    digitalWrite(3, HIGH); // DEBUG
    delay(10); // DEBUG
    digitalWrite(3, LOW); // DEBUG
    i++;
    // check, if the measurement-queue is full. If this is the case start working with them.
    if(i==MEASURECOUNT){
      m_temp_i = 0;
      m_temp_a = 0;
      m_hum_a = 0;
      m_hum_i = 0;
      
      m_temp_hdc = 0;
      m_hum_hdc = 0;
      
      // calculate mean of measurement-queue/series
      for(int j = 0; j <= MEASURECOUNT-1;j++){
        // TODO for later applications:
        // Handle obvious mis-measurements like 0xFF- or sticky-Data-Registers
        m_temp_i += temp_i[j];
        m_temp_a += temp_a[j];
        m_hum_a += hum_a[j];
        m_hum_i += hum_i[j];
        m_pres_a += pres_a[j];
        m_pres_i += pres_i[j];
        
        m_temp_hdc += temp_hdc[j];
        m_hum_hdc += hum_hdc[j];
      }
      
      m_temp_i = m_temp_i/MEASURECOUNT;
      m_temp_a = m_temp_a/MEASURECOUNT;
      m_hum_a =  m_hum_a/MEASURECOUNT;
      m_hum_i =  m_hum_i/MEASURECOUNT;
      m_pres_a = m_pres_a/MEASURECOUNT;
      m_pres_i = m_pres_i/MEASURECOUNT;
      
      m_temp_hdc = m_temp_hdc/MEASURECOUNT;
      m_hum_hdc = m_hum_hdc/MEASURECOUNT;

      // calculate STD-DEViation
      for(int j = 0; j <= MEASURECOUNT-1;j++){
        std_temp_i += pow(m_temp_i - temp_i[j], 2);
        std_temp_a += pow(m_temp_a - temp_a[j], 2);
        std_hum_a += pow(m_hum_a - hum_a[j], 2);
        std_hum_i += pow(m_hum_i - hum_i[j], 2);
        std_pres_a += pow(m_pres_a - pres_a[j], 2);
        std_pres_i += pow(m_pres_i - pres_i[j], 2);
        std_temp_hdc += pow(m_temp_hdc - temp_hdc[j], 2);
        std_hum_hdc += pow(m_hum_hdc - hum_hdc[j], 2);
      }

      std_temp_i = sqrt(std_temp_i / MEASURECOUNT);
      std_temp_a = sqrt(std_temp_a / MEASURECOUNT);
      std_hum_i = sqrt(std_hum_i / MEASURECOUNT);
      std_hum_a = sqrt(std_hum_a / MEASURECOUNT);
      std_pres_i = sqrt(std_pres_i / MEASURECOUNT);
      std_pres_a = sqrt(std_pres_a / MEASURECOUNT);
      
      std_temp_hdc = sqrt(std_temp_hdc / MEASURECOUNT);
      std_hum_hdc = sqrt(std_temp_hdc / MEASURECOUNT);
      
      // ouptut the data
      Serial.print("Temp Inn=");
      Serial.print(m_temp_i);
      Serial.print(" Hudy Inn=");
      Serial.print(m_hum_i);
      Serial.print(" Pres Inn=");
      Serial.println(m_pres_i);
      
      Serial.print("Temp Auß=");
      Serial.print(m_temp_a);
      Serial.print(" Hudy Auß=");
      Serial.print(m_hum_a);
      Serial.print(" Pres Auß=");
      Serial.println(m_pres_a);

      Serial.print("Temp HDC=");
      Serial.print(m_temp_hdc);
      Serial.print(" Hudy HDC=");
      Serial.println(m_hum_hdc);
      
      Serial.println("-------------------------");
      Serial.print("T");
      Serial.print(std_temp_i);
      Serial.print(" H");
      Serial.print(std_hum_i);
      Serial.print(" P");
      Serial.print(std_pres_i);
      Serial.println();
      Serial.print("T");
      Serial.print(std_temp_a);
      Serial.print(" H");
      Serial.print(std_hum_a);
      Serial.print(" P");
      Serial.print(std_pres_a);
      Serial.println();
      Serial.print("T");
      Serial.print(std_temp_hdc);
      Serial.print(" H");
      Serial.print(std_hum_hdc);
      Serial.println();
      Serial.println("#########################");
      Serial.println();
      Serial.println();
      delay(60000);
      i = 0;
    }
}
