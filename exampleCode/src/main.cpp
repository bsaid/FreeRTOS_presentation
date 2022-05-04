#include <Arduino.h>
#include <esp_task_wdt.h>

// Select the example you want to run
#define RUN_EXAMPLE_1

// Functions used globally in all examples
void initLEDs() {
	pinMode(L_R, OUTPUT);
	pinMode(L_Y, OUTPUT);
    pinMode(L_G, OUTPUT);
}

void blinkLedForever(int led) {
	for(;;) {  // repeat forever
		digitalWrite(led, HIGH); // led on
		digitalWrite(led, LOW);  // led off
	}
}

void blinkLedForeverWithDelay(int led, uint32_t ticks) {
    for(;;) {
        digitalWrite(led, HIGH);
        vTaskDelay(ticks);
        digitalWrite(led, LOW);
        vTaskDelay(ticks);
    }
}

void blinkLedOnce(uint8_t led, uint64_t run_us) {
	for(uint64_t t = micros(); t + run_us > micros();) {
		digitalWrite(led, HIGH);
		digitalWrite(led, LOW);
	}
}

void blinkLedForTime(uint8_t led, uint64_t run_us, uint64_t wait_ms) {
    for(;;) {
        blinkLedOnce(led, run_us);
        vTaskDelay(wait_ms);
	}
}


#ifdef RUN_EXAMPLE_1
// Slides: 7 - 15

void setup() {
    int t = 500; // time in milliseconds

    pinMode(L_R, OUTPUT);
    pinMode(L_Y, OUTPUT);
    pinMode(L_G, OUTPUT);
	pinMode(SW3, INPUT_PULLUP);

    for(;;) {
		if(!digitalRead(SW3)) t = 5;
        digitalWrite(L_R, HIGH);
        digitalWrite(L_Y, LOW);
        digitalWrite(L_G, LOW);
        vTaskDelay(2*t);
        digitalWrite(L_R, HIGH);
        digitalWrite(L_Y, HIGH);
        digitalWrite(L_G, LOW);
        vTaskDelay(t);
        digitalWrite(L_R, LOW);
        digitalWrite(L_Y, LOW);
        digitalWrite(L_G, HIGH);
        vTaskDelay(2*t);
        digitalWrite(L_R, LOW);
        digitalWrite(L_Y, HIGH);
        digitalWrite(L_G, LOW);
        vTaskDelay(t);
    }
}

#elif defined(RUN_EXAMPLE_2)
// Slides: 17 - 25

void yellowTask(void * pvParameters) {
    blinkLedForever(L_Y);
}

void setup() {
    pinMode(L_Y, OUTPUT); // Activate yellow LED

    xTaskCreate(
	    yellowTask,   // Function to implement the task
	    "yellowTask", // Name of the task
	    10000,          // Stack size in words
	    NULL,           // Task input parameter
	    1,              // Priority of the task
	    NULL           // Task handle.
	);
}

#elif defined(RUN_EXAMPLE_3)
// Slides: 26 - 30

void redTask(void * pvParameters) {
	blinkLedForeverWithDelay(L_R, 5);
}
		
void setup() {
    pinMode(L_R, OUTPUT);

	xTaskCreatePinnedToCore(
		redTask,   // Function to implement the task
		"redTask", // Name of the task
		10000,     // Stack size in words
		NULL,      // Task input parameter
		2,         // Priority of the task
		NULL       // Task handle
	);
}

#elif defined(RUN_EXAMPLE_4)
// Slides: 33 - 41

void yellowTask(void * pvParameters) {
	blinkLedForever(L_Y);
}

void setup() {
	esp_task_wdt_deinit();
	pinMode(L_Y, OUTPUT);
		
	xTaskCreatePinnedToCore(
		yellowTask,   // Function to implement the task
		"yellowTask", // Name of the task
		10000,     // Stack size in words
		NULL,      // Task input parameter
		2,         // Priority of the task
		NULL,       // Task handle.
		1  // CPU core ID
	);
}

#elif defined(RUN_EXAMPLE_5)
// Slides: 43 - 49

void redTask(void * pvParameters) {
	blinkLedForever(L_R);
}

void yellowTask(void * pvParameters) {
	blinkLedForever(L_Y);
}

void setup() {
	esp_task_wdt_deinit();
    pinMode(L_R, OUTPUT);
	pinMode(L_Y, OUTPUT);
    pinMode(L_G, OUTPUT);
		
	xTaskCreate(
		redTask,   // Function to implement the task
		"redTask", // Name of the task
		10000,     // Stack size in words
		NULL,      // Task input parameter
		1,         // Priority of the task
		NULL       // Task handle.
	);

    xTaskCreate(
		yellowTask,   // Function to implement the task
		"yellowTask", // Name of the task
		10000,     // Stack size in words
		NULL,      // Task input parameter
		1,         // Priority of the task
		NULL       // Task handle.
	);

    blinkLedForever(L_G);
}

