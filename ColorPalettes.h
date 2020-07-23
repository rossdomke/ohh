#ifndef ColorPalettes_h
#define ColorPalettes_h
#include "FastLED.h"


DEFINE_GRADIENT_PALETTE( bhw1_06_gp ) {
    0, 184,  1,128,
  160,   1,193,182,
  219, 153,227,190,
  255, 255,255,255};

DEFINE_GRADIENT_PALETTE(  bhw3_62_gp ) {
    0, 255,255, 45,
   43, 208, 93,  1,
  137, 224,  1,242,
  181, 159,  1, 29,
  255,  63,  4, 68};

DEFINE_GRADIENT_PALETTE( bhw3_01_gp ) {
    0, 255,146,228,
   25, 152, 43, 65,
   48,  36, 17, 22,
   72,   8, 34, 75,
   81,   1, 58,170,
  104,  42, 49,245,
  124,  15, 15, 95,
  150,   1,  8, 39,
  175,  98, 32, 35,
  204, 123,  9,  2,
  219, 220, 15,  2,
  237, 255, 93,  6,
  255, 244,244,  0};

DEFINE_GRADIENT_PALETTE( bhw4_029_gp ) {
    0,  28,  7, 75,
   43,  73, 22, 74,
   79, 177,146,197,
  122,  21, 72,137,
  165,  15,184, 75,
  255, 224,205,  4};

// Gradient palette "Abstract_3_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/ggr/tn/Abstract_3.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 52 bytes of program space.

DEFINE_GRADIENT_PALETTE( Abstract_3_gp ) {
    0,   0, 38,  1,
   12,  42, 75,  8,
  111, 255,125, 30,
  125,  55, 52, 54,
  150,   1, 12, 87,
  168,  14, 40, 71,
  203,  54, 90, 56,
  224, 130,161, 45,
  240, 249,255, 34,
  245,  84,213,114,
  249,  12,175,255,
  252,   2,213, 45,
  255,   0,255,  0};


// Gradient palette "Abstract_2_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/ggr/tn/Abstract_2.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 52 bytes of program space.

DEFINE_GRADIENT_PALETTE( Abstract_2_gp ) {
    0, 255,  0,  1,
   85, 229,  9, 50,
  145, 206, 44,245,
  157,  46, 31,138,
  169,   1, 21, 66,
  185,   1,101, 63,
  192,   1,255, 59,
  203,  45,239, 55,
  214, 234,227, 52,
  230, 244,122,130,
  242, 255, 53,255,
  252,  73, 23,255,
  255,   6,  6,255};

// Gradient palette "gr65_hult_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/hult/tn/gr65_hult.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 24 bytes of program space.

DEFINE_GRADIENT_PALETTE( gr65_hult_gp ) {
    0, 247,176,247,
   48, 255,136,255,
   89, 220, 29,226,
  160,   7, 82,178,
  216,   1,124,109,
  255,   1,124,109};

DEFINE_GRADIENT_PALETTE( magenta_blue ) {
  0, 255, 0, 125,
  75, 0, 100, 255,
  150, 255, 0 , 125,
  255, 0, 100, 255
};

DEFINE_GRADIENT_PALETTE( seafoam ){
  0, 0, 255, 255,
  75, 0, 255, 100,
  150, 0, 50, 200,
  255, 100, 255, 255
}; 
DEFINE_GRADIENT_PALETTE( sunrise ){
  0, 255, 150, 0,
  50, 255, 255, 0,
  120, 100, 0, 200,
  180, 25, 0, 200,
  255, 255, 255, 255
};
DEFINE_GRADIENT_PALETTE( red_orange ){
  0, 255, 0, 0,
  75, 255, 150, 0, 
  150, 255, 50, 0,
  255, 255, 255, 0
};

CRGBPalette16 palettes[] = {
  red_orange,
  sunrise,
  seafoam,
  magenta_blue,
  Rainbow_gp,
  gr65_hult_gp,
  Abstract_3_gp, 
  Abstract_2_gp,
  bhw1_06_gp, 
  bhw3_62_gp, 
  bhw3_01_gp, 
  bhw4_029_gp 

};
inline uint8_t GetRandomPaletteIndex() {
  return random(0, sizeof(palettes)/sizeof(CRGBPalette16));
}


#endif
