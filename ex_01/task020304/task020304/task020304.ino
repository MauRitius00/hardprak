#include <Adafruit_TinyUSB.h>
#include <Wire.h>
#include <U8g2lib.h>


// Task 2 ii.) Constants — fill in the correct values from the datasheet
//             You can also write the answers to the questions here.
//– Which address belongs to the SGP30?
// => 0x58
//– What is the two-byte command code for initialization?
// => 0x2003
//– What is the two-byte command code for measurement?
// => 0x2008
//– How many bytes does a measurement response contain? What does each byte represent?
// => 6 bytes. 1 byte Most Significant Byte (MSB), 1 byte Least Significant Byte (LSB),
//    1 byte CRC, 1 byte MSB, 1 byte LSB, 1 byte CRC
// ------------------------------------------------------------

#define SGP30_ADDR       0x58   // 7-bit I2C address of the SGP30 

// Command codes (2 bytes each, MSB first — see datasheet )
#define CMD_INIT_MSB     0x20   //    first byte
#define CMD_INIT_LSB     0x03   //    second byte
#define CMD_MEAS_MSB     0x20
#define CMD_MEAS_LSB     0x08

// Display: air quality range for mapping CO2 to a percentage, you can change these to test more ranges
#define CO2_MIN          400    // ppm — clean outdoor air
#define CO2_MAX          2000   // ppm — poor indoor air quality

#define MEAS_INTERVAL_MS  10UL  // 1 second on, 1 second off → 1 Hz

#define THRESH_GOOD 400
#define THRESH_BAD  2000

#define PI 3.1415926535897932384626433832795



unsigned long last_meas_ms = 0;


// ------------------------------------------------------------
//  Display constructor
// ------------------------------------------------------------
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);



//  Task 2 iii.) — Helper functions
  // ------------------------------------------------------------
  //  Raw byte storage for the last SGP30 measurement.
  //
  // We use global variables here to keep the function signatures simple. 
  // The cleaner alternatives (pointers or a result struct) use C concepts not yet introduced. 
  // At this scale globals are fine; in a larger project you would avoid them.
  //
  //  The SGP30 sends each 16-bit value as two separate bytes:
  //    MSB — most significant byte  (upper 8 bits of the value)
  //    LSB — least significant byte (lower 8 bits of the value)
  //  A third byte per value is a CRC checksum — we discard it.
  // ------------------------------------------------------------
uint8_t raw_co2_msb,  raw_co2_lsb;   // assign values with sgp30_read()
uint8_t raw_tvoc_msb, raw_tvoc_lsb;  // assign values with sgp30_read()



void sgp30_cmd(uint8_t msb, uint8_t lsb) {
  // ------------------------------------------------------------
  //  sgp30_cmd : send a 2-byte command to the SGP30
  //
  //  The SGP30 expects all commands as two bytes (MSB first).
  //  Example:  sgp30_cmd(CMD_INIT_MSB, CMD_INIT_LSB);
  // ------------------------------------------------------------
  // TODO: open a transmission to SGP30_ADDR,
  //       write msb, write lsb, close the transmission.
  //
  Wire.beginTransmission(SGP30_ADDR);
  Wire.write(msb);
  Wire.write(lsb);
  Wire.endTransmission();       
}

void draw_per_circle(int x_cent, int y_cent, int rad, int perVal) {
  int circVal = map(perVal, 0, 100, 0, 360);
  for (int i = 0; i < circVal; i++) {
    float radians = i * PI / 180;
    int x = x_cent + rad * cos(radians);
    int y = y_cent + rad * sin(radians);

    u8g2.drawPixel(x, y);
  }
}

bool sgp30_read(uint8_t n) {
  // ------------------------------------------------------------
  //  sgp30_read : read one measurement(n bytes) from the SGP30.
  //
  //  Call sgp30_cmd() first, wait max measurement duration, 
  //  then call this function.
  //  The 4 raw bytes are stored in the global variables above.
  //  The two CRC bytes are read from the bus and discarded.
  //
  //  Returns true if all 6 bytes were received, false on error.
  // ------------------------------------------------------------
  // TODO: use Wire.requestFrom(SGP30_ADDR, n) to request n bytes.
  //       If the return value is not n, return false immediately. 
  //       The expected return value is in the Datasheet.
  //       Read the n bytes in order, remember wire.read() can only
  //       read one byte at a time. Look out for CRC bytes,  
  //       we don't need to store those.  
  //    
  Wire.requestFrom(SGP30_ADDR, n);
  // Serial.print(Wire.available());
  if (Wire.available() != n) {
    return false;
  }
  raw_co2_msb = Wire.read();
  raw_co2_lsb = Wire.read();
  Wire.read();
  raw_tvoc_msb = Wire.read();
  raw_tvoc_lsb = Wire.read();
  Wire.read();
  return true;
}