#elif defined(RUN_EXAMPLE_6)
// Slides: 51 - 60

void setup() {
    pinMode(L_G, OUTPUT);
    blinkLedForTime(L_G, 1500, 1);
}

#elif defined(RUN_EXAMPLE_7)
// Slides: 61 - 62

void redTask(void * pvParameters) {
	blinkLedForTime(L_R, 300, 1);
}

void yellowTask(void * pvParameters) {
	blinkLedForTime(L_Y, 300, 1);
}

void setup() {
    pinMode(L_R, OUTPUT);
	pinMode(L_Y, OUTPUT);
    pinMode(L_G, OUTPUT);
		
	xTaskCreatePinnedToCore(
		redTask,   // Function to implement the task
		"redTask", // Name of the task
		10000,     // Stack size in words
		NULL,      // Task input parameter
		3,         // Priority of the task
		NULL,       // Task handle.
		0
	);

    xTaskCreatePinnedToCore(
		yellowTask,   // Function to implement the task
		"yellowTask", // Name of the task
		10000,     // Stack size in words
		NULL,      // Task input parameter
		2,         // Priority of the task
		NULL,       // Task handle.
		0
	);

    blinkLedForTime(L_G, 300, 1);
}

#elif defined(RUN_EXAMPLE_8)
// Slides: 65 - 67

void vTimerCallback(TimerHandle_t xTimer) {
	blinkLedOnce(L_Y, 1600);
}

void setup() {
	pinMode(L_Y, OUTPUT);
	TimerHandle_t xTimer = xTimerCreate("Timer1", 2, pdTRUE, ( void * ) 0, vTimerCallback);
	xTimerStart(xTimer, 0);
}

#elif defined(RUN_EXAMPLE_9)
// Slides: 69 - 76

void blinkLedForTimeCritical(uint8_t led, uint64_t run_us,
                             uint64_t wait_ms, portMUX_TYPE *myMutex) {
    for(;;) {
        portENTER_CRITICAL(myMutex);
        blinkLedOnce(led, run_us);
        portEXIT_CRITICAL(myMutex);
        vTaskDelay(wait_ms);
	}
}

portMUX_TYPE myMutex = portMUX_INITIALIZER_UNLOCKED;

void redTask(void * pvParameters) {
	blinkLedForTimeCritical(L_R, 300, 1, &myMutex);
}

void yellowTask(void * pvParameters) {
	blinkLedForTimeCritical(L_Y, 300, 1, &myMutex);
}

void setup() {
    initLEDs();

	xTaskCreate(redTask, "redTask", 10000, NULL, 3, NULL);
    xTaskCreate(yellowTask, "yellowTask", 10000, NULL, 2, NULL);
    blinkLedForTimeCritical(L_G, 300, 1, &myMutex);
}

#elif defined(RUN_EXAMPLE_10)
// Slides: 78 - 83

QueueHandle_t myQueue = xQueueCreate(10, sizeof(uint32_t));

void redTask(void * pvParameters) {
	for(;;) {
		uint32_t data = 0;
		if(xQueueReceive(myQueue, &data, (TickType_t) 0) == pdPASS) {
			Serial.println("Received in red thread.");
			blinkLedOnce(L_R, 400);
		}
		vTaskDelay(3);
	}
}

void yellowTask(void * pvParameters) {
	for(;;) {
		uint32_t data = 0;
		if(xQueueReceive(myQueue, &data, (TickType_t) 0) == pdPASS) {
			Serial.println("Received in yellow thread.");
			blinkLedOnce(L_Y, 400);
		}
		vTaskDelay(4);
	}
}

void setup() {
    initLEDs();
	Serial.begin(115200);

	xTaskCreate(redTask, "redTask", 10000, NULL, 3, NULL);
    xTaskCreate(yellowTask, "yellowTask", 10000, NULL, 2, NULL);

	for(;;) {
		blinkLedOnce(L_G, 400);
		uint32_t data = 42;
		xQueueSend(myQueue, (void*) &data, (TickType_t) 0);
		Serial.println("Sent from green thread.");
		vTaskDelay(2);
	}
}

#else

vois setup() {
	Serial.begin(115200);
	for(;;) {
		Serial.println("No example selected.");
		delay(1000);
	}
}

#endif

void loop() {}
