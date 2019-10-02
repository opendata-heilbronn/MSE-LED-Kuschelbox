#include <FastLED.h>
#include "FastLED_RGBW.h"

//1,65m => 99 LEDs
#define NUM_LEDS    99      
#define LED_PIN     12
#define BRIGHTNESS  255
#define INIT_COLOR  CRGBW(0, 0, 0, 255)
extern CRGBW leds[];
extern CRGB *ledsRGB;

#define ARTNET_TIMEOUT 5000 // start idle animation after 5s of Artnet missing

#define BUILTIN_LED 2