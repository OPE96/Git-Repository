#include <SPI.h>
#include <Wire.h>
#include <Arduino_FreeRTOS.h>
#include "DHT.h"
#include <queue.h>
#define DHTTYPE DHT11
#define DHTPIN 2 
DHT dht(DHTPIN, DHTTYPE);
struct DHT11_READ{
  float humidity;
  float temperature;
};

QueueHandle_t MYQUEUE;

void setup() {
  // put your setup code here, to run once:
  MYQUEUE = xQueueCreate( 10, sizeof(struct DHT11_READ));

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

        xTaskCreate(TaskreadDH11, // Task function
                "DHT", // Task name
                256,  // Stack size
                NULL, 
                2, // Priority
                NULL);
   }
  
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
    vTaskDelay( 2500 / portTICK_PERIOD_MS );
  }
}

void TaskreadDH11(void *pvParameters)
{
  
  
  (void) pvParameters;
  Serial.begin(9600);
  
  for (;;)
  {
    
    struct DHT11_READ sensor;
    // dht.begin();
    // sensor.humidity = dht.readHumidity();
    // sensor.temperature = dht.readTemperature();

    xQueueSend (MYQUEUE, &sensor, portMAX_DELAY);
    Serial.println("DHT TASK");
    vTaskDelay( 5000 / portTICK_PERIOD_MS );   
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
    
    struct DHT11_READ sensor;
    if (xQueueReceive (MYQUEUE, &sensor, portMAX_DELAY) == pdPASS){
        Serial.print(F("Humidity: "));
        Serial.print(sensor.humidity);
        Serial.print(F("%  Temperature: "));
        Serial.print(sensor.temperature);
        Serial.print(F("°C "));
        Serial.println();
    }
    vTaskDelay( 2500 / portTICK_PERIOD_MS );   
  }
}
