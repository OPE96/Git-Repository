#include <SPI.h>
#include <Wire.h>
#include <Arduino_FreeRTOS.h>
#include <queue.h>
#define Tmp102_adrress 0x48 // tmp102 address see datasheet

struct TMP102_SENSOR{
  int16_t temperature;
};
uint8_t temp[2];
int16_t value;


QueueHandle_t MYQUEUE;

void setup() {
  // put your setup code here, to run once:
  MYQUEUE = xQueueCreate( 2, sizeof(struct TMP102_SENSOR));

  if  (MYQUEUE != NULL){
    xTaskCreate(TaskSerial, // Task function
                "Serial", // A name just for humans
                128,  // This stack size can be checked & adjusted by reading the Stack Highwater
                NULL, 
                2, // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
                NULL);
              
        xTaskCreate(TaskBlink, // Task function
              "Blink", // Task name
              128, // Stack size 
              NULL, 
              0, // Priority
              NULL );

        xTaskCreate(TaskreadTMP102, // Task function
                "TMP102", // Task name
                256,  // Stack size
                NULL, 
                2, // Priority
                NULL);
  
}
}

void loop() {
  // put your main code here, to run repeatedly:

}
void TaskBlink(void *pvParameters)/* led loacted on the uno board*/
{
  (void) pvParameters;
  pinMode(LED_BUILTIN, OUTPUT);
  static bool led_toggle = false;
  for (;;)
  {
    led_toggle = !led_toggle;
    digitalWrite(LED_BUILTIN, led_toggle);
    vTaskDelay( 500 / portTICK_PERIOD_MS );
  }
}

void TaskreadTMP102(void *pvParameters)
{
  struct TMP102_SENSOR TEMP_sensor;
  Wire.begin();
  (void) pvParameters;
  
  for (;;)
  {
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
  int16_t temporary_value = ((temp[0] << 4) | temp[1]);

  //convert to celcius (0.0625C resolution)

  value = temporary_value * 0.0625;
  TEMP_sensor.temperature = value ;
  xQueueSend (MYQUEUE, &TEMP_sensor, portMAX_DELAY);
  vTaskDelay( 5000 / portTICK_PERIOD_MS );
  }
}
}

void TaskSerial(void * pvParameters) {

  (void) pvParameters;
  // Init Arduino serial
  Serial.begin(9600);

  // Wait for serial port to connect. Needed for native USB, on LEONARDO, MICRO, YUN, and other 32u4 based boards.
  while (!Serial) {
    vTaskDelay(1);
  }
  struct TMP102_SENSOR TEMP_sensor;
  for (;;) 
  {
    if (xQueueReceive (MYQUEUE, &TEMP_sensor, portMAX_DELAY) == pdPASS){
        Serial.print(F("Temperature: "));
        Serial.print(TEMP_sensor.temperature);
        Serial.print(F("°C "));
        Serial.println();
    }
    vTaskDelay( 500 / portTICK_PERIOD_MS );   
  }
}
