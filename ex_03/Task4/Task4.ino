// Task 4: Smart Plant Monitoring Station

// #include <ArduinoBLE.h>
#include <Wire.h>
#include <Adafruit_SGP30.h>
#include <U8g2lib.h>
#include <DHT.h>
#include <Adafruit_TinyUSB.h>


// --- Hardware Configuration ---
// TODO: define pins (DHT, light sensor, LED, buzzer)
// define Light Sensor
#define analogPin A0

// define Temperature and Humidity Monitoring sensor pin and type (DHT11)
#define DHTPIN D7
#define DHTTYPE DHT11

// define air quality object
Adafruit_SGP30 sgp;

// define humidity and temp object
DHT dht(DHTPIN, DHTTYPE);



// --- System Constants ---2000
// TODO: define timing constants for:
// - sensor sampling
const int LightDelay = 500;
const int HumidDelay = 2000;
const int AirqualDelay = 1000;
const int ScoreDelay = 1000;
unsigned long lsLight = 0;
unsigned long lsAirqual = 0;
unsigned long lsHumid = 0;
unsigned long lsWarmup = 0;
unsigned long lsScore = 0;

// LED
const int LEDDelayAttention = 500;
const int LEDDelpayStressed = 50;
unsigned long lsLED = 0;

const int analogResolution = 12;


// - display refresh (~2 Hz)
const int displayDelay = 500;
unsigned long lsDisplay = 0;

// - BLE transmission (1 Hz)
const int BLEDelay = 1000;

// - warm-up duration (30 s)
const int warmUpDelay = 3000;

// global variables for measurements
int normalizedLightVal;
int eCo2val;
float tempVal;
float humidVal;
int score = 0;

// --- Objects ---
// TODO: initialize display, sensors, BLE service and characteristic
const int LightMin = 50;
const int LightMax = 3500; // Sensor can only read to ~2500 (350 lux)

// ------------------------------------------------------------
//  Display constructor
// ------------------------------------------------------------
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);



// --- FSM ---
enum SystemState {
    STATE_INIT,
    STATE_HEALTHY,
    STATE_ATTENTION,
    STATE_STRESSED
};

const char* stateNames[] = {
    "Init",
    "Healthy",
    "Attention",
    "Stressed"
};

SystemState currentState = STATE_INIT;

// TODO: define timing variables for asynchronous operation

// TODO: define variables for sensor data storage

// TODO: helper function(s), e.g.:
// - state to string conversion
// - value normalization (light)

void display_values() {
  // ------------------------------------------------------------
  //  display_values : show co2 and tvoc on the OLED
  //
  // ------------------------------------------------------------
  // TODO (Task 3): set cursor, print co2 and tvoc values.
  u8g2.clearBuffer();

  u8g2.setCursor(0, 10);
  u8g2.print("SysState: ");
  u8g2.print(stateNames[currentState]);

  u8g2.setCursor(0, 20);
  u8g2.print("Temp. [C]: ");
  u8g2.print(tempVal);

  u8g2.setCursor(0, 30);
  u8g2.print("Humidity [%]: ");
  u8g2.print(humidVal);

  u8g2.setCursor(0, 40);
  u8g2.print("Light Level [%]: ");
  u8g2.print(normalizedLightVal);

  u8g2.setCursor(0, 50);
  u8g2.print("eCO2 [ppm]: ");
  u8g2.print(eCo2val);
  
  u8g2.sendBuffer();

  displayLight();
}

void displayLight() {
    int now = millis();
    if (currentState == STATE_HEALTHY) {
        digitalWrite(LED_GREEN, LOW);
    } else {
        int currentDelay;
    sgp.softReset();

    dht.begin();

        if (currentState == STATE_ATTENTION) {
            currentDelay = LEDDelayAttention;
        } else {
            currentDelay = LEDDelpayStressed;
        }

        if (now - lsLED >= currentDelay) {
            digitalWrite(LED_GREEN, !digitalRead(LED_GREEN));
            lsLED = millis();
        }

    }
}

void setup() {
    Serial.begin(115200);

    sgp.softReset();

    dht.begin();

    // TODO: inloopitialize hardware (pins, display, sensors)
    analogReadResolution(analogResolution);

    sgp.begin();
    if (!sgp.IAQinit()) {
        Serial.println("[ERROR] IAQ Sensor not initialized ");
    }
    sgp.softReset();

    dht.begin();

    // init disloopplay microcontroller
    u8g2.begin();
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_6x10_tf);
    u8g2.drawStr(0,10,"Hardware Praktikum");
    u8g2.sendBuffer();

    // LED setup
    pinMode(LED_GREEN, OUTPUT);


    // TODO: initialize BLE and start advertising
    // TODO: store system start time (for warm-up)
}