uint16_t to_uint16(uint8_t msb, uint8_t lsb) {
  // ------------------------------------------------------------
  //  to_uint16 : combine two bytes into one 16-bit value.
  //

  //  A sensor value like CO2 = 450 ppm cannot fit in a single
  //  byte (max 255). The sensor splits it across two bytes:
  //    MSB holds the upper half: 450 >> 8  = 1   (0x01)
  //    LSB holds the lower half: 450 & 0xFF = 194 (0xC2)
  //
  //  To reconstruct the original value:
  //    shift MSB left by 8 bits  →  0x01 becomes 0x0100 (= 256)
  //    OR with LSB               →  0x0100 | 0xC2 = 0x01C2 (= 450)
  //
  //  The cast to uint16_t before shifting is necessary because
  //  uint8_t would overflow when shifted — always cast first.
  //
  // ------------------------------------------------------------
  return ((uint16_t)msb << 8) | lsb;
}



//  Task 3 — Display helper  (optional, but keeps loop() clean)

void display_values(uint16_t co2, uint16_t tvoc) {
  // ------------------------------------------------------------
  //  display_values : show co2 and tvoc on the OLED
  //
  // ------------------------------------------------------------
  // TODO (Task 3): set cursor, print co2 and tvoc values.
  u8g2.clearBuffer();

  u8g2.setCursor(0, 10);
  u8g2.print("CO2 [ppm]: ");
  u8g2.print(co2);

  u8g2.setCursor(0, 25);
  u8g2.print("TVOC [ppb]: ");
  u8g2.print(tvoc);
  

  // TODO (Task 4): map co2 to pct (0-100), draw a filled bar with
  //                u8g2.drawBox(x, y, width, height).
  //                Bar width  = map(pct, 0, 100, 0, 128)
  //                Remember: constrain pct to [0, 100] before mapping.

  uint16_t sensVal = constrain(co2, THRESH_GOOD, THRESH_BAD);
  long perVal = map(sensVal, THRESH_GOOD, THRESH_BAD, 0, 100);
  u8g2.setCursor(0, 40);
  u8g2.print("CO2 [%]: ");
  u8g2.print(perVal);
  u8g2.drawBox(0, 60, map(perVal, 0, 100, 0, 128), 2 );

  draw_per_circle(100, 40, 15, perVal);
  u8g2.sendBuffer();
}


void setup() {
  Serial.begin(115200);
  while (!Serial);           // Wait for USB Serial connection
  //     Task 2 i.): I2C scanner
  // TODO: Transmit to each available I2C address, print the adress when receiving an ACK.
  //       You can use decimal adresses when sending but convert them to hex when printing them out.
  //       Use Serial.print(address, HEX) to make it easier.

  //     Task 2 iv.): Initialise SGP30 
  // TODO: send the init command, wait for initialization
  //       and print out a message.

  Wire.begin();
  sgp30_cmd(CMD_INIT_MSB, CMD_INIT_LSB);
  delay(20);

  delay(1000);
  for(int address = 7; address < 128; address++){
    Wire.beginTransmission(address);
    int error = Wire.endTransmission();
    if(error == 0){
      Serial.print("0x");
      Serial.println(address, HEX);
    }
  }



 

  // --- Task 3 i.): Simple display use ---
  // TODO: initialize display, set a font, display "Hardware Praktikum 2026",
  //       and push it to the screen.
  u8g2.begin();
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_6x10_tf);
  u8g2.drawStr(0,10,"Hardware Praktikum");
  u8g2.sendBuffer();

}


void loop() {
  // --- Task 2 iv.): Send measure command and read response ---
  // TODO: call sgp30_cmd() with the measure command bytes.
  // TODO: call sgp30_read().
  //       If it returns false, print an error message and return early.
  // TODO: Reconstruct 16-bit values from the raw bytes ----
  // TODO: print co2 and tvoc with appropriate labels and units.
  if (millis() - last_meas_ms >= MEAS_INTERVAL_MS) {
    last_meas_ms = millis();
    sgp30_cmd(CMD_MEAS_MSB, CMD_MEAS_LSB);
    delay(20);
    if (!sgp30_read(6)) {
      Serial.println("[ERROR] Read returned False");
    }
    else {
      Serial.print("CO2: ");
      Serial.print(to_uint16(raw_co2_msb, raw_co2_lsb));
      Serial.print(" ppm");
      Serial.print("\nVOC: ");
      Serial.print(to_uint16(raw_tvoc_msb, raw_tvoc_lsb));
      Serial.print(" ppb");
      Serial.print("\n");
      // --- Task 3 ii.): Print the sgp30 values on the display
      //                  in addition to the Serial monitor 
      display_values(to_uint16(raw_co2_msb, raw_co2_lsb), to_uint16(raw_tvoc_msb, raw_tvoc_lsb));
    }
  }


  // --- Task 4: Map CO2 to a percentage ---
  // TODO: use map() to scale co2 from raw values to 0-100%.
  //       Then use constrain() to make sure the percetange 
  //       doesnt go outside 0-100.


}