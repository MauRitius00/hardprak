// Task 3: Air Quality Monitoring with SGP30


#include <Adafruit_SGP30.h>

// --- Objects ---
Adafruit_SGP30 sgp;

// --- Timing ---
// TODO: define sampling interval (1 second)
unsigned long lastSample = 0;
const int SerialDelay = 1000; // 2000ms = 2sec

// --- Warm-up ---
// TODO: define warm-up duration (15 seconds)
#define warmupduration 15000
unsigned long startwarmup = 0.0;
// TODO: maintain a warm-up state indicator
bool warmupstate = true;

void setup() {
    Serial.begin(115200);
    while (!Serial && millis() < 3000);

    // TODO: initialize the sensor and handle initialization failure
    sgp.begin();

    // TODO: start air quality measurements
    if (!sgp.IAQinit()) {
        Serial.println("[ERROR] IAQ Sensor not initialized ");
    }
    sgp.softReset();

    // TODO: record system start time
    startwarmup = millis();

}

void loop() {
    unsigned long now = millis();
    if (warmupstate && (now - startwarmup) > warmupduration) {
        warmupstate = false;
    }
    // TODO: implement periodic sampling at 1 Hz using millis()
    if (now - lastSample >= SerialDelay) {
        if (!sgp.IAQmeasure()) {
            Serial.println("[WARN] Measurment Failure");
        }

        else {

            // TODO: perform a measurement and handle failure cases


            // TODO: update warm-up state based on elapsed time
            // Outside the if statement

            // TODO: output measurement results
            //       - indicate warm-up vs ready state
            //       - ensure correct formatting for integer values
            if (warmupstate) {
                Serial.print("[WARN] UNSTABLE/WARMUP");
            }
            Serial.print(" | eCO2: ");
            Serial.print(sgp.eCO2);
            Serial.print(" | TVOC: ");
            Serial.print(sgp.TVOC);
            Serial.println(" |");
        }
        lastSample = millis();
    }
}
