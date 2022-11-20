#include <Arduino_FreeRTOS.h>
#include <queue.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
#include "DHT.h"
#define DHTTYPE DHT11
#define DHTPIN 2 
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
// Initialize DHT sensor.
DHT dht(DHTPIN, DHTTYPE);
#define BUTTON 4
#define LED 3
int BUTTONstate = 0;
struct dht_output
{
  float temp;
  float humidity;
};dht_output s1;
QueueHandle_t H_Queue, T_Queue;


void setup() {
  Serial.begin(9600);
  dht.begin();
  // put your setup code here, to run once:
  display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);// Address 0x3D for 128x64

  //delay(2000);
  display.clearDisplay();
  display.setTextColor(WHITE);
  H_Queue = xQueueCreate(5, sizeof(float) );
  T_Queue = xQueueCreate(5, sizeof(float) );
  if (H_Queue != NULL)
  {
    // Serial.println("In Setup function");
     xTaskCreate(Dht11, "Task 1", 240, NULL, 1, NULL);
     vTaskStartScheduler();
  }
  else{
    Serial.print("The Queue could not be created");
  }
 
  
 //xTaskCreate(Task1, "Task 1", 240, NULL, 1, NULL);

/*   if (taskRet == pdPASS)
    Serial.println(F("Task Successful"));
  else
    Serial.println(F("Failed")); */
              
/*   xTaskCreate(
    Task2_SERIAL
    , "Task 2"
    , 128
    , (void *) 1
    , 0
    , NULL);
  Serial.println(F("Starting scheduler"));  
  vTaskStartScheduler();
} */

}
void loop() {
 
}

static void Dht11(void * pvParameters){
  //(void) pvParameters;

  while(1)
  {
      Serial.println("Task 1 - SENSOR READING");
      s1.humidity = dht.readHumidity();
      s1.temp  = dht.readTemperature(); 
      xQueueSend(H_Queue, &s1.humidity, portMAX_DELAY == pdPASS);
  
      delay(2000);
      display.clearDisplay();
    // display temperature
      display.setTextSize(1);
      display.setCursor(0, 0);
      display.print("Temperature: ");  
      display.setTextSize(2);
      display.setCursor(0, 10);
      display.print(s1.temp);
      display.print(" ");
      display.setTextSize(1);
      display.cp437(true);//To display the º symbol, we use
      display.write(167);//Then, use the write() method to display your chosen character. The º 
      display.setTextSize(2);
      display.print("C"); 
    // display humidity
      display.setTextSize(1);
      display.setCursor(0, 35);
      display.print("Humidity: ");
      display.setTextSize(2);
      display.setCursor(0, 45);
      display.print(s1.humidity);
      display.print(" %"); 
  
      display.display();//call display.display()


     vTaskDelay(2000 / portTICK_PERIOD_MS);
  }
 }
/* 
void Task2_SERIAL(void* pvParameters){
  (void) pvParameters;
  while(1)
  {
    xSemaphoreTake(serial_mutex, portMAX_DELAY);
    Serial.print(F("Humidity: "));
    Serial.print( s1.humidity);
    Serial.print(F("%  Temperature: "));
    Serial.print(s1.temp);
    Serial.print(F("°C "));
    Serial.println();
    xSemaphoreGive(serial_mutex);
    vTaskDelay(2000 / portTICK_PERIOD_MS);
 }
} */
