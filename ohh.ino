#include "ColorPalettes.h"
#include "FastLED.h"

#define NUM_LEDS 284 
#define DATA_PIN 15

uint8_t G_RING_SEGMENTS = 1;
uint16_t G_START_OFFSET = 0;
uint8_t G_SPIN_AMOUNT = 0;
uint8_t G_SERIAL_MODE = 0;
bool G_SPIN_CLOCKWISE = true;
bool G_ANIMATE_FORWARD = true;
uint16_t G_ANIMATION_FRAME = 0;
uint8_t G_ANIMATION_SPEED = 1;
uint8_t G_COLOR_PALETTE_INDEX = 4;
uint8_t G_ANIMATION_INDEX = 3;
typedef void (*Animations[])(uint16_t segLeg, uint16_t segStart, uint8_t segNum);



// Define the array of leds
CRGB leds[NUM_LEDS];

void setup() { 
  Serial.begin(57600);
  Serial.println("resetting");
  pinMode(2, OUTPUT);
  LEDS.addLeds<WS2812,DATA_PIN,GRB>(leds,NUM_LEDS);
  LEDS.setBrightness(255);
}

void fadeall(uint8_t amount) { for(int i = 0; i < NUM_LEDS; i++) { leds[i].nscale8(amount); } }
void setAll(CRGB clr) { for(int i = 0; i < NUM_LEDS; i++) { leds[i] = clr; } }
CRGB getColor(uint8_t index, uint8_t brightness) {
  return ColorFromPalette(palettes[G_COLOR_PALETTE_INDEX], index, brightness, LINEARBLEND);
}

// ---------ANIMATIONS BLOCK -----------------
void segment(uint16_t segLen, uint16_t segStart, uint8_t segNum) {
  for (uint16_t i = 0; i < segLen; i++) {
    leds[(i + segStart) % NUM_LEDS] = getColor(segNum * 40 + G_ANIMATION_FRAME, 255); 
  }
}
void rainbow(uint16_t segLen, uint16_t segStart, uint8_t segNum) {
  for (uint16_t i = 0; i < segLen; i++) {
    leds[(i + segStart) % NUM_LEDS] = getColor(map(i, 0, segLen, 0, 255), 255); 
  } 
  
}
void streamer(uint16_t segLen, uint16_t segStart, uint8_t segNum) {
  uint16_t midpoint = segLen / 2;
  uint8_t width = constrain(G_SPIN_AMOUNT, 1, 10);
  for(int i = 0; i < width; i++) {
    leds[(midpoint + segStart + i) % NUM_LEDS] = getColor((midpoint + segStart + (60 * segNum)), 255);
  }
  fadeall(253);
}
void colorgrow(uint16_t segLen, uint16_t segStart, uint8_t segNum) {
  uint16_t midpoint = segLen / 2;
  for(int i = 0; i < segLen; i++) {
    uint8_t clr = map(abs(midpoint - i), 0, midpoint, 0, 255);
    leds[(i + segStart) % NUM_LEDS] = getColor(clr + G_ANIMATION_FRAME, 255);
  }
}
Animations animations = {segment, rainbow,streamer,colorgrow};
// ---------END ANIMATIONS BLOCK -----------------

void loop() { 
  EVERY_N_MILLISECONDS(30){
    uint8_t extraLights = NUM_LEDS % G_RING_SEGMENTS;
    for(uint8_t seg = 0; seg < G_RING_SEGMENTS; seg++) {
      uint16_t segLen = NUM_LEDS / G_RING_SEGMENTS;
      
      uint16_t segStart = G_START_OFFSET + (segLen * seg);
      if(extraLights > seg) {
        segLen++;
        segStart += (extraLights - (extraLights - seg));
      } else {
        segStart += extraLights;
      }
      animations[G_ANIMATION_INDEX](segLen, segStart, seg);
    }
    FastLED.show();  
  }
  EVERY_N_MILLISECONDS(50) { 
    G_ANIMATION_FRAME += G_ANIMATION_SPEED; 
    if(G_SPIN_CLOCKWISE) {
      G_START_OFFSET += G_SPIN_AMOUNT;
    } else {
      G_START_OFFSET -= G_SPIN_AMOUNT;
    }
  }
  if(G_SERIAL_MODE == 0) {
    digitalWrite(2, LOW);
  } else {
    digitalWrite(2, HIGH);
  }
  handleSerial();
}



void handleSerial() {
  while (Serial.available() > 0) {
    int val = fromSerial();
    switch (G_SERIAL_MODE) {
      case 0: // SET MODE
        G_SERIAL_MODE = val;
        break;
      case 1: // SEGMENTS
        if(val < 1) {
          val = 1;
        }
        G_RING_SEGMENTS = val;
        G_SERIAL_MODE = 0;
        break;
      case 2: // SPIN AMOUNT
        G_SPIN_AMOUNT = val;
        G_SERIAL_MODE = 0;
        break;
      case 3: // SPIN DIRECTION 1 = clockwise 
        G_SPIN_CLOCKWISE = val == 1;
        G_SERIAL_MODE = 0;
        break;
      case 4: // ANIMATION
        if(val >= sizeof(animations)/4) {
          val = 0;
        }
        G_ANIMATION_INDEX = val;
        G_SERIAL_MODE = 0;
        break;
      case 5: // ANIMATION SPEED
        if (val < 0) {
          val = 0;
        }
        G_ANIMATION_SPEED = val;
        G_SERIAL_MODE = 0;
        break;
      case 6: // ADVANCE FRAME
        G_ANIMATION_FRAME += val;
        G_SERIAL_MODE = 0;
        break;
      case 7: // COLOR PALETTE
        if(sizeof(palettes)/sizeof(CRGBPalette16) < val || 0 > val) {
          val = GetRandomPaletteIndex();
        }
        G_COLOR_PALETTE_INDEX = val;
        G_SERIAL_MODE = 0;
        break;
      default: 
        G_SERIAL_MODE = 0;
        break;
    }
  }
}
int fromSerial() {
  String inString;
  while (Serial.available() > 0) {
    int inChar = Serial.read();
    if (isDigit(inChar)) {
      inString += (char)inChar;
    }
    
    if (inChar == '\n') {
      Serial.print("Value:");
      Serial.println(inString.toInt());
      Serial.print("String: ");
      Serial.println(inString); 
      return inString.toInt();
    }
  }
  return 0;
}
