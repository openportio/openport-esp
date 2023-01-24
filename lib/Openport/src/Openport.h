//
// Created by Jan De Bleser on 19/01/23.
//

#ifndef ARDUINO_WEBSOCKETS_OPENPORT_H
#define ARDUINO_WEBSOCKETS_OPENPORT_H

//#include <ArduinoWebsockets.h>
#include <WiFiClientSecure.h>
#include <ESP8266HTTPClient.h>

#include <ArduinoJson.h>
#include "../../../.pio/libdeps/Debug/ArduinoWebsockets/src/tiny_websockets/client.hpp"
#include "../../../.pio/libdeps/Debug/ArduinoWebsockets/src/tiny_websockets/message.hpp"
#include "../../../.pio/libdeps/Debug/ArduinoJson/ArduinoJson.h"

#define DEBUG_SERIAL Serial

using f_string = String(*)(char*);

class Openport {
public:
    Openport(char *host, char *key_token, f_string on_message);
    bool connect();
    void loop();
private:
    char *_host;
    String _ws_host;
    char *_key_token;
    String _session_token{};
    int _server_port{};
    f_string _on_message;
    websockets::WebsocketsClient _webSocket;
    unsigned long _lastUpdate = millis();

    void webSocketEvent(websockets::WebsocketsEvent event, String data);
    void webSocketMessage(websockets::WebsocketsMessage message);
    bool sendHttpRequest();
    void requestWSPortForward();
    bool connectWS();
};

void setTimeUsingSNTP();
#endif //ARDUINO_WEBSOCKETS_OPENPORT_H
