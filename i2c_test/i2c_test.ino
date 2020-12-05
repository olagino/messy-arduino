
#include <Wire.h>

void setup()
{
  Wire.begin();

  Serial.begin(115200);
  Serial.println("Sensorscan");
}

void loop()
{
  byte addr;
  byte test;
  Serial.println("SUCHE");
  for (addr = 1; addr < 127; addr++)
  {
    Wire.beginTransmission(addr);
    test= Wire.endTransmission();

    if (test == 0)
    {
      Serial.print("Sensor bei Adresse gefunden: HEX_");
      Serial.println(address, HEX);
    }
  }
  delay(1000);
}
