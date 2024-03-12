#include <FastLED.h>

#define LED_PIN 12
#define NUM_LEDS 120
#define SENSOR_PIN 3

#define SENSOR_PIN_2 4
#define POTENTIOMETER_PIN A7
#define DEFAULT_STACK_SIZE 256

#define DEBOUNCE_ON_MS 2000
#define DEBOUNCE_OFF_MS 10000

#define MAX_BRIGHTNESS_PERCENTAGE .50
#define FRAME_RATE 15
CRGB leds[NUM_LEDS];

// TaskHandle_t LED_Task_handle;
// TaskHandle_t Serial_Task_handle;
// TaskHandle_t Motion_Detection_Task_handle;

int gHue = 0;

String currentColor = "blue";
String gSerialBuffer = "";

void setup() {
  Serial.begin(115200);
  FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);

  pinMode(POTENTIOMETER_PIN, INPUT);
  pinMode(SENSOR_PIN, INPUT);

  changeAllLeds(CRGB::Black);
  // xTaskCreate(LED_loop, "LED Task", DEFAULT_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, &LED_Task_handle);
  // xTaskCreate(Serial_loop, "Serial Task", DEFAULT_STACK_SIZE, NULL, tskIDLE_PRIORITY, &Serial_Task_handle);
  // vTaskStartScheduler();
}

void loop() {
  LED_loop(NULL);
}

void changeAllLeds(CRGB::HTMLColorCode color) {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = color;
  }
  FastLED.show();
}

void LED_loop(void *pvParameters) {
  bool motionDetected = false;
  bool motionDetectedTemp = false;
  bool debounceStartStarted = false;
  int dial, brightness;
  unsigned long lastMotionDetected, debounceOnStartMs;
  while (1) {
    dial = 1000 - analogRead(POTENTIOMETER_PIN);
    brightness = max((MAX_BRIGHTNESS_PERCENTAGE * 255.) * ((double)dial / 1000.0), 5);

    FastLED.setBrightness(brightness);
    currentColor.toLowerCase();

    if (!motionDetected) {
      changeAllLeds(CRGB::Black);
      motionDetectedTemp = digitalRead(SENSOR_PIN) || digitalRead(SENSOR_PIN_2);
      if (motionDetectedTemp && debounceStartStarted && (millis() - debounceOnStartMs) > DEBOUNCE_ON_MS) {
        lastMotionDetected = millis();
        motionDetected = true;
      } else if (motionDetectedTemp && !debounceStartStarted) {
        debounceStartStarted = true;
        debounceOnStartMs = millis();
      } else if (!motionDetectedTemp) {
        debounceStartStarted = false;
      }
    }

    else if (currentColor == "white") {
      changeAllLeds(CRGB::White);
    } else if (currentColor == "blue") {
      changeAllLeds(CRGB::Blue);
    } else if (currentColor == "red") {
      changeAllLeds(CRGB::Red);
    } else if (currentColor == "green") {
      changeAllLeds(CRGB::Green);
    } else if (currentColor == "rainbow") {
      fill_rainbow(leds, NUM_LEDS, 0);
      FastLED.show();
    } else {
      changeAllLeds(CRGB::Black);
    }

    if (motionDetected) {
      motionDetectedTemp = digitalRead(SENSOR_PIN) || digitalRead(SENSOR_PIN_2);

      // No motion has been detected and the debounce time has passed
      if (!motionDetectedTemp && (millis() - lastMotionDetected) >= DEBOUNCE_OFF_MS) {
        motionDetected = false;  // Set motionDetected to false as no motion is detected within the debounce period
      } else if (motionDetectedTemp) {
        lastMotionDetected = millis();  // Reset last motion detected time as motion is detected
      }
    }
  }
}

  // void Serial_loop(void *pvParameters)
  // {
  //     // Serial.setTimeout(100);
  //     Serial.write("Enter LED Color: ");
  //     while (1)
  //     {
  //         // Serial.println(analogRead(POTENTIOMETER_PIN));
  //         if (Serial.available() > 0)
  //         {
  //             while (Serial.available() > 0)
  //             {
  //                 char read;
  //                 Serial.readBytes(&read, 1);
  //                 if (read == '\n')
  //                 {
  //                     currentColor = gSerialBuffer;
  //                     gSerialBuffer = "";
  //                 }
  //                 else
  //                 {
  //                     gSerialBuffer += read;
  //                 }
  //             }
  //         }
  //         vTaskDelay(pdMS_TO_TICKS(200));
  //     }
  // }