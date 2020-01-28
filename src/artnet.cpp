#include "artnet.h"
#include "globals.h"
#include <ArtnetWifi.h>
ArtnetWifi artnet;
const int startUniverse = 1; // CHANGE FOR YOUR SETUP most software    this is 1, some software send out artnet first universe as 0.

// Check if we got all universes
#define numberOfChannels (NUM_LEDS * 4)
const int maxUniverses = numberOfChannels / 512 + ((numberOfChannels % 512) ? 1 : 0);
bool universesReceived[maxUniverses];
bool sendFrame = 1;
int previousDataLength = 0;

byte lastSequenceNum = 0;
uint32_t lastArtnetReceived = 0;

void onDmxFrame(uint16_t universe, uint16_t length, uint8_t sequence, uint8_t *data) {
    // Serial.println(data[0]);

    sendFrame = 1;
    // set brightness of the whole strip

    // Store which universe has got in
    if ((universe - startUniverse) < maxUniverses) {
        universesReceived[universe - startUniverse] = 1;
        // Serial.println("inside store universe");
    }

    for (int i = 0; i < maxUniverses; i++) {
        // Serial.println(maxUniverses);
        if (universesReceived[i] == 0) {
            Serial.println("Broke");
            sendFrame = 0;
            break;
        }
    }

    int diff = sequence - lastSequenceNum;
    if (diff < 0 && diff > -200) {
        sendFrame = 0;
    } else {
        lastSequenceNum = sequence;
    }

    // read universe and put into the right part of the display buffer
    for (int i = 0; i < length / 4; i++) {
        // Serial.println(i);
        int led = i + (universe - startUniverse) * (previousDataLength / 4);
        if (led < NUM_LEDS) {
            leds[led] = CRGBW(data[i * 4], data[i * 4 + 1], data[i * 4 + 2], data[i * 4 + 3]);
            // setLed(led % WIDTH, led / WIDTH, CRGB(data[i * 3], data[i * 3 + 1], data[i * 3 + 2]));
        }
    }
    previousDataLength = length;

    if (sendFrame) {
        FastLED.show();
        // Serial.println(sequence);
        // Reset universeReceived to 0
        memset(universesReceived, 0, maxUniverses);
        lastArtnetReceived = millis();
    }
}

void artnetInit() {
    artnet.begin();
    // this will be called for each packet received
    artnet.setArtDmxCallback(onDmxFrame);
}

// returns false when data timout
bool artnetLoop() {
    artnet.read();
    return (millis() - lastArtnetReceived < ARTNET_TIMEOUT);
}