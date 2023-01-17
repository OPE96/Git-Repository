#include <Wire.h>
void setup() {
  // put your setup code here, to run once:
  Wire.begin();

  Serial.begin(9600);
  while(!Serial); // waiting till serial monitor is available
  Serial.print("\nI2C Scanner");



}

void loop() {
  // put your main code here, to run repeatedly:
  int sensors = 0;
  Serial.println ("......Scanning");

  for (byte address = 1; address < 127; ++address){
    Wire.beginTransmission(address); 
    byte error = Wire.endTransmission(); // // if a device is acknowledge with an address

    if (error == 0){ // if the error is zero a device has sent an acknowledge back to arduino 
      Serial.print("I2C device found at address 0x");
    if (address < 16) {
        Serial.print("0");
      }
      Serial.print(address, HEX);
      Serial.println("  !");

      ++sensors;
    } else if (error == 4) {
      Serial.print("Unknown error at address 0x");
      if (address < 16) {
        Serial.print("0");
      }
      Serial.println(address, HEX); //address in Hex
    }
  }
  if (sensors == 0) {
    Serial.println("No I2C devices found\n");
  } else {
    Serial.println("done\n");
  }
  delay(5000); // Wait 5 seconds for next scan
 }


