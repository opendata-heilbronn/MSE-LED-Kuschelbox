#include "leds.h"
#define ANIMATION_USE_FASTLED
#include "ledAnimation.h"

const char* effectNames[] = {
    "static",
    "artnet",
};
int numEffects = sizeof(effectNames) / sizeof(effectNames[0]);

CRGBW leds[NUM_LEDS];
CRGB *ledsRGB = (CRGB *) &leds[0];


fx_t effectStringToFx(String effectName) {
    fx_t result = fx_undefined;
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
    fx_static,
    5000
};


void fill_solid( struct CRGBW * leds, int numToFill, CRGBW color) {
    for (int i = 0; i < numToFill; i++) {
        leds[i] = color;
    }
}

void fill_solid( struct CRGBW * leds, int numToFill, CRGB color) {
    fill_solid(leds, numToFill, color);
}

void fill_solid( struct CRGBW * leds, int numToFill, uint32_t color) {
    CRGBW rgbw;
    rgbw.r = (color >> 16) & 0xFF;
    rgbw.g = (color >>  8) & 0xFF;
    rgbw.b = (color >>  0) & 0xFF;
    rgbw.w = (color >> 24) & 0xFF;
    fill_solid(leds, numToFill, rgbw);
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
    setAnimationBaseColor(color);
}

void setLedWhiteValue(uint8_t whiteValue) {
    curLedState.color &= 0x00FFFFFF; // keep color value
    curLedState.color |= whiteValue << 24;
}

void setLedAnimationTime(uint32_t transition) {
    curLedState.animationTime = transition;
}

void setLedEffect(String effect) {
    fx_t fx = effectStringToFx(effect);
    if(fx != fx_undefined) {
        curLedState.effect = fx;
    }
    else {
        uint8_t anim = animationNameToId(effect.c_str());
        if(anim > -1) {
            setAnimation(anim);
            curLedState.effect = fx_animation;
        }
    }
}

const char* getCurLedEffectName() {
    if(curLedState.effect != fx_animation) {
        return effectNames[curLedState.effect];
    }
    else {
        return getCurAnimationName();
    }
}

void setLedState(ledState_t state) {
    curLedState.state = state.state;
    curLedState.brightness = state.brightness;
    curLedState.color = state.color;
    curLedState.effect = state.effect;
    curLedState.animationTime = state.animationTime;

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

void initLeds() {
    //leds.begin();
    // idleAnimationSetup();
    //applyLedState();
    FastLED.addLeds<WS2812B, LED_PIN, RGB>(ledsRGB, getRGBWsize(NUM_LEDS));
    initAnimation(NUM_LEDS);
}

uint32_t lastLedUpdate = 0, lastAnimationStep = 0;

void loopLeds() {
    if(millis() - lastLedUpdate > (1000 / UPDATES_PER_SECOND)) {
        lastLedUpdate = millis();

        if(!curLedState.state) {
            fill_solid(leds, NUM_LEDS, CRGB::Black);
            FastLED.show();
        }
        else {
            switch(curLedState.effect) {
                case fx_static:
                    fill_solid(leds, NUM_LEDS, curLedState.color);
                    FastLED.show();
                    break;
                case fx_artnet:
                    artnetLoop();
                    break;
                case fx_animation:
                    if(millis() - lastAnimationStep > (curLedState.animationTime / 256)) {
                        // TODO: not sure if I want to skip animation steps, or freeze when timing is not met
                        lastAnimationStep = millis();
                        stepAnimation();
                        FastLED.show();
                    }
                    break;
                default:
                    break;
            }
        }
    }
}