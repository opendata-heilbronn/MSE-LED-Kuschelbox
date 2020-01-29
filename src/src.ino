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
    DEBUG.begin(115200);

    // initialize LEDs to default color
    initLeds();
    loopLeds();

    wifi.addAP(WIFI_SSID, WIFI_PASS);

    DEBUG.println("Connecting Wifi...");
    if (wifi.run() == WL_CONNECTED) {
        WiFi.setHostname(MY_HOSTNAME);
        DEBUG.println("");
        DEBUG.println("WiFi connected");
        DEBUG.println("IP address: ");
        DEBUG.println(WiFi.localIP());
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