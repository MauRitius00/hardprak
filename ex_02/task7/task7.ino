// ------------------------------------------------------------
//  Task 7:
//      Use your code from Task 6, adding a parser function to
//      go over any given string and create the array needed to play 
//      it.
//      You are free to use or discard any helper functions and add
//      any helper functions you need for parsing.
// ------------------------------------------------------------

#include <Adafruit_TinyUSB.h>
#include <Arduino.h>
#include <Wire.h>
#include <U8g2lib.h>

#define MAX_MELODY_LEN 100

const char noteNames[] = {'c', 'C', 'd', 'D', 'e', 'f', 'F', 'g', 'G', 'a', 'A', 'b'};
const uint16_t notes[] = {262, 277, 294, 311, 330, 349, 370, 392, 415, 440, 466, 494};

char buffer[] = "Test:d=4,o=5,b=200:8g,8a,8c6,8a,e6,8p,e6,8p,d6.,8p,8g,8a,8c6,8a,d6,8p,d6,8p,c6,8p,a.,8g,8a,8c6,8a,2c6,d6,b,a,g.,8p,g,2d6,2c6.,p,8g,8a,8c6,8a,e6,8p,e6,8p,d6.,8p,8g,8a,8c6,8a,g6,b,c6,8p,b,8a,p,8g,8a,8c6,8a,2c6,d6,b,a,g,p,g,d6,c6";

char song1[] = "GoodSong1:d=4,o=4,b=112:c,d#,f.,c,d#,8f#,f,p,c,d#,f.,d#,c";
char emptySong[] = "empty"; // test case without melody
char song2[] = "GoodSong2:o=5,d=4,b=320,b=320:c,8d,8d,d,2d,c,c,c,c,8d#,8d#,2d#,d,d,d,c,8d,8d,d,2d,c,c,c,c,8d#,8d#,d#,2d#,d,c#,c,c6,1b.,g,f,1g.";
char song3[] = "GoodSong3:o=5,d=8,b=112,b=112:d,d,a,d,e6,d,d6,d,f#,g,c6,f#,g,c6,e,d,d,d,a,d,e6,d,d6,d,f#,g,c6,f#,g,c6,e,d,c,d,a,d,e6,d,d6,d,f#,g,c6,f#,g,c6,e,d,c,d,a,d,e6,d,d6,d,a,d,e6,d,d6";
char song4[] = "GoodSong4:o=5,d=8,b=125,b=125:16g,16g,a#.,16g,16p,16g,c6,g,f,4g,d6.,16g,16p,16g,d#6,d6,a#,g,d6,g6,16g,16f,16p,16f,d,a#,2g,4p,16f6,d6,c6,a#,4g,a#.,16g,16p,16g,c6,g,f,4g,d6.,16g,16p,16g,d#6,d6,a#,g,d6,g6,16g,16f,16p,16f,d,a#,2g";
char song5[] = "GoodSong5:o=5,d=16,b=100,b=100:g,g,a,a,e,e,8g,g,g,a,a,e,e,8g,g,g,a,a,c6,c6,8b,8b,8a,8g,8f,f,f,g,g,d,d,8f,f,f,g,g,d,d,8f,f,f,g,g,a,b,8c6,8a,8g,8e,4c";
char song6[] = "GoodSong6:o=5,d=8,b=140,b=140:g,e,4p,p,e,f,g,e6,p,e6,p,2c6,g,e,4p,p,e,f,e,g,p,g,p,2f,f,d,4p,p,d,e,f,g,e,4p,p,e,f#,e,d,g,p,e,f#,d,p,a,g.,16f#,g,a,g,f,e,d";
char song7[] = "GoodSong7:o=5,d=8,b=63,b=63:a4,c,e,a,b,e,c,b,c6,e,c,c6,f#,d,a4,f#,e.,16c,a4,4e,c,a4,e,g4,a4,4a4";
char song8[] = "GoodSong8:o=5,d=8,b=200,b=200:g#,4c#,p,4c#6,a#,4g#,4c#,p,4g#,f#,f,f,f#,g#,4c#,4d#,2f,2p,4g#,4c#,p,4c#6,a#,4g#,4c#,p,4g#,f#,f,f#,g#,4c#,4d#,2c#";
char song9[] = "GoodSong9:o=4,d=8,b=125,b=125:c6,c6,a#5,c6,p,g5,p,g5,c6,f6,e6,c6,2p,c6,c6,a#5,c6,p,g5,p,g5,c6,f6,e6,c6";
char song10[] = "GoodSong10:o=5,d=8,b=160,b=160:c#6,a#,2p,a#,g#,f#,g#,a#,4c#6,a#,4c#6,d#6,a#,2p,a#,g#,f#,g#,a#,4c#6,a#,4c#6,d#6,b,2p,b,a#,g#,a#,b,4d#6,f#6,4d#6,4f6.,4d#6.,4c#6.,4b.,4a#,4g#";
char song11[] = "GoodSong11:o=5,d=16,b=125,b=125:b,a,4b,4e,4p,8p,c6,b,8c6,8b,4a,4p,8p,c6,b,4c6,4e,4p,8p,a,g,8a,8g,8f#,8a,4g.,f#,g,4a.,g,a,8b,8a,8g,8f#,4e,4c6,2b.,b,c6,b,a,1b";


