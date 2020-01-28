#include "leds.h"

const char* effectNames[] = {
    "static",
    "artnet",
    "rainbow",
};
int numEffects = sizeof(effectNames) / sizeof(effectNames[0]);

CRGBW leds[NUM_LEDS];
CRGB *ledsRGB = (CRGB *) &leds[0];


fx_t effectStringToFx(String effectName) {
    fx_t result = (fx_t)-1;
    for(int i = 0; i < numEffects; i++) {
        if(effectName == String(effectNames[i])) {
            result = (fx_t)i;
            break;
        }
    }
    return result;
}



ledState_t curLedState = {
    INIT_STATE,
    INIT_BRIGHTNESS,
    INIT_COLOR,
    fx_static
};




/*void applyLedState() {
    FastLED.setBrightness(curLedState.brightness);
    if(curLedState.effect == "static") {
        fill_solid(leds, NUM_LEDS, CRGB(curLedState.color));
        FastLED.show();
    }
}*/

void fill_solid( struct CRGBW * leds, int numToFill, CRGBW color) {
    for (int i = 0; i < numToFill; i++) {
        leds[i] = color;
    }
}

void fill_solid( struct CRGBW * leds, int numToFill, CRGB color) {
    fill_solid(leds, numToFill, color);
}

void fill_solid( struct CRGBW * leds, int numToFill, uint32_t color) {
    CRGBW col;
    col.raw32 = color;
    fill_solid(leds, numToFill, col);
}

void fillColor(uint32_t color) {
    fill_solid(leds, NUM_LEDS, color);
}

void setLedState(bool state) {
    curLedState.state = state;
}

void setLedBrightness(uint8_t brightness) {
    curLedState.brightness = brightness;
    FastLED.setBrightness(brightness);
}

void setLedColor(uint32_t color) {
    curLedState.color &= 0xFF000000; // keep white value
    curLedState.color |= color & 0x00FFFFFF;
}

void setLedWhiteValue(uint8_t whiteValue) {
    curLedState.color &= 0x00FFFFFF; // keep color value
    curLedState.color |= whiteValue << 24;
}

void setLedEffect(String effect) {
    fx_t a = effectStringToFx(effect);
    if(a > -1) {
        curLedState.effect = a;
    }
}

void setLedState(ledState_t state) {
    curLedState.state = state.state;
    curLedState.brightness = state.brightness;
    curLedState.color = state.color;
    curLedState.effect = state.effect;

    //applyLedState();
}

ledState_t getLedState() {
    return curLedState;
}

uint8_t lastPercent = 0;

void setLedProgress(uint8_t percent) {
    if(lastPercent != percent) {
        lastPercent = percent;
        fill_solid(leds, NUM_LEDS, CRGB::Black);
        fill_solid(leds, (NUM_LEDS * percent) / 100, CRGB::White);
        FastLED.show();
    }
}

void rainbowAnim() {
    uint32_t now = millis();
    for(uint16_t i = 0; i < NUM_LEDS; i++) {
        uint16_t timeOffset = (255UL * (now % 5000)) / 5000;
        uint16_t ledOffset = (255UL * i) / (NUM_LEDS - 1);
        uint8_t hue = (timeOffset + ledOffset) % 256;
        leds[i] = CHSV(hue, 255, 255);
    }
    FastLED.show();
}

void initLeds() {
    //leds.begin();
    // idleAnimationSetup();
    //applyLedState();
    FastLED.addLeds<WS2812B, LED_PIN, RGB>(ledsRGB, getRGBWsize(NUM_LEDS));
}

uint32_t lastLedUpdate = 0;
uint16_t animationStep = 0;

void loopLeds() {
    if(millis() - lastLedUpdate > (1000 / UPDATES_PER_SECOND)) {
        lastLedUpdate = millis();
        // simple rainbow fade animation

        if(!curLedState.state) {
            fill_solid(leds, NUM_LEDS, CRGB::Black);
            FastLED.show();
        }
        else {
            //Serial.print(curLedState.effect);
            switch(curLedState.effect) {
                case fx_static:
                    //Serial.println("static");
                    fill_solid(leds, NUM_LEDS, curLedState.color);
                    FastLED.show();
                    break;
                case fx_artnet:
                    //Serial.println("artnet");
                    artnetLoop();
                    break;
                case fx_rainbow:
                    rainbowAnim();
                    break;
                default:
                    break;
            }
        }
    }
}