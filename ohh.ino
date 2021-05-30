#include "ColorPalettes.h"
#include "FastLED.h"


// server includes
#include <WiFi.h>
#include <SPIFFS.h>
#include <ESPAsyncWebServer.h>
#include <WebSocketsServer.h>
#define NUM_LEDS 284 
#define DATA_PIN 15

//web socket constants
const char *ssid = "ESP32-AP";
const char *password =  "LetMeInPlz";
const int dns_port = 53;
const int http_port = 80; 
const int ws_port = 1337;
//web socket globals
AsyncWebServer server(http_port);
WebSocketsServer webSocket = WebSocketsServer(ws_port);
char msg_buf[10];

// LED Globals 
uint8_t G_BRIGHTNESS = 25;
uint8_t G_RING_SEGMENTS = 1;
uint16_t G_START_OFFSET = 0;
uint8_t G_SPIN_AMOUNT = 0;
uint8_t G_SERIAL_MODE = 0;
bool G_SPIN_CLOCKWISE = true;
bool G_ANIMATE_FORWARD = true;
uint16_t G_ANIMATION_FRAME = 0;
uint8_t G_ANIMATION_SPEED = 1;
uint8_t G_COLOR_PALETTE_INDEX = 12;
uint8_t G_ANIMATION_INDEX = 1;
typedef void (*Animations[])(uint16_t segLeg, uint16_t segStart, uint8_t segNum);



// Define the array of leds
CRGB leds[NUM_LEDS];




