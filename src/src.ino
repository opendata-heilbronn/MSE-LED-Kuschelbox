#include <Arduino.h>

#include "config.h"
#include "globals.h"
#include "artnet.h"
#include "ota.h"

#include <FastLED.h>
#include "FastLED_RGBW.h"

#include <WiFiMulti.h>

WiFiMulti wifi;
CRGBW leds[NUM_LEDS];
CRGB *ledsRGB = (CRGB *) &leds[0];

void fillColor(CRGBW color) {
    for(uint16_t i = 0; i < NUM_LEDS; i++) {
        leds[i] = color;
    }
}

void setup() {
    Serial.begin(115200);

    FastLED.addLeds<WS2812B, LED_PIN, RGB>(ledsRGB, getRGBWsize(NUM_LEDS));
    FastLED.setBrightness(BRIGHTNESS);
    fillColor(INIT_COLOR);
    FastLED.show();

    wifi.addAP(WIFI_SSID, WIFI_PASS);

    Serial.println("Connecting Wifi...");
    if (wifi.run() == WL_CONNECTED) {
        WiFi.setHostname(MY_HOSTNAME);
        Serial.println("");
        Serial.println("WiFi connected");
        Serial.println("IP address: ");
        Serial.println(WiFi.localIP());
    }

    initOTA();

    artnetInit();
}

void loop() {
    wifi.run();
    loopOTA();

    if (!artnetLoop()) {
        //idleAnimationLoop();
    }

}