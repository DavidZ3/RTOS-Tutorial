#if CONFIG_FREERTOS_UNICORE
  static const BaseType_t app_cpu = 0;
#else
  static const BaseType_t app_cpu = 1;
#endif

#define LED_BUILTIN 2
#define STR_BUFFER_SIZE 25

static int ledBlinkDelayMs = 500;
static bool ledStatus = false;
static char strBuffer[STR_BUFFER_SIZE];

// Tasks
void setBlinkDelayTask(void *parameter){
  for(;;){
    int availableBytes = Serial.available();
    for(int i = 0; ((i < STR_BUFFER_SIZE) && (i < availableBytes)); i++){
        char c = Serial.read();
        if(c == '\n'){
          break;
        }
        strBuffer[i] = c;
        strBuffer[i+1] = '\0'; // Append a null
    }

    if(strlen(strBuffer) && (strspn(strBuffer, "0123456789") == strlen(strBuffer))){
      ledBlinkDelayMs = atoi(strBuffer);
      Serial.print("Acknowledged an input blink delay of ");
      Serial.print(ledBlinkDelayMs);
      Serial.println(" ms.");
      strBuffer[0] = '\0';
    }
    vTaskDelay(1000/portTICK_RATE_MS);
  }
}

void blinkTask(void *parameter){
  for(;;){
    ledStatus = !ledStatus;
    digitalWrite(LED_BUILTIN, ledStatus);
    vTaskDelay(ledBlinkDelayMs/portTICK_RATE_MS);
  }
}

// Task handles
static TaskHandle_t blinkTaskHandle = NULL;
static TaskHandle_t setBlinkDelayTaskHandle = NULL;


void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
    // Configure Serial (go slow so we can watch the preemption)
  Serial.begin(300);

  // Wait a moment to start so we dont miss serial output.
  vTaskDelay(1000/portTICK_PERIOD_MS);
  Serial.println();
  Serial.println("--- FreeRTOS TASK DEMO ---");

  // Print self priority
  Serial.print("Setup and loop task running on core ");
  Serial.print(xPortGetCoreID());
  Serial.print(" with priority ");
  Serial.println(uxTaskPriorityGet(NULL));

  xTaskCreatePinnedToCore(
    blinkTask,              // Task Func Pointer
    "blinkTask",            // Task Name
    1024,                   // Stack Size
    NULL,                   // Parameter to Pass
    2,                      // Task Priority
    &blinkTaskHandle,       // Task handle
    app_cpu                 // CPU To Run IN
  );

  xTaskCreatePinnedToCore(
    setBlinkDelayTask,   // Task Func Pointer
    "setBlinkDelayTask",     // Task Name
    1024,         // Stack Size
    NULL,         // Parameter to Pass
    3,            // Task Priority
    &setBlinkDelayTaskHandle,      // Task handle
    app_cpu       // CPU To Run IN
  );
  
  vTaskDelete(NULL);
}

void loop() {
  // put your main code here, to run repeatedly:
  // Suspend the higher priority task for some intervals
  for(;;){
    vTaskSuspend(setBlinkDelayTaskHandle);
    if(Serial.available() > 0){
      vTaskResume(setBlinkDelayTaskHandle);
    }
    vTaskDelay(1000/portTICK_PERIOD_MS);
  }
}
