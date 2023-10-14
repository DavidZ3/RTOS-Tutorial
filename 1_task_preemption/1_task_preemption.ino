#if CONFIG_FREERTOS_UNICORE
  static const BaseType_t app_cpu = 0;
#else
  static const BaseType_t app_cpu = 1;
#endif

// Some String To Print
const char msg[] = "Barkadeer brig Arr booty rum.";

// Task handles
static TaskHandle_t task_1 = NULL;
static TaskHandle_t task_2 = NULL;

// Tasks

// Task: print to Serial Terminal with lower priority
void startTask1(void *parameter){
  // Count number of characters in string
  int msg_len = strlen(msg);

  // Print string to Terminal
  for(;;){
    Serial.println();
    for(int i = 0; i < msg_len; i++){
      Serial.print(msg[i]);
    }
    Serial.println();
    vTaskDelay(1000/portTICK_PERIOD_MS);
  }
}

// Task: print to Serial Terminal with higher priority
void startTask2(void *parameter){
  for(;;){
    Serial.print("*");
    vTaskDelay(100/portTICK_PERIOD_MS);
  }
}

void setup() {
  // Configure Serial (go slow so we can watch the preemption)
  Serial.begin(300);

  // Wait a moment to start so we dont miss serial output.
  vTaskDelay(1000/portTICK_PERIOD_MS);
  Serial.println();
  Serial.println("--- FreeRTOS TASK DEMO ---");

  // Print self priority
  Serial.print("Setup and loop task running on core.");
  Serial.print(xPortGetCoreID());
  Serial.print(" with priority ");
  Serial.println(uxTaskPriorityGet(NULL));

  xTaskCreatePinnedToCore(
    startTask1,   // Task Func Pointer
    "Task 1",     // Task Name
    1024,         // Stack Size
    NULL,         // Parameter to Pass
    1,            // Task Priority
    &task_1,      // Task handle
    app_cpu       // CPU To Run IN
  );

  xTaskCreatePinnedToCore(
    startTask2,   // Task Func Pointer
    "Task 2",     // Task Name
    1024,         // Stack Size
    NULL,         // Parameter to Pass
    2,            // Task Priority
    &task_2,      // Task handle
    app_cpu       // CPU To Run IN
  );
}

void loop() {
  // Suspend the higher priority task for some intervals
  for(int i = 0; i < 3; i++){
    vTaskSuspend(task_2);
    vTaskDelay(2000/portTICK_PERIOD_MS);
    vTaskResume(task_2);
    vTaskDelay(2000/portTICK_PERIOD_MS);
  }

  // Delete the lower priority task
  if (task_1 != NULL){
    vTaskDelete(task_1);
    task_1 = NULL;
  }
}























