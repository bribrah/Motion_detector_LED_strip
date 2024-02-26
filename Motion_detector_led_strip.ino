#include <Arduino_FreeRTOS.h>
#include <FastLED.h>
#include <semphr.h>

#define LED_PIN 7
#define NUM_LEDS 20
#define SENSOR_PIN 8
#define SENSOR_PIN_2 9
#define POTENTIOMETER_PIN A0
#define DEFAULT_STACK_SIZE 256
#define LIGHTS_ON_SECONDS 15

#define MAX_BRIGHTNESS_PERCENTAGE .75
#define FRAME_RATE 10
CRGB leds[NUM_LEDS];

TaskHandle_t LED_Task_handle;
TaskHandle_t Serial_Task_handle;
TaskHandle_t Motion_Detection_Task_handle;

int gHue = 0;
SemaphoreHandle_t motionDetectedSemaphore;

String currentColor = "blue";
String gSerialBuffer = "";

void setup()
{
    Serial.begin(115200);
    FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);

    pinMode(POTENTIOMETER_PIN, INPUT);
    pinMode(SENSOR_PIN, INPUT);

    changeAllLeds(CRGB::Black);
    xTaskCreate(LED_loop, "LED Task", DEFAULT_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, &LED_Task_handle);
    xTaskCreate(Serial_loop, "Serial Task", DEFAULT_STACK_SIZE, NULL, tskIDLE_PRIORITY, &Serial_Task_handle);
    vTaskStartScheduler();
}

void loop()
{
    // never gets ran
}

void changeAllLeds(CRGB::HTMLColorCode color)
{
    for (int i = 0; i < NUM_LEDS; i++)
    {
        leds[i] = color;
    }
    FastLED.show();
}

void LED_loop(void *pvParameters)
{
    bool motionDetected = false;
    int dial, brightness;
    unsigned long lastMotionDetected;
    while (1)
    {
        dial = 1000 - analogRead(POTENTIOMETER_PIN);
        brightness = max((MAX_BRIGHTNESS_PERCENTAGE * 255.) * ((double)dial / 1000.0), 0);

        FastLED.setBrightness(brightness);
        currentColor.toLowerCase();

        if (!motionDetected)
        {
            changeAllLeds(CRGB::Black);
            motionDetected = digitalRead(SENSOR_PIN) || digitalRead(SENSOR_PIN_2);
            if (motionDetected)
            {
                lastMotionDetected = millis();
            }
        }
        else if (currentColor == "white")
        {
            changeAllLeds(CRGB::White);
        }
        else if (currentColor == "blue")
        {
            changeAllLeds(CRGB::Blue);
        }
        else if (currentColor == "red")
        {
            changeAllLeds(CRGB::Red);
        }
        else if (currentColor == "green")
        {
            changeAllLeds(CRGB::Green);
        }
        else if (currentColor == "rainbow")
        {
            fill_rainbow(leds, NUM_LEDS, 0);
            FastLED.show();
        }
        else
        {
            changeAllLeds(CRGB::Black);
        }

        if (motionDetected && millis() - lastMotionDetected >= LIGHTS_ON_SECONDS * 1000)
        {
            motionDetected = digitalRead(SENSOR_PIN) || digitalRead(SENSOR_PIN_2);
        }
        vTaskDelay(pdMS_TO_TICKS(1000 / FRAME_RATE));
    }
}

void Serial_loop(void *pvParameters)
{
    // Serial.setTimeout(100);
    Serial.write("Enter LED Color: ");
    while (1)
    {
        // Serial.println(analogRead(POTENTIOMETER_PIN));
        if (Serial.available() > 0)
        {
            while (Serial.available() > 0)
            {
                char read;
                Serial.readBytes(&read, 1);
                if (read == '\n')
                {
                    currentColor = gSerialBuffer;
                    gSerialBuffer = "";
                }
                else
                {
                    gSerialBuffer += read;
                }
            }
        }
        vTaskDelay(pdMS_TO_TICKS(200));
    }
}
