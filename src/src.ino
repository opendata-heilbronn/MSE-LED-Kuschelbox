#include <Arduino.h>

#include "config.h"
#include "globals.h"
#include "artnet.h"
#include "ota.h"
#include "leds.h"
#include "mqttLogic.h"

#include <WiFiMulti.h>

WiFiMulti wifi;

void setup() {
    Serial.begin(115200);

    // initialize LEDs to default color
    initLeds();
    loopLeds();

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
    initMqtt();
    artnetInit();
}

void loop() {
    wifi.run();
    loopOTA();

    if(!otaRunning) {
        loopLeds();
        loopMqtt();
    }

    // if (!artnetLoop()) {
    //     // if no Artnet Data received for TIMEOUT ms
    //     fillColor(INIT_COLOR);
    //     FastLED.show();
    // }

}