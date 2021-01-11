# Mapping of I²C-addresses

 0x40 = HDC 1080

 0x76 = BME 280 "LOW"
 
 0x77 = BME 280 "HIGH"

# usage

The i2c_read_*-Folders were used as a starting point to get into the communication-protocols of the given sensors. Code from there derivates from the standard-libraries in the Arduino Library-Manager.
Code in the finalskript-Folder is the final code for "production"-Usage.
Known issues: Code does not check if the raw measurements from the sensors are realistic yet. To implement that it could be useful to restructure the code into reusable functions, for now the code is more or less linearly organized.
