//
// Created by Jan De Bleser on 19/01/23.
//

#ifndef ARDUINO_WEBSOCKETS_OPENPORT_H
#define ARDUINO_WEBSOCKETS_OPENPORT_H

#include <WebSocketsClient.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

#define DEBUG_SERIAL Serial

using f_string = String(*)(char*);

class Openport {
public:
    Openport(char *host, char *key_token, f_string on_message);
    void connect();
    void loop();
private:
    void webSocketEvent(WStype_t type, uint8_t *payload, size_t length);
    char *_host;
    char *_key_token;
    String _session_token{};
    int _server_port{};
    f_string _on_message;
    WebSocketsClient _webSocket;
    unsigned long _lastUpdate = millis();
};

#endif //ARDUINO_WEBSOCKETS_OPENPORT_H
