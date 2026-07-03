/*
 * Exercise 04: Wearable Context Recognition with IMU
 * Author: Roozbeh Ghazavi
 * Year: 2026
 * Board: XIAO nRF52840 Sense
 * Sensor: LSM6DS3
 *
 * Implement ALL TODO sections.
 */

#include <Arduino.h>
#include "LSM6DS3.h"
#include "Wire.h"

// =====================
// Part E: BLE Library
// =====================
#include <ArduinoBLE.h>

LSM6DS3 myIMU(I2C_MODE, 0x6A);

// Constants
#define CONVERT_G_TO_MS2 9.80665f
#define FREQUENCY_HZ 50
#define INTERVAL_MS (1000 / (FREQUENCY_HZ + 1))


static unsigned long last_interval_ms = 0;

// =====================
// Part C: Accelerometer Buffer (Z-axis)
// =====================
#define WINDOW_SIZE 10

float az_buffer[WINDOW_SIZE];
int buffer_index = 0;
bool buffer_full = false;


// =====================
// Part C & D: Last Detected Gestures
// =====================
// TODO: Create variables to store last detected gestures
// - lastDetectedGesture: from accelerometer FSM
// - lastDynamicGesture: from gyroscope

String lastDetectedGesture = "NONE";
String lastDynamicGesture = "NONE";



// =====================
// Part E: BLE Objects
// =====================
// TODO: Create BLE service and characteristic
// Example:
// BLEService imuService("180C");
// BLECharacteristic imuCharacteristic("2A56", BLERead | BLENotify, 100);
BLEService imuService("180C");
BLECharacteristic imuCharacteristic("2A56", BLERead | BLENotify, 150);

// =====================
// Part B: Orientation Detection
// =====================
String detectOrientation(float ax, float ay, float az) {

  // TODO: Detect device orientation from accelerometer
  // Use thresholds on ay and az axes

  if (az > 8.0)
    return "Face up";
  else if (az < -8.0)
    return "Face down";
  else if (ay > 8.0)
    return "Right";
  else if (ay < -8.0)
    return "Left";

  return "UNKNOWN";
}


// =====================
// Part C: Accelerometer-based Gesture Detection
// =====================
String detectGestureWindow() {

  // TODO: Detect SUPINATION/PRONATION from Z-axis acceleration buffer
  // Analyze min/max range and motion direction

  if (buffer_full) {
    float min = az_buffer[0];
    float max = az_buffer[0];

    // min/max bestimmen
    for (int i=1; i < 10; i++) {
      if (az_buffer[i] < min) {
        min = az_buffer[i];
      }
      if (az_buffer[i] > max) {
        max = az_buffer[i];
      }
    }

    if ((max - min) > 15.0) {
      if (az_buffer[0] < 0 && az_buffer[9] > 0)
        return "PRONATION";
      else if (az_buffer[0] > 0 && az_buffer[9] < 0)
        return "SUPINATION";
    }
  }

  return "NONE";
}


// =====================
// Part D: Gyroscope-based Dynamic Gesture Detection
// =====================
String detectDynamicGesture(float gyrX, float gyrY, float gyrZ) {

  // TODO: Detect 6 gestures using gyroscope
  // Gestures: TILT_LEFT, TILT_RIGHT, MOVE_UP, MOVE_DOWN, MOVE_LEFT, MOVE_RIGHT
  // Return strongest gesture (highest magnitude)

  
  // largest value with an absolute value greater than 100
  if (abs(gyrX) > 100 && abs(gyrX) > abs(gyrY) && abs(gyrX) > abs(gyrZ)) {
    // gyrX is the largest value
    if (gyrX < 0)
      return "TILT_LEFT";
    else
      return "TILT_RIGHT";
  } else if (abs(gyrY) > 100 && abs(gyrY) > abs(gyrX) && abs(gyrY) > abs(gyrZ)) {
    // gyrY is the largest value
    if (gyrY < 0)
      return "MOVE_DOWN";
    else
      return "MOVE_UP";
  } else if (abs(gyrZ) > 100 && abs(gyrZ) > abs(gyrX) && abs(gyrZ) > abs(gyrY)) {
    // gyrZ is the largest value
    if (gyrZ < 0)
      return "MOVE_RIGHT";
    else
      return "MOVE_LEFT";
  }

  return "NONE";
}


// =====================
// Part C (Bonus): Gesture State Machine (FSM)
// =====================
enum State {
  IDLE,
  MOVING,
  DETECTED
};

State currentState = IDLE;

