// Task 2: Robust Temperature and Humidity Monitoring

#include <Adafruit_TinyUSB.h>
#include <DHT.h>
#include <math.h>

// --- Configuration ---
// TODO: define sensor pin and type (DHT11)
#define DHTPIN D7
#define DHTTYPE DHT11

// --- Objects ---
// TODO: create DHT sensor instance


DHT dht(DHTPIN, DHTTYPE);

// --- Timing ---
unsigned long lastSample = 0;

// TODO: define sampling interval (2 seconds)
const int SerialDelay = 2000; // 2000ms = 2sec


// --- State Variables ---
// TODO: store last valid temperature and humidity

float temp = 0.0;
float humid = 0.0;

// TODO: maintain a failure counter
int fcounter = 0;

// constants
const float a = 17.62;
const float b = 243.12;

const int failurwarningthreshhold = 5;

// helper variables
float gamm = 0.0f;
float dewPoint = 0.0f;

// --- Computation ---
float computeDewPoint(float tempC, float relHum) {
    // TODO: implement Magnus formula using natural logarithm
    gamm = log(relHum / 100) + ((a * tempC) / (b + tempC));

    return ((b * gamm) / (a - gamm));
}

void setup() {
    Serial.begin(115200);
    while (!Serial && millis() < 3000);

    // TODO: initialize sensor
    dht.begin();
}

void loop() {
    unsigned long now = millis();
    // TODO: implement non-blocking sampling (2 s)
    if (now - lastSample >= SerialDelay) {

    

    // TODO: read temperature and humidity


    // TODO: handle invalid readings (NaN)
    // THIS solultion should work, but does not
    /*

    float temponread = dht.readTemperature();
    float humidonread = dht.readHumidity();
    if (temponread != temponread || humidonread != humidonread) {
//  if (isnan(temponread) || isnan(humidonread)) // alternative Variante, aber beide funktionieren nichjt richtig, wurde auch im Forum nachgefragt
        fcounter ++;
    }
    else {
        fcounter = 0;
        temp = temponread;
        humid = humidonread;
    }
    */

    // new Solution
    if (dht.read()) {   // returnse False if nan value apears
    temp = dht.readTemperature();
    humid = dht.readHumidity();
    fcounter = 0;
    } else {
        fcounter++;
    }


    // - update failure counter (reset on success)
    // - reuse last valid values if needed

    // TODO: compute dew point, print formatted output. If failure count exceeds threshold print a warning.
    dewPoint = computeDewPoint(temp, humid);

    // we added the information, that it is using the last valid Measurment, if the
    // first NaN value apears
    // if failurwarningthreshhold is exceeded the [WARN] will be added
    if (fcounter > failurwarningthreshhold) {

        Serial.print("[WARN] DHT failures: ");
        Serial.println(fcounter);
        Serial.print("temp: ");
        Serial.print(temp, 1);
        Serial.print("°C");
        Serial.print(" humid: ");
        Serial.print(humid, 1);
        Serial.print("%");
        Serial.print(" dewpoint ");
        Serial.print(dewPoint, 1);
        Serial.print("C");
        Serial.println(" (Using last valid)");
    }

    else if (fcounter > 0) {
        Serial.print("temp: ");
        Serial.print(temp, 1);
        Serial.print("°C");
        Serial.print(" humid: ");
        Serial.print(humid, 1);
        Serial.print("%");
        Serial.print(" dewpoint ");
        Serial.print(dewPoint, 1);
        Serial.print("C");
        Serial.println(" (Using last valid)");
    }
    else {
        Serial.print("temp: ");
        Serial.print(temp, 1);
        Serial.print("°C");
        Serial.print(" humid: ");
        Serial.print(humid, 1);
        Serial.print("%");
        Serial.print(" dewpoint ");
        Serial.print(dewPoint, 1);
        Serial.println("C");
    }


    lastSample = millis();
    }
}