char *songs[] = {
  buffer,
  song1,
  emptySong,
  song2,
  song3,
  song4,
  song5,
  song6,
  song7,
  song8,
  song9,
  song10,
  song11
};

const uint8_t songCount = sizeof(songs) / sizeof(songs[0]);

uint16_t durations[MAX_MELODY_LEN];
uint16_t melody[MAX_MELODY_LEN];

volatile uint8_t melodyIdx = 0;
volatile uint8_t melodyLen = 0;
volatile uint32_t tCount = 0;

uint16_t standardDuration = 4;
uint16_t standardOctave = 6;
uint16_t standardBPM = 63;

volatile bool currentToggleState = false;

uint8_t currentSong = 0;
char currentSongName[18];

bool lastButtonState = false;
uint32_t lastButtonTime = 0;

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

void setP029(bool high);
void setTimer1Freq(uint32_t freq);
void setTimer2(bool enable);
void playRTTTL();
void parseRTTTL(char *buf);
bool parseRTTLNote(char *buf, uint16_t *idx);
uint16_t freqFromNote(char note, bool sharp);
uint16_t str2uint(char *buf, uint16_t *idx);
bool isDigit(char c);
char lowerChar(char c);
void setBuzzerFreq(uint32_t freq);
void showSongName();
void copySongName(char *buf);
bool buttonPressed();


void setup() {
  Serial.begin(115200);
  while (!Serial);

  u8g2.begin();
  u8g2.setFont(u8g2_font_6x10_tf);

  NRF_P0->DIRSET = (1UL << 29);
  NRF_P0->OUTCLR = (1UL << 29);

  NRF_P0->PIN_CNF[3] = 12;

  setTimer1Freq(1046);

  NRF_TIMER1->TASKS_STOP = 1;
  NRF_TIMER1->TASKS_CLEAR = 1;

  playRTTTL();
}


void loop() {
  bool pressed = buttonPressed();

  if (pressed && !lastButtonState && millis() - lastButtonTime > 250) {
    lastButtonTime = millis();

    currentSong++;

    if (currentSong >= songCount) {
      currentSong = 0;
    }

    playRTTTL();
  }



  lastButtonState = pressed;


}


void playRTTTL() {
  setTimer2(false);
  setBuzzerFreq(0);

  parseRTTTL(songs[currentSong]);
  showSongName();

  tCount = 0;
  melodyIdx = 0;

  if (melodyLen > 0) {
    setBuzzerFreq(melody[melodyIdx]);
    setTimer2(true);
  }
}

