#include "mqttLogic.h"
#include "globals.h"
#include "leds.h"
#include "ledAnimation.h"
#include <ArduinoJson.h>
#include <MQTT.h>
#include <WiFiClient.h>

#define DISCOVERY_PREFIX "homeassistant"

WiFiClient net;
MQTTClient mqtt(512);
String baseTopic;

bool ignoreRetainedMessage = true;


void sendAutoDiscovery() {
    DynamicJsonDocument payload(500);
    
    payload["~"] = baseTopic;
    payload["ret"] = false; // enable retain of command messages on MQTT broker
    payload["name"] = MY_HOSTNAME;
    payload["unique_id"] = MY_HOSTNAME;
    payload["cmd_t"] = "~/set";
    payload["stat_t"] = "~/state";
    payload["schema"] = "json";
    payload["brightness"] = true;
    payload["rgb"] = true;

    if(numEffects > 0) {
        payload["effect"] = true;
        JsonArray fx = payload.createNestedArray("effect_list");
        for(uint8_t i = 0; i < numEffects; i++) {
            fx.add(effectNames[i]);
        }
        anim_t *anims = getAnimations();
        for(uint8_t i = 0; i < getAnimationCount(); i++) {
            fx.add(anims[i].animName);
        }
    }

    payload["white_value"] = true; // enable for RGBW
    payload["transition"] = true;

    // DEBUG.println("Payload:");
    // serializeJsonPretty(payload, DEBUG);

    String buf;
    serializeJson(payload, buf);
    mqtt.publish(String(baseTopic + "/config"), buf, true, 0); // publish config with retained flag 

}


void sendLedState() {
    ledState_t state = getLedState();
    DynamicJsonDocument payload(500);
    payload["state"] = state.state ? "ON" : "OFF";
    payload["brightness"] = state.brightness;
    payload["effect"] = getCurLedEffectName();
    JsonObject c = payload.createNestedObject("color");
    c["r"] = (state.color >> 16) & 0xFF;
    c["g"] = (state.color >> 8) & 0xFF;
    c["b"] = (state.color >> 0) & 0xFF;
    payload["white_value"] = (state.color >> 24) & 0xFF;
    payload["transition_time"] = (float)state.transitionTime / 1000;

    // DEBUG.println(state.color, 16);
    // DEBUG.println("State:");
    // serializeJsonPretty(payload, DEBUG);

    String buf;
    serializeJson(payload, buf);
    mqtt.publish(String(baseTopic + "/state"), buf, true, 0); // retain = true
}


void parseHAssCmd(String &payload) {
    // Update: should not be needed anymore, due to retain being set to false in the discovery message
    // TODO: remove if first real command gets ignored
    // ignore first message that controller receives, so it will init correctly
    // workaround TODO: find neater solution
    if(ignoreRetainedMessage) {
        ignoreRetainedMessage = false;
        return;
    }

    DynamicJsonDocument doc(500);
    DeserializationError err = deserializeJson(doc, payload);
    if(err) {
        DEBUG.println("MQTT JSON payload parsing error");
        DEBUG.println(err.c_str());
        return;
    }

    setLedState((doc["state"] == "ON"));
    
    JsonVariant br = doc["brightness"];
    if(!br.isNull()) {
        setLedBrightness(br);
    }

    JsonVariant fx = doc["effect"];
    if(!fx.isNull()) {
        setLedEffect(fx);
    }

    JsonObject c = doc["color"];
    if(!c.isNull()) {
        uint8_t r = c["r"];
        uint8_t g = c["g"];
        uint8_t b = c["b"];

        uint32_t color =  (r << 16) | (g << 8) | b; 
        
        setLedColor(color);
    }

    JsonVariant whiteVal = doc["white_value"];
    if(!whiteVal.isNull()) {
        setLedWhiteValue(whiteVal);
    }

    JsonVariant transitionTime = doc["transition"];
    if(!transitionTime.isNull()) {
        float trans = transitionTime;
        setLedTransitionTime(trans * 1000);
    }

    sendLedState();
}


void mqttMessageHandler(String &topic, String &payload) {
    DEBUG.println("incoming: " + topic + " - " + payload);
    if(topic == baseTopic + "/set") {
        parseHAssCmd(payload);
    }
}


void connectMqtt() {
    DEBUG.print("Connecting to broker");
    
    uint32_t initStart = millis();
    mqtt.setOptions(5, false, 1000);
    mqtt.setWill(String(baseTopic + "/state").c_str(), "{\"state\": \"OFF\"}");
    while(!mqtt.connect(MY_HOSTNAME, MQTT_USER, MQTT_PASS)) {
        DEBUG.print(".");
        delay(250);
        if(millis() - initStart > 10000) {
            DEBUG.println("\nTimeout");
            return;
        }
    }
    DEBUG.println(" done.");
    mqtt.subscribe(baseTopic + "/set");

    sendLedState();
}


void initMqtt() {
    mqtt.begin(BROKER_IP, net);
    mqtt.onMessage(mqttMessageHandler);

    baseTopic = DISCOVERY_PREFIX "/light/" MY_HOSTNAME;

    connectMqtt();
    sendAutoDiscovery();
}


void loopMqtt() {
    if(!mqtt.connected()) {
        connectMqtt();
    }
    mqtt.loop();
}