//web socket event function
void onWebSocketEvent(uint8_t client_num, WStype_t type, uint8_t * payload, size_t length) {
  String strPayload;
  bool num1 = true;
  switch(type) {
    case WStype_DISCONNECTED:
      Serial.printf("[%u] Disconnected!\n", client_num);
      break;
    case WStype_CONNECTED:
      {
        IPAddress ip = webSocket.remoteIP(client_num);
        Serial.printf("[%u] Connection from ", client_num);
        Serial.println(ip.toString());
      }
      break;
     case WStype_TEXT:
      Serial.printf("[%u] Received text: %s\n", client_num, payload);
      uint8_t fp, sp;
      for(uint8_t i = 0; i < length; i++){
        if((char)payload[i] == ':') {
          fp = strPayload.toInt();
          strPayload = "";
        } else {
          strPayload += (char)payload[i];
        }
      }
      sp = strPayload.toInt();
      handleText(fp, sp);
      break;
    // For everything else: do nothing
    case WStype_BIN:
    case WStype_ERROR:
    case WStype_FRAGMENT_TEXT_START:
    case WStype_FRAGMENT_BIN_START:
    case WStype_FRAGMENT:
    case WStype_FRAGMENT_FIN:
    default: 
      Serial.printf("[%u] Unknown Type!\n", type);
      break;
  }
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

uint8_t G_GS_THRESHOLD = 0;
bool G_GS_GROW = true;
void growShrink(uint16_t segLen, uint16_t segStart, uint8_t segNum) {
  uint16_t midpoint = segLen / 2;
  CRGB clr = getColor(G_ANIMATION_FRAME, 255);
  if(G_GS_THRESHOLD >= midpoint) {
    G_GS_GROW = false;
  } else if (G_GS_THRESHOLD == 0) {
    G_GS_GROW = true;
  }
  for(int i = 0; i < segLen; i++) {
    bool light = false;
    if(i >= midpoint - G_GS_THRESHOLD && i <= midpoint + G_GS_THRESHOLD) {
      light = true;
    }
    leds[(i + segStart) % NUM_LEDS] = light ? clr : CRGB(0,0,0);
  }
}
Animations animations = {segment, rainbow,streamer,colorgrow, growShrink};
// ---------END ANIMATIONS BLOCK -----------------

void handleText(uint8_t firstParam, uint8_t secondParam){
  Serial.print(firstParam);
  Serial.print(" -- ");
  Serial.print(secondParam);
  Serial.println(" - Params");
      switch (firstParam) {
        case 0: // BRIGHTNESS
          if(secondParam == 1) {
            if (G_BRIGHTNESS > 245) {
              G_BRIGHTNESS = 255;
            } else {
              G_BRIGHTNESS += 10;
            }
          } else {
             if (G_BRIGHTNESS < 11) {
              G_BRIGHTNESS = 1;
            } else {
              G_BRIGHTNESS -= 10;
            }
          }
          break;
        case 1: // SEGMENTS
          Serial.println("--- FIRST PARAM---");
          if(secondParam == 1) {
            G_RING_SEGMENTS += 1;
          } else {
            if(G_RING_SEGMENTS > 1) {
              G_RING_SEGMENTS -= 1;
            }
          }
          break;
        case 2: // SPIN AMOUNT
         if(secondParam == 1) {
            G_SPIN_AMOUNT += 1;
          } else {
            if(G_SPIN_AMOUNT > 0) {
              G_SPIN_AMOUNT -= 1;
            }
          }

          break;
        case 3: // SPIN DIRECTION 1 = clockwise 
           if(secondParam == 1) {
            G_SPIN_CLOCKWISE = true;
          } else {
            G_SPIN_CLOCKWISE = false;
          }
          break;
        case 4: // ANIMATION
          G_ANIMATION_INDEX = secondParam;
          break;
        case 5: // ANIMATION SPEED
          if(secondParam == 1) {
            G_ANIMATION_SPEED += 1;
          } else {
            if(G_ANIMATION_SPEED > 0) {
              G_ANIMATION_SPEED -= 1;
            }
          }
          break;
        case 6: // ADVANCE FRAME
//          G_ANIMATION_FRAME += val;
//          G_SERIAL_MODE = 0;
          break;
        case 7: // COLOR PALETTE
          G_COLOR_PALETTE_INDEX = secondParam;
          break;
        default: 
//        G_SERIAL_MODE = 0;
          break;
      }
}

// Callback: send homepage
void onIndexRequest(AsyncWebServerRequest *request) {
  IPAddress remote_ip = request->client()->remoteIP();
  Serial.println("[" + remote_ip.toString() +
                  "] HTTP GET request of " + request->url());
  request->send(SPIFFS, "/index.html", "text/html");
}
// Callback: send style sheet
void onCSSRequest(AsyncWebServerRequest *request) {
  IPAddress remote_ip = request->client()->remoteIP();
  Serial.println("[" + remote_ip.toString() +
                  "] HTTP GET request of " + request->url());
  request->send(SPIFFS, "/style.css", "text/css");
}

// Callback: send 404 if requested file does not exist
void onPageNotFound(AsyncWebServerRequest *request) {
  IPAddress remote_ip = request->client()->remoteIP();
  Serial.println("[" + remote_ip.toString() +
                  "] HTTP GET request of " + request->url());
  request->send(404, "text/plain", "Not found");
}

void setup() { 
  delay(1000);
  pinMode(2, OUTPUT);
  bool ledOn = true;
  for(int i = 0; i < 10; i++){
    if(ledOn)
      digitalWrite(2, HIGH);  
    else
      digitalWrite(2, LOW);
    ledOn = !ledOn;
    delay(200);
  }
  digitalWrite(2, LOW);
 
  Serial.begin(115200);
  Serial.println("startup");
  // Make sure we can read the file system
  if( !SPIFFS.begin()){
    while(1) {
      for(int i = 0; i < 12; i++) {
//          digitalWrite(2, ledOn ? HIGH : LOW);
          ledOn = !ledOn;
          delay(50);
      }
      Serial.println("Error mounting SPIFFS");
    }
  } else {
//    digitalWrite(2, HIGH);
    delay(500);                                                                
  }

   // Start access point
  WiFi.softAP(ssid, password);

  // Print our IP address
  Serial.println();
  Serial.println("AP running");
  Serial.print("My IP address: ");
  Serial.println(WiFi.softAPIP());

  // On HTTP request for root, provide index.html file
  server.on("/", HTTP_GET, onIndexRequest);

  // On HTTP request for style sheet, provide style.css
  server.on("/style.css", HTTP_GET, onCSSRequest);

  // Handle requests for pages that do not exist
  server.onNotFound(onPageNotFound);

  // Start web server
  server.begin();

  // Start WebSocket server and assign callback
  webSocket.begin();
  webSocket.onEvent(onWebSocketEvent);
  
  
  
  
  LEDS.addLeds<WS2812,DATA_PIN,GRB>(leds,NUM_LEDS);
  LEDS.setBrightness(G_BRIGHTNESS);
}

void fadeall(uint8_t amount) { for(int i = 0; i < NUM_LEDS; i++) { leds[i].nscale8(amount); } }
void setAll(CRGB clr) { for(int i = 0; i < NUM_LEDS; i++) { leds[i] = clr; } }
CRGB getColor(uint8_t index, uint8_t brightness) {
  return ColorFromPalette(palettes[G_COLOR_PALETTE_INDEX], index, brightness, LINEARBLEND);
}



void loop() { 
  webSocket.loop();
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
    LEDS.setBrightness(G_BRIGHTNESS);
    FastLED.show();  
  }
  EVERY_N_MILLISECONDS(50) { 
    
    G_ANIMATION_FRAME += G_ANIMATION_SPEED; 
    if(G_SPIN_CLOCKWISE) {
      G_START_OFFSET += G_SPIN_AMOUNT;
    } else {
      G_START_OFFSET -= G_SPIN_AMOUNT;
    }
    if(G_GS_GROW) {
      G_GS_THRESHOLD += 1;
    }else {
      G_GS_THRESHOLD -= 1;
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