// with a little Help of ChatGPT for debugging
// in function parseRTTTL and parseRTTLNote
void parseRTTTL(char *buf) {
  uint16_t idx = 0;

  standardDuration = 4;
  standardOctave = 6;
  standardBPM = 63;
  melodyLen = 0;

  copySongName(buf);

  while (*(buf + idx) != '\0' && buf[idx] != ':') {
    idx++;
  }

  if (buf[idx] == ':') {
    idx++;
  }

  while (buf[idx] != '\0' && *(buf + idx) != ':') {
    char param = lowerChar(buf[idx]);
    idx++;

    if (buf[idx] == '=') {
      idx++;
    }

    uint16_t val = str2uint(buf, &idx);

    if (param == 'd') {
      standardDuration = val;
    } else if (param == 'o') {
      standardOctave = val;
    } else if (param == 'b') {
      standardBPM = val;
    }

    if (*(buf + idx) == ',') {
      idx++;
    }
  }

  if (*(buf + idx) == ':') {
    idx++;
  }

  while (buf[idx] != '\0' && melodyLen < MAX_MELODY_LEN) {
    if (!parseRTTLNote(buf, &idx)) {
      break;
    }
  }
}


bool parseRTTLNote(char *buf, uint16_t *idx) {
  uint16_t noteDuration = standardDuration;
  uint16_t octave = standardOctave;

  if (buf[*idx] == '\0') {
    return false;
  }

  if (isDigit(buf[*idx])) {
    noteDuration = str2uint(buf, idx);
  }

  char note = buf[*idx];

  if (note == '\0' || note == ',') {
    return false;
  }

  *idx = *idx + 1;

  bool sharp = false;

  if (buf[*idx] == '#') {
    sharp = true;
    *idx = *idx + 1;
  }

  if (isDigit(buf[*idx])) {
    octave = str2uint(buf, idx);
  }

  if (noteDuration == 0) {
    noteDuration = 4;
  }

  if (standardBPM == 0) {
    standardBPM = 63;
  }

  uint32_t freq = freqFromNote(note, sharp);

  if (freq > 0) {
    while (octave > 4) {
      freq *= 2;
      octave--;
    }

    while (octave < 4) {
      freq /= 2;
      octave++;
    }
  }

  uint32_t durationMs = 240000UL / ((uint32_t)standardBPM * noteDuration);

  if (buf[*idx] == '.') {
    durationMs += durationMs / 2;
    *idx = *idx + 1;
  }

  melody[melodyLen] = (uint16_t)freq;
  durations[melodyLen] = (uint16_t)durationMs;
  melodyLen++;

  if (buf[*idx] == ',') {
    *idx = *idx + 1;
  }

  return true;
}


uint16_t freqFromNote(char note, bool sharp) {
  int8_t noteIdx = -1;

  switch (note) {
    case 'c': noteIdx = 0; break;
    case 'C': noteIdx = 1; break;
    case 'd': noteIdx = 2; break;
    case 'D': noteIdx = 3; break;
    case 'e': noteIdx = 4; break;
    case 'f': noteIdx = 5; break;
    case 'F': noteIdx = 6; break;
    case 'g': noteIdx = 7; break;
    case 'G': noteIdx = 8; break;
    case 'a': noteIdx = 9; break;
    case 'A': noteIdx = 10; break;
    case 'b': noteIdx = 11; break;
    case 'h': noteIdx = 11; break;
    case 'p': return 0;
    case 'P': return 0;
    default: return 0;
  }

  if (sharp) {
    noteIdx++;

    if (noteIdx >= 12) {
      noteIdx = 0;
    }
  }

  return notes[noteIdx];
}


uint16_t str2uint(char *buf, uint16_t *idx) {
  uint16_t val = 0;

  while (isDigit(buf[*idx])) {
    val = (val * 10) + (buf[*idx] - '0');
    *idx = *idx + 1;
  }

  return val;
}


bool isDigit(char c) { 
  if (c < '0' || c > '9') { // ASCII comparison
    return false;
  }

  return true;
}


char lowerChar(char c) {
  if (c >= 'A' && c <= 'Z') {
    return c + 32;
  }

  return c;
}


void setBuzzerFreq(uint32_t freq) {
  if ((100 <= freq) && (freq <= 3000)) {
    NRF_TIMER1->CC[0] = 1000000UL / (2UL * freq);
    NVIC_ClearPendingIRQ(TIMER1_IRQn);
    NRF_TIMER1->TASKS_CLEAR = 1;
    NRF_TIMER1->TASKS_START = 1;
  } else {
    setP029(false);
    NRF_TIMER1->TASKS_STOP = 1;
  }
}


