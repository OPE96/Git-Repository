#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
#define Tmp102_adrress 0x48 // tmp102 address see datasheet
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

//Led Button setup
const int BUTTON = 4;
const int LED = 3;
int BUTTONstate = 0;
void setup() {
  // put your setup code here, to run once:
  Wire.begin();
  //Led intialisation
  pinMode(BUTTON, INPUT);
  pinMode(LED, OUTPUT);
  Serial.begin(9600);
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) { // Address 0x3D for 128x64
    
  Serial.println(F("SSD1306 allocation failed")); //This snippet also prints a message on the Serial Monitor, in case we’re not able to connect to the display.
  for(;;);// Don't proceed, loop forever
  }
  display.clearDisplay();
  display.setTextColor(WHITE);

}

void loop() {
  // put your main code here, to run repeatedly:
  buttonled();
  print_temp();
  Display();
  delay(500); //1/2 seconds
}

double TMP102(void){
  uint8_t temp[2]; // temp holds 2 bytes of data 
  int16_t value; // for storing the 2 bytes combination
  Wire.beginTransmission(Tmp102_adrress); // wake the sensor point to 0x48
  Wire.write(0x00); //register address  reset register pointer address to defualt so its ready to read temp
  Wire.endTransmission();
  Wire.requestFrom(Tmp102_adrress, 2); // request 2 bytes from senor temp data
  Wire.endTransmission();
  delay(10);
  if ( Wire.available()<=2){ /* if  2byte is available*/
  //read data out
  temp[0] = Wire.read();
  temp[1] = Wire.read();

  //ignore the lower 4 bits of byte 2
  temp[1] = temp [1] >> 4 ; // use left shit
  //combine to make 12 bits binary number
  value = ((temp[0] << 4) | temp[1]);

  //convert to celcius (0.0625C resolution)
  //value = value * 0.0625;
  //Serial.println(value);
  return value * 0.0625;
  }
}
void Display(){
  //Serial.println("display function");
  float temp = TMP102();
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("Temperature: "); 
  display.setTextSize(2);
  display.setCursor(0, 10);
  display.print(temp);
  display.print(" ");
  display.setTextSize(1);
  display.cp437(true);//To display the º symbol, we use
  display.write(167);//Then, use the write() method to display your chosen character. The º 
  display.setTextSize(2);
  display.print("C");
  display.display();
}
void print_temp(){
  float temp_in_celsius = TMP102();
  Serial.print("TEMP : ");
  Serial.println(temp_in_celsius);
}
void buttonled(){
  BUTTONstate = digitalRead(BUTTON);
  if (BUTTONstate == HIGH)
  {
    digitalWrite(LED, HIGH);
  } 
  else{
    digitalWrite(LED, LOW);
  }
}
