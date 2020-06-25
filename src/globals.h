#include "config.h"
#define FASTLED_INTERNAL // disable the FastLED pragma messages
#include <FastLED.h>
#include "FastLED_RGBW.h"

#define DEBUG Serial

//1,65m => 99 LEDs
#define NUM_LEDS    99      
#define LED_PIN     12
#define INIT_COLOR  CRGBW(255, 100, 0, 255)
#define INIT_STATE  true
#define INIT_BRIGHTNESS  255
extern CRGBW leds[];
extern CRGB *ledsRGB;
#define UPDATES_PER_SECOND 60

#define ANIMATION_USE_FASTLED

#define ARTNET_TIMEOUT 5000 // start idle animation after 5s of Artnet missing

#define BUILTIN_LED 2

#define BROKER_IP "mse-pi.makerspace.experimenta.science"

extern bool otaRunning;