void setTimer1Freq(uint32_t freq) {
  NRF_TIMER1->TASKS_STOP = 1;
  NRF_TIMER1->TASKS_CLEAR = 1;

  NRF_TIMER1->MODE = 0;
  NRF_TIMER1->BITMODE = 3;
  NRF_TIMER1->PRESCALER = 4;

  NRF_TIMER1->CC[0] = 1000000UL / (2UL * freq);
  NRF_TIMER1->SHORTS = TIMER_SHORTS_COMPARE0_CLEAR_Msk;
  NRF_TIMER1->EVENTS_COMPARE[0] = 0;
  NRF_TIMER1->INTENSET = (1UL << 16);

  NVIC_ClearPendingIRQ(TIMER1_IRQn);
  NVIC_EnableIRQ(TIMER1_IRQn);

  NRF_TIMER1->TASKS_START = 1;
}


extern "C" void TIMER1_IRQHandler() {
  if (NRF_TIMER1->EVENTS_COMPARE[0]) {
    NRF_TIMER1->EVENTS_COMPARE[0] = 0;

    currentToggleState = !currentToggleState;
    setP029(currentToggleState);
  }
}


void setTimer2(bool enable) {
  if (enable) {
    NRF_TIMER2->TASKS_STOP = 1;
    NRF_TIMER2->TASKS_CLEAR = 1;

    NRF_TIMER2->MODE = 0;
    NRF_TIMER2->BITMODE = 3;
    NRF_TIMER2->PRESCALER = 4;

    NRF_TIMER2->CC[0] = 1000;
    NRF_TIMER2->SHORTS = TIMER_SHORTS_COMPARE0_CLEAR_Msk;
    NRF_TIMER2->EVENTS_COMPARE[0] = 0;
    NRF_TIMER2->INTENSET = (1UL << 16);

    NVIC_ClearPendingIRQ(TIMER2_IRQn);
    NVIC_EnableIRQ(TIMER2_IRQn);

    NRF_TIMER2->TASKS_START = 1;
  } else {
    NRF_TIMER2->TASKS_STOP = 1;
    NRF_TIMER2->TASKS_CLEAR = 1;
    NRF_TIMER2->EVENTS_COMPARE[0] = 0;
  }
}


extern "C" void TIMER2_IRQHandler() {
  if (NRF_TIMER2->EVENTS_COMPARE[0]) {
    NRF_TIMER2->EVENTS_COMPARE[0] = 0;

    tCount++;

    if (melodyLen == 0) {
      setBuzzerFreq(0);
      setTimer2(false);
      return;
    }

    if (tCount >= durations[melodyIdx]) {
      tCount = 0;
      melodyIdx++;

      if (melodyIdx >= melodyLen) {
        setBuzzerFreq(0);
        setTimer2(false);
      } else {
        setBuzzerFreq(melody[melodyIdx]);
      }
    }
  }
}


void setP029(bool high) {
  if (high) {
    NRF_P0->OUTSET = (1UL << 29);
  } else {
    NRF_P0->OUTCLR = (1UL << 29);
  }
}


void copySongName(char *buf) {
  uint8_t i = 0;

  while (buf[i] != ':' && i < 17) {
    currentSongName[i] = buf[i];
    i++;
  }

  currentSongName[i] = '\0';

  if (i == 0) {
    currentSongName[0] = '-';
    currentSongName[1] = '\0';
  }
}


void showSongName() {
  u8g2.clearBuffer();
  u8g2.drawStr(0, 12, "Current song:");
  u8g2.drawStr(0, 28, currentSongName);
  u8g2.drawStr(0, 48, "Button: next song");
  u8g2.sendBuffer();
}

 // active low (if button pressed the value of the register is 0 -> returns true).
 // Datasheet: PIN[i] (i=0...31). Button is PIN[3]
bool buttonPressed() {
  return (NRF_P0->IN & (1UL << 3)) == 0;
}