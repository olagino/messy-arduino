#include <Adafruit_BME280.h>


#include <Wire.h>

long cal_temp1, cal_temp2, cal_temp3;
long cal_pres1, cal_pres2, cal_pres3, cal_pres4, cal_pres5, cal_pres6, cal_pres7, cal_pres8, cal_pres9;
long cal_hudy1, cal_hudy2, cal_hudy3, cal_hudy4, cal_hudy5, cal_hudy6;


void bme_initialize(byte addr) {
  // Sensor-Einstellungen - bevor man die nicht setzt misst er Mist
  Wire.beginTransmission(addr);
  Wire.write(0xf2);
  Wire.write(0x01);

  Wire.write(0xf4);
  Wire.write(0x27);

  Wire.write(0xf5);
  Wire.write(0xa0);
  Wire.endTransmission();
  return;
}

void bme_calibrate(byte addr) {
  // Kalibrierungsdaten lesen und "vorberechnen"
  unsigned char calib[36];


  Wire.beginTransmission(addr);
  Wire.write(0x88);
  Wire.endTransmission();

  Wire.requestFrom(addr, 24);
  for( int i = 0; i <= 23; i++) {
    calib[i] = Wire.read();
  }

  Wire.beginTransmission(addr);
  Wire.write(0xa1);
  Wire.endTransmission();

  Wire.requestFrom(addr, 1);
  calib[24] = Wire.read();

  Wire.beginTransmission(addr);
  Wire.write(0xe1);
  Wire.endTransmission();

  Wire.requestFrom(addr, 7);
  for( int i = 25; i <= 31; i++) {
    calib[i] = Wire.read();
  }

  cal_temp1 = calib[0] + (calib[1] << 8);
	cal_temp2 = calib[2] + (calib[3] << 8);
	if (cal_temp2 > 32767) {cal_temp2 -= 65536;} // negative value
	cal_temp3 = calib[4] + (calib[5] << 8);
	if (cal_temp3 > 32767) {cal_temp3 -= 65536;}

  cal_hudy1 = calib[24];
  cal_hudy2 = calib[25] + (calib[26] << 8);
  if (cal_hudy2 > 32767) {cal_hudy2 -= 65536;} // Abgleich unsigned-signed
  cal_hudy3 = calib[27];
  cal_hudy4 = (calib[28] << 4) + (calib[29] & 0xf); // 28 4 bit nach links, dazu die vier rechten bits von 29
  if (cal_hudy4 > 2047) {cal_hudy4 -= 4096;} // Ablgeich signed/unsigned, Basis "12-bit"
  cal_hudy5 = (calib[30] << 4) + (calib[29] >> 4); // 30 4bit nach links, dazu die vier höheren bits von 29
  if (cal_hudy5 > 2047) {cal_hudy5 -= 4096;} // Ablgeich signed/unsigned, Basis "12-bit"
  return;
}

void bme_readAll(byte addr, long *temp, long *press, long *hudy) {
 
  long tmp_temp = 0;
  long tmp_press = 0;
  long tmp_hudy = 0;
  long tmp_calc1 = 0;
  long tmp_calc2 = 0;
  long tmp_calc3 = 0;

  Wire.beginTransmission(addr);
  Wire.write(0xFA); // request
  Wire.endTransmission();

  Wire.requestFrom(addr, 3);
  byte data[8];
  for( int i = 0; i <= 2; i++) {
    data[i] = Wire.read();
    Serial.print(" 0x");
    Serial.print(data[i], HEX);
  }
  Serial.println();

  // die 20 Temperaturbits zusammenbasteln
  // AAAAAAAABBBBBBBBCCCC
  tmp_temp = data[2] << 12; // AAAAAAAA000000000000
  tmp_temp += data[1] << 4; // 00000000BBBBBBBB0000
  tmp_temp += data[0] >> 4; // 0000000000000000CCCC

  Wire.beginTransmission(addr);
  Wire.write(0xFC); // request
  Wire.endTransmission();

  Wire.requestFrom(addr, 3);
  //byte data[8];
  for( int i = 0; i <= 2; i++) {
    data[i] = Wire.read();
    Serial.print(" 0x");
    Serial.print(data[i], HEX);
  }
  Serial.println();

  // die 20 Luftdruck-Bits zusammenbasteln
  tmp_press = data[2] << 12; // AAAAAAAA000000000000
  tmp_press += data[1] << 4; // 00000000BBBBBBBB0000
  tmp_press += data[0] >> 4; // 0000000000000000CCCC

  Wire.beginTransmission(addr);
  Wire.write(0xF7); // request
  Wire.endTransmission();

  Wire.requestFrom(addr, 2);
//  byte data[8];
  for( int i = 0; i <= 1; i++) {
    data[i] = Wire.read();
    Serial.print(" 0x");
    Serial.print(data[i], HEX);
  }
  Serial.println();

  // die 16 Luftfeuchte-Bits zusammenbasteln
  tmp_hudy = data[1] << 8;
  tmp_hudy += data[0];

  // Temperatur-Messwert nachkalibrieren
  tmp_calc1 = ((((tmp_temp >> 3) - (cal_temp1 <<1))) * (cal_temp2)) >> 11;
	tmp_calc2 = (((((tmp_temp >> 4) - (cal_temp1)) * ((tmp_temp>>4) - (cal_temp1))) >> 12) * (cal_temp3)) >> 14;
	tmp_calc3 = tmp_calc1 + tmp_calc2;
	*temp = (tmp_calc3 * 5 + 128) >> 8;
  *temp += 0;
  Serial.print("SENSOR 0x");
  Serial.println(addr, HEX);
  Serial.print(" Temp=");
  Serial.print(tmp_temp / 100.0);
  Serial.print(" TempCal=");
  Serial.print(*temp / 100.0);
  Serial.print(" Hudy=");
  Serial.print(tmp_hudy/1024.0);
  Serial.print(" Press=");
  Serial.println(tmp_press);
  return;
}

byte bme_getStatus(byte addr) {
  byte test = 0x00;

  Wire.beginTransmission(addr);
  Wire.write(0xF3); // request
  Wire.endTransmission();

  Wire.requestFrom(addr, 1);
  byte b = 0x00;
  b = Wire.read();

  Serial.print(" Status=");
  Serial.println(b, BIN);
  return b;
}

void setup() {
  byte addr = 0x76;
  Wire.begin();

  Serial.begin(115200);
  Serial.println("BME 280 auslesen");


  bme_calibrate(0x76);
  bme_initialize(0x76);

  bme_calibrate(0x77);
  bme_initialize(0x77);
}


void loop() {
  long temp, hudy, press;
  byte addr = 0x76;
  bme_getStatus(addr);
  bme_readAll(addr, &temp, &hudy, &press);
  addr += 1;
  bme_getStatus(addr);
  bme_readAll(addr, &temp, &hudy, &press);

  delay(1000);
}
