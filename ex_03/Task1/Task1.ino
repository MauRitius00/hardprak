// Task 1: Environmental Light Categorization
// Complete the implementation according to the task description.

// #include <Adafruit_TinyUSB.h>

// TODO: define necessary constants (e.g., calibration range, thresholds, timing)
#define analogPin A0

const int BaudRate = 115200;
const int analogResolution = 12;
const int LightMin = 50;
const int LightMax = 3500; // Sensor can only read to ~2500 (350 lux)
const int SerialDelay = 500; // ms

unsigned long last_blink_ms = 0;  // timestamp of last Execution

// TODO: implement a function that categorizes the normalized value
const char* categorize(int normalized) {
    // TODO: return "LOW", "MEDIUM", or "HIGH"
    switch(normalized) {
        case 0 ... 29:
            return "LOW";
            break;
        case 30 ...69:
            return "MEDIUM";
            break;
        case 70 ... 100:
            return "HIGH";
            break;
        default:
            return "[ERROR] Value out of Range";
    }
}

void setup() {
    // TODO: initialize Serial communication
    Serial.begin(BaudRate);
    while (!Serial);

    // TODO: configure ADC resolution to 12-bit
    analogReadResolution(analogResolution);
}

void loop() {
    // TODO: implement non-blocking timing using millis()
    // The system should sample every 500 ms


    if (millis() - last_blink_ms >= SerialDelay) {
        // TODO: read raw value from light sensor
        int analogValue = analogRead(analogPin);

        // TODO: clamp the raw value to a calibrated range
        int sensVal = constrain(analogValue, LightMin, LightMax);

        // TODO: normalize the value to a 0–100 scale
        int normalizedVal = map(sensVal, LightMin, LightMax, 0, 100);
        

        // TODO: determine the category using the categorize() function
        const char* category = categorize(normalizedVal);

        // TODO: print raw value, normalized value, and category to Serial
        Serial.print("raw value: ");
        Serial.print(analogValue);
        Serial.print(" - normalized value: ");
        Serial.print(normalizedVal);
        Serial.print(" - category: ");
        Serial.println(category);

        
        last_blink_ms = millis();
  }
}