void loop() {
    unsigned long now = millis();


    // TODO: maintain BLE stack (if required)


    // i) TODO: asynchronous sensor acquisition (light, DHT, SGP30)
    if (now - lsLight >= LightDelay) { // from ex01
        int analogValue = analogRead(analogPin);
        int sensVal = constrain(analogValue, LightMin, LightMax);
        normalizedLightVal = map(sensVal, LightMin, LightMax, 0, 100);

        lsLight = millis();
    }
        lsHumid = millis();

    if (now - lsAirqual>= AirqualDelay) { // from ex03
        sgp.IAQmeasure();
        eCo2val = sgp.eCO2;

        lsAirqual = millis();
    }

    if (now - lsHumid>= HumidDelay) { // from ex03
        dht.read();
        tempVal = dht.readTemperature();
        humidVal = dht.readHumidity();

        lsHumid = millis();
    }

    Serial.print(" | ");
    Serial.print(eCo2val);
    Serial.print(" | ");
    Serial.print(tempVal);
    Serial.print(" | ");
    Serial.println(humidVal);


    // iii) TODO: warm-up handling (STATE_INIT for 30 s)

    // iii) TODO: compute health score (0–100)
    if (currentState != STATE_INIT){
        if (now - lsScore >= ScoreDelay) {
            score = 0;
            if (normalizedLightVal <= 90 && normalizedLightVal >= 25)
            {
                score += 25;
            }
            if (tempVal <= 30 && tempVal >= 18)
            {
                score += 25;
            }
            if (humidVal <= 75 && humidVal >= 30)
            { 
                score += 25;
            }
            if (eCo2val < 1200)
            {
                score += 25;
            }
            lsScore = millis();
        }
        // update Display currentStateevery 2 seconds
        if (now - lsDisplay >= displayDelay) {
            display_values();
            lsDisplay = millis();
        }
    }



    // iii) TODO: implement state transitions (including critical override)
    switch (currentState) {
        case STATE_INIT:
            u8g2.clearBuffer();
            u8g2.setCursor(0, 20);
            u8g2.print("Warming up ...");
            u8g2.sendBuffer();

            if (now - lsWarmup >= warmUpDelay) {
                switch(score) {
                    case 75 ... 100:
                        currentState = STATE_HEALTHY;
                        display_values();
                        break;
                    case 50 ... 74:
                        currentState = STATE_ATTENTION;
                        display_values();
                        break;                
                    case 0 ... 49:
                        currentState = STATE_STRESSED;
                        display_values();
                        break;
                }
                currentState = STATE_HEALTHY; 
                lsWarmup = millis();
            }
        
            break;

        case STATE_HEALTHY:
            if (eCo2val > 2200) {
                currentState = STATE_STRESSED;
                break;
            }
            switch(score) {
            break;

                case 75 ... 100:
                    currentState = STATE_HEALTHY;
                    display_values();
                    break;
                case 50 ... 74:
                    currentState = STATE_ATTENTION;
                    display_values();
                    break;                
                case 0 ... 49:
                    currentState = STATE_STRESSED;
                    display_values();
                    break;
            }        
            break;

        case STATE_ATTENTION:
            if (eCo2val > 2200) {
                currentState = STATE_STRESSED;
                break;
            }
            switch(score) {
                case 75 ... 100:
                    currentState = STATE_HEALTHY;
                    display_values();
                    break;
                case 50 ... 74:
                    currentState = STATE_ATTENTION;
                    display_values();
                    break;                
                case 0 ... 49:
                    currentState = STATE_STRESSED;
                    display_values();
                    break;
            }        
            break;

        case STATE_STRESSED:
            if (eCo2val > 2200) {
                currentState = STATE_STRESSED;
                break;
            }
            switch(score) {
                case 75 ... 100:
                    currentState = STATE_HEALTHY;
                    display_values();
                    break;
                case 50 ... 74:
                    currentState = STATE_ATTENTION;
                    display_values();
                    break;                
                case 0 ... 49:
                    currentState = STATE_STRESSED;
                    display_values();
                    break;
            }        
            break;

        default:
            break;
    }

    // iv) TODO: update OLED display (~2 Hz)

    // v) TODO: implement LED and buzzer behavior (non-blocking)

    // vi) TODO: send BLE telemetry (formatted string, 1 Hz)
}