String detectGestureFSM() {

  // TODO: Implement FSM for robust gesture detection
  // Transitions: IDLE → MOVING → DETECTED → IDLE
  // Store detected gesture in lastDetectedGesture

  return "NONE";
}

void setup() {
  Serial.begin(115200);
  while (!Serial);

  if (myIMU.begin() != 0) {
    Serial.println("IMU initialization failed!");
    while (1);
  }

  Serial.println("IMU initialized.");

  // =====================
  // Part E: BLE Setup
  // =====================
  // TODO: Initialize BLE, add service/characteristic, start advertising
  if (!BLE.begin()) {
        Serial.println("[ERROR] BLE init failed");
    }
    BLE.setLocalName("IMU_ex4");
    BLE.setAdvertisedService(imuService);
    imuService.addCharacteristic(imuCharacteristic);
    BLE.addService(imuService);
    BLE.advertise();
    Serial.println("[BLE] Advertising...");
}


void loop() {
  // maintain BLE stack
  BLE.poll();

  if (millis() > last_interval_ms + INTERVAL_MS) {
    last_interval_ms = millis();

    // =====================
    // Part A: IMU Data Acquisition
    // =====================
    // TODO: Read accelerometer (ax, ay, az) from myIMU
    // TODO: Convert accelerometer from G to m/s² using CONVERT_G_TO_MS2
    // TODO: Read gyroscope (gyrX, gyrY, gyrZ) from myIMU

    float ax = myIMU.readFloatAccelX() * CONVERT_G_TO_MS2;
    float ay = myIMU.readFloatAccelY() * CONVERT_G_TO_MS2;
    float az = myIMU.readFloatAccelZ() * CONVERT_G_TO_MS2;

    float gyrX = myIMU.readFloatGyroX();
    float gyrY = myIMU.readFloatGyroY();
    float gyrZ = myIMU.readFloatGyroZ();


    // =====================
    // Part C: Accelerometer Buffer Management
    // =====================
    az_buffer[buffer_index] = az;
    buffer_index++;

    if (buffer_index >= WINDOW_SIZE) {
      buffer_index = 0;
      buffer_full = true;
    }


    // =====================
    // Part B: Orientation Detection
    // =====================
    String orientation = detectOrientation(ax, ay, az);


    // =====================
    // Part C: Accelerometer-based Gesture Detection
    // =====================
    // TODO: Call detectGestureWindow() or detectGestureFSM()
    // TODO: If gesture detected (not "NONE"), store in lastDetectedGesture
    String detectedGesture = detectGestureWindow();

    if (!detectedGesture.equals("NONE"))
      lastDetectedGesture = detectedGesture;

    // =====================
    // Part D: Gyroscope-based Dynamic Gesture Detection
    // =====================
    // TODO: Call detectDynamicGesture()
    // TODO: If gesture detected (not "NONE"), store in lastDynamicGesture
    String dynamicGesture = detectDynamicGesture(gyrX, gyrY, gyrZ); 

    if (!dynamicGesture.equals("NONE"))
      lastDynamicGesture = dynamicGesture;

    // =====================
    // Serial Output (USB)
    // =====================
    Serial.print("ax: "); Serial.print(ax);
    Serial.print(" | ay: "); Serial.print(ay);
    Serial.print(" | az: "); Serial.print(az);

    Serial.print(" | gyrX: "); Serial.print(gyrX);
    Serial.print(" | gyrY: "); Serial.print(gyrY);
    Serial.print(" | gyrZ: "); Serial.print(gyrZ);

    Serial.print(" | Orientation: ");
    Serial.print(orientation);

    Serial.print(" | Accelerometer Gesture: ");
    Serial.print(lastDetectedGesture);

    Serial.print(" | Gyro Gesture: ");
    Serial.println(lastDynamicGesture);
  

    // =====================
    // Part E: Bluetooth Low Energy (BLE) Communication
    // =====================
    // TODO: Format and send data via BLE:
    // - Accelerometer readings (ax, ay, az)
    // - Gyroscope readings (gyrX, gyrY, gyrZ)
    // - Orientation detection result
    // - Gesture detection results (FSM and Gyro)

    BLEDevice central = BLE.central();

    if (central) {
      char buf[150];

      snprintf(buf, sizeof(buf), "ax: %.2f | ay: %.2f | az: %.2f | gyrX: %.2f | gyrY: %.2f | gyrZ: %.2f | Orientation: %s | FSM Gesture: %s | Gyro Gesture: %s",
              ax, ay, az,
              gyrX, gyrY, gyrZ,
              orientation.c_str(),
              lastDetectedGesture.c_str(),
              lastDynamicGesture.c_str());

      imuCharacteristic.writeValue(buf);
    }
    
  }
}