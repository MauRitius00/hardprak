// Task 4: Smart Plant Monitoring Station

// #include <Adafruit_TinyUSB.h>
#include <ArduinoBLE.h>
#include <Wire.h>
#include <Adafruit_SGP30.h>
#include <U8g2lib.h>
#include <DHT.h>


// --- Hardware Configuration ---
#define analogPin   A0
#define DHTPIN      D7
#define DHTTYPE     DHT11
#define BUZZER_PIN  D3

// define air quality object
Adafruit_SGP30 sgp;

// define humidity and temp object
DHT dht(DHTPIN, DHTTYPE);


// --- System Constants ---
// sensor sampling
const int LightDelay = 500;
const int HumidDelay = 2000;
const int AirqualDelay = 1000;
const int ScoreDelay = 1000;
const int SerialDelay = 1000; // debugging

// display refresh (2 Hz)
const int displayDelay = 500;

// BLE transmission (1 Hz)
const int BLEDelay = 1000;

// warm-up 30 s
const int warmUpDelay = 30000;

// LED
const int LEDDelayAttention = 500;
const int LEDDelayStressed  = 250;

const int analogResolution = 12;

// light sensor normalization range
const int LightMin = 50;
const int LightMax = 3500;

// Timing
unsigned long lsLight = 0;
unsigned long lsAirqual = 0;
unsigned long lsHumid = 0;
unsigned long lsWarmup = 0;
unsigned long lsScore = 0;
unsigned long lsDisplay = 0;
unsigned long lsBLE = 0;
unsigned long lsLED = 0;
unsigned long lsSerial = 0; // for debugging

// Sensor Data
int normalizedLightVal = 0;
int eCo2val = 400;
float tempVal = 0.0;
float humidVal = 0.0;
int score = 0;

// --- Objects ---
// display
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

// BLE service and characteristic
BLEService plantService("181A");
BLEStringCharacteristic telemetryChar("2A3D", BLERead | BLENotify, 150);


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

// short names to print in on BLE
const char* shortState[] = {
    "I", // Init
    "H", // Healthy
    "A", // Attention
    "S"  // Stressed
};

SystemState currentState = STATE_INIT;


// Helper Functions 

// compute health score from current sensor values
void computeScore() {
    score = 0;
    if (normalizedLightVal >= 25 && normalizedLightVal <= 90)
        score += 25;

    if (tempVal  >= 18 && tempVal  <= 30) // TODO aendern!!
        score += 25;

    if (humidVal >= 30 && humidVal <= 75)
        score += 25;

    if (eCo2val  < 1200)
    score += 25;
}

// state transition based on score
void updateState() {
    if (eCo2val > 2200) {
        currentState = STATE_STRESSED;
        return;
    }
    if (score >= 75)
    currentState = STATE_HEALTHY;

    else if (score >= 50)
    currentState = STATE_ATTENTION;

    else
    currentState = STATE_STRESSED;
}

// print Values on OLED
void display_values() {
    u8g2.clearBuffer();

    u8g2.setCursor(0, 10);
    u8g2.print("SysState: ");
    u8g2.print(stateNames[currentState]);

    u8g2.setCursor(0, 20);
    u8g2.print("Temp. [C]: ");
    u8g2.print(tempVal, 1);

    u8g2.setCursor(0, 30);
    u8g2.print("Humidity [%]: ");
    u8g2.print(humidVal, 1);

    u8g2.setCursor(0, 40);
    u8g2.print("Light Level [%]: ");
    u8g2.print(normalizedLightVal);

    u8g2.setCursor(0, 50);
    u8g2.print("eCO2 [ppm]: ");
    u8g2.print(eCo2val);

    u8g2.setCursor(0, 60);
    u8g2.print("Score: ");
    u8g2.print(score);

    u8g2.sendBuffer();
}

// Manage LED blinking
void displayLight() {
    unsigned long now = millis();
    static bool ledOn = false;

    // LED aus
    if (currentState == STATE_INIT) {
        digitalWrite(LED_GREEN, HIGH); // off
        ledOn = false;
        return;
    }

    // LED steady on
    if (currentState == STATE_HEALTHY) {
        digitalWrite(LED_GREEN, LOW);
        ledOn = true;
        return;
    }

    // toggle 500 ms and toggle 250 ms
    int currentDelay = (currentState == STATE_ATTENTION)
                       ? LEDDelayAttention
                       : LEDDelayStressed;

    if (now - lsLED >= (unsigned long)currentDelay) {
        ledOn = !ledOn;
        digitalWrite(LED_GREEN, ledOn ? LOW : HIGH); // active LOW
        lsLED = now;
    }
}

// aktivate Buzzer 
void updateBuzzer() {
    static bool alarmWasOn = false;

    bool alarmOn = (eCo2val > 2200);

    if (alarmOn && !alarmWasOn) {
        tone(BUZZER_PIN, 1000);
        alarmWasOn = true;
    }

    if (!alarmOn && alarmWasOn) {
        noTone(BUZZER_PIN);
        alarmWasOn = false;
    }
}


// --- Setup ---
void setup() {
    Serial.begin(115200);

    // pin modes
    analogReadResolution(analogResolution);
    pinMode(LED_GREEN, OUTPUT);
    digitalWrite(LED_GREEN, HIGH); // off
    pinMode(BUZZER_PIN, OUTPUT);

    // DHT sensor
    dht.begin();

    // SGP30 air quality sensor
    if (!sgp.begin()) {
        Serial.println("[ERROR] SGP30 not found");
    }

    // display
    u8g2.begin();
    u8g2.setFont(u8g2_font_6x10_tf);
    u8g2.clearBuffer();
    u8g2.setCursor(0, 10);
    u8g2.print("Hardware Praktikum");
    u8g2.sendBuffer();

    // BLE
    if (!BLE.begin()) {
        Serial.println("[ERROR] BLE init failed");
    }
    BLE.setLocalName("PlantMonitor_G4");
    BLE.setAdvertisedService(plantService);
    plantService.addCharacteristic(telemetryChar);
    BLE.addService(plantService);
    telemetryChar.writeValue("T=0 H=0 L=0 C=0 S=Init");
    BLE.advertise();
    Serial.println("[BLE] Advertising...");

    lsWarmup = millis();
}


// --- Loop ---
void loop() {
    unsigned long now = millis();

    // maintain BLE stack
    BLE.poll();


    // i) Asynchronous sensor acquisition (light, DHT, SGP30)
    // Timers for each value
    if (now - lsLight >= (unsigned long)LightDelay) {
        int analogValue = analogRead(analogPin);
        int sensVal = constrain(analogValue, LightMin, LightMax);
        normalizedLightVal = map(sensVal, LightMin, LightMax, 0, 100);
        lsLight = now;
    }

    if (now - lsAirqual >= (unsigned long)AirqualDelay) {
        if (sgp.IAQmeasure()) {
            eCo2val = sgp.eCO2;
        }
        lsAirqual = now;
    }

    if (now - lsHumid >= (unsigned long)HumidDelay) {
        float t = dht.readTemperature();
        float h = dht.readHumidity();
        if (!isnan(t)) tempVal  = t;
        if (!isnan(h)) humidVal = h;
        lsHumid = now;
    }



    // iii) Warm-up handling (STATE_INIT for 30 s)

    // for debugging
    if (now - lsSerial >= (unsigned long)SerialDelay) {
        Serial.print("eCO2: "); Serial.print(eCo2val);
        Serial.print(" | Temp: "); Serial.print(tempVal);
        Serial.print(" | Humid: "); Serial.print(humidVal);
        Serial.print(" | Light: "); Serial.print(normalizedLightVal);
        Serial.print(" | State: "); Serial.println(stateNames[currentState]);

        lsSerial = now;
    }

    if (currentState == STATE_INIT) {
        if (now - lsDisplay >= (unsigned long)displayDelay) {
            u8g2.clearBuffer();
            u8g2.setCursor(0, 20);
            u8g2.print("Warming up...");
            u8g2.setCursor(0, 35);
            u8g2.print((warmUpDelay - (long)(now - lsWarmup)) / 1000); // print seconds
            u8g2.print(" s");
            u8g2.sendBuffer();
            lsDisplay = now;
        }

        if (now - lsWarmup >= (unsigned long)warmUpDelay) {
            computeScore();
            updateState();
        }

        displayLight();
        updateBuzzer();
        return;
    }


    // iii) Health score (1 Hz)
    if (now - lsScore >= (unsigned long)ScoreDelay) {
        computeScore();
        updateState();
        lsScore = now;
    }

    // iv) Update OLED display (~2 Hz)
    if (now - lsDisplay >= (unsigned long)displayDelay) {
        display_values();
        lsDisplay = now;
    }

    // v) LED and buzzer (non-blocking)
    displayLight();
    updateBuzzer();

    // vi) BLE telemetry (1 Hz)
    if (now - lsBLE >= (unsigned long)BLEDelay) {
        char buf[50];

        snprintf(buf, sizeof(buf), "S%s T%.1f H%.0f L%d C%d",
                shortState[currentState],
                tempVal, humidVal, normalizedLightVal, eCo2val);

        telemetryChar.writeValue(buf);
        Serial.println(buf);

        lsBLE = now;
    }
}