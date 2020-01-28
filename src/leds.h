#pragma once

#include "globals.h"
#include "artnet.h"

typedef enum effect {
    fx_static           = 0,
    fx_artnet           = 1,
    fx_rainbow          = 2,
} fx_t;

typedef struct {
    bool state;
    uint8_t brightness;
    uint32_t color;
    fx_t effect;
} ledState_t;

extern const char* effectNames[];
extern int numEffects;
extern CRGBW leds[];

void fillColor(uint32_t color);
void setLedState(ledState_t state);
void setLedState(bool state);
void setLedBrightness(uint8_t brightness);
void setLedColor(uint32_t color);
void setLedWhiteValue(uint8_t whiteValue);
void setLedEffect(String effect);
ledState_t getLedState();
void setLedProgress(uint8_t percent);
void initLeds();
void loopLeds();