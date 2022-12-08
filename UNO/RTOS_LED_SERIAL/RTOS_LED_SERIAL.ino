#include <SPI.h>
#include <Wire.h>
#include <Arduino_FreeRTOS.h>
#include "DHT.h"
#include <queue.h>
#define DHTTYPE DHT11
#define DHTPIN 2 
DHT dht(DHTPIN, DHTTYPE);
const int BUTTON = 4;
const int LED = 3;
int BUTTONstate = 0;
// Define a struct
struct pinRead {
  int pin;
  int value;
};

QueueHandle_t MYQUEUE;

void setup() {
  // put your setup code here, to run once:
  MYQUEUE = xQueueCreate( 10, sizeof(struct pinRead));

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

        xTaskCreate(TaskLED, // Task function
                "LED ", // Task name
                256,  // Stack size
                NULL, 
                2, // Priority
                NULL);
   }
       xTaskCreate(TaskDigitalReadPin3, // Task function
                "AnalogReadPin0", // Task name
                128,  // Stack size
                NULL, 
                2, // Priority
                NULL);
  
}

void loop() {
  // put your main code here, to run repeatedly:

}
void TaskBlink(void *pvParameters)
{
  (void) pvParameters;
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
  // Serial.println("LED TASK");
  static bool led_toggle = false;
  for (;;)
  {
    led_toggle = !led_toggle;
    digitalWrite(LED_BUILTIN, led_toggle);
    vTaskDelay( 500 / portTICK_PERIOD_MS ); //Block for 500ms  TOGGLE EVERY 500MS
  }
}

void TaskDigitalReadPin3(void *pvParameters)
{
  (void) pvParameters;
  
  for (;;)
  {
    struct pinRead currentPinRead;
    currentPinRead.pin = LED;
    currentPinRead.value = digitalRead(LED);

    /**
     * Post an item on a queue.
     * https://www.freertos.org/a00117.html
     */
    xQueueSend(MYQUEUE, &currentPinRead, portMAX_DELAY);

    // One tick delay (15ms) in between reads for stability
    vTaskDelay(1);
  }
}

void TaskLED(void *pvParameters)
{
  (void) pvParameters;
  Serial.begin(9600);
  pinMode(BUTTON, INPUT);
  pinMode(LED, OUTPUT);
  
  for (;;)
  {
    BUTTONstate = digitalRead(BUTTON);
    vTaskDelay( 500 / portTICK_PERIOD_MS );
    if (BUTTONstate == HIGH){
    digitalWrite(LED, HIGH);
  } 
  else{
    digitalWrite(LED, LOW);
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
  
  for (;;) 
  {
    
    struct pinRead currentPinRead;
    if (xQueueReceive (MYQUEUE, &currentPinRead, portMAX_DELAY) == pdPASS){
      Serial.print("Pin: ");
      Serial.print(currentPinRead.pin);
      Serial.print(" Value: ");
      Serial.println(currentPinRead.value);
    }
    vTaskDelay( 500 / portTICK_PERIOD_MS );   
  }
}

