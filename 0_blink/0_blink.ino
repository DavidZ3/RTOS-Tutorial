// Use only 1 core for demo purposes
#if CONFIG_FREERTOS_UNICORE
static const BaseType_t app_cpu = 0;
#else
static const BaseType_t app_cpu = 1;
#endif

#define LED_BUILTIN 2


// Pins
static const int led_pin = LED_BUILTIN;
static bool led_status = false;

// Our task: blink an LED
void toggleLed(void *parameter) {
  while(1){
    led_status = !led_status;
    digitalWrite(led_pin, led_status);
    vTaskDelay(500/portTICK_PERIOD_MS);
  }
}

void toggleLedSlow(void *parameter){
  while(1){
    led_status = !led_status;
    digitalWrite(led_pin, led_status);
    vTaskDelay(1200/portTICK_PERIOD_MS);
  }
}

void setup() {
  // put your setup code here, to run once:
  pinMode(led_pin, OUTPUT);

  xTaskCreatePinnedToCore(
    toggleLed,    // Task Func Pointer
    "Toggle LED", // Task Name
    1024,         // Stack Size
    NULL,         // Parameter to Pass
    0,            // Task Priority
    NULL,         // Task handle
    app_cpu       // CPU To Run IN
  );

    xTaskCreatePinnedToCore(
    toggleLedSlow,    // Task Func Pointer
    "Toggle LED Slow", // Task Name
    1024,         // Stack Size
    NULL,         // Parameter to Pass
    0,            // Task Priority
    NULL,         // Task handle
    app_cpu       // CPU To Run IN
  );

  // If this was vanilla FreeRTOS we would also call vTaskStartScheduler() 
  // but this is called by default in main after setting up our tasks.
}

void loop() {
  // put your main code here, to run repeatedly:

}
