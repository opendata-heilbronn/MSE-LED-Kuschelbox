#include "ledAnimation.h"

uint16_t animationStepIdx = 0;
uint8_t curAnim = 0;
uint16_t animNumLeds = 0;
uint32_t (*_hsv2rgb)(uint8_t hue, uint8_t sat, uint8_t val);
void (*_setLed)(uint16_t index, uint32_t rgb);
uint32_t animBaseColor = 0xFFFFFF;
void _fillLeds(uint32_t rgb);

uint32_t scaleRGBW(uint32_t rgbw, uint8_t scale) {
    uint32_t bd = rgbw & 0x00FF00FF;
    uint32_t ac = (rgbw >> 8) & 0x00FF00FF;
    bd *= scale;
    ac *= scale;
    uint32_t out = ((bd >> 8) & 0x00FF00FF) | (ac & 0xFF00FF00);
    return out;
}

/////////////////////////////////////////////
///////    Define animations here    ////////
/////////////////////////////////////////////

void rainbowAnimation() {
    if(animationStepIdx >= 256) {
        animationStepIdx = 0;
    }
    for(uint16_t i = 0; i < animNumLeds; i++) {
        uint16_t ledOffset = (255UL * i) / (animNumLeds - 1);
        uint8_t hue = (animationStepIdx + ledOffset) % 256;
        _setLed(i, _hsv2rgb(hue, 255, 255));
    }
}

void rainbowFade() {
    if(animationStepIdx >= 256) {
        animationStepIdx = 0;
    }
    _fillLeds(_hsv2rgb(animationStepIdx, 255, 255));
}

void blink() {
    if(animationStepIdx >= 2) {
        animationStepIdx = 0;
    }
    _fillLeds((animationStepIdx) ? 0 : 0xFFFFFFFF);
}

void fadeUpDown() {
    if(animationStepIdx >= 512) {
        animationStepIdx = 0;
    }
    uint8_t scale = animationStepIdx < 256 ? animationStepIdx : 511 - animationStepIdx; //count up to 255, then back to zero
    _fillLeds(scaleRGBW(animBaseColor, scale));
}

anim_t anims[] = {
    {"rainbowScroll", rainbowAnimation},
    {"rainbowFade", rainbowFade},
    {"blink", blink},
    {"fadeUpDown", fadeUpDown}

};





/////////////////////////////////////////
///////    Internal functions    ////////
/////////////////////////////////////////

int numAnims = sizeof(anims) / sizeof(anims[0]);

bool setAnimation(uint8_t animId) {
    if(animId >= 0 && animId < numAnims) {
        curAnim = animId;
        return true;
    }
    return false;
}

bool setAnimation(const char* animName) {
    return setAnimation(animationNameToId(animName));
}

int animationNameToId(const char* animName) {
    int result = -1;
    for(uint8_t i = 0; i < numAnims; i++) {
        if(strcmp(anims[i].animName, animName) == 0) {
            result = i;
            break;
        }
    }
    return result;
}

void stepAnimation() {
    anims[curAnim].funcPtr();
    animationStepIdx++;
}

const char* getCurAnimationName() {
    return anims[curAnim].animName;
}

void setAnimationBaseColor(uint32_t rgb) {
    animBaseColor = rgb;
}

anim_t *getAnimations() {
    return anims;
}

uint8_t getAnimationCount() {
    return numAnims;
}

#ifdef ANIMATION_USE_FASTLED
uint32_t FastLED_hsv2rgb(uint8_t hue, uint8_t sat, uint8_t val) {
    CRGB crgb = CHSV(hue, sat, val);
    uint32_t rgb = crgb.r << 16 | crgb.g << 8 | crgb.b;
    return rgb;
}

void FastLED_setLed(uint16_t idx, uint32_t rgb) {
    leds[idx] = CRGBW(rgb);
}

void initAnimation(uint16_t numLeds) {
    animNumLeds = numLeds;
    _hsv2rgb = FastLED_hsv2rgb;
    _setLed = FastLED_setLed;
}
#else
void initAnimation(uint16_t numLeds, void (*setLed)(uint16_t index, uint32_t rgb), uint32_t (*hsv2rgb)(uint8_t hue, uint8_t sat, uint8_t val)) {
    _hsv2rgb = hsv2rgb;
    _setLed = setLed;
}
#endif

void _fillLeds(uint32_t rgb) {
    for(uint16_t i = 0; i < animNumLeds; i++) {
        _setLed(i, rgb);
    }
}