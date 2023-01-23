//
// Created by Jan De Bleser on 19/01/23.
//

#include "Openport.h"
#include <ArduinoJson.h>

// TODO: dynamic or based on fingerprint of CA
//https://github.com/gilmaimon/ArduinoWebsockets

// test.openport.io
const char *FINGERPRINT = "3F 51 88 B5 B3 06 42 24 1A 7E 03 E8 27 47 D5 0B 3D EC 53 70";

Openport::Openport(char *host, char *key_token, f_string on_message) {
    _host = host;
    _key_token = key_token;
    _on_message = on_message;
}

void addrFromPayload(char *address, uint8_t *payload) {
    sprintf(address, "%d.%d.%d.%d:%d", int(payload[0]), int(payload[1]), int(payload[2]), int(payload[3]),
            int((payload[4] << 8) + payload[5]));
}

void Openport::webSocketEvent(WStype_t type, uint8_t *payload, size_t length) {
    String response2;
    DynamicJsonDocument doc(2000);
    String jsonStr;

    switch (type) {
        case WStype_DISCONNECTED:
            DEBUG_SERIAL.printf("Disconnected!\n");
            break;
        case WStype_CONNECTED:
            DEBUG_SERIAL.println("Websocket is connected");

            doc["token"] = _session_token;
            doc["port"]   = _server_port;

            serializeJson(doc, jsonStr);
            DEBUG_SERIAL.println("sending ");
            DEBUG_SERIAL.println(jsonStr);
            _webSocket.sendTXT(jsonStr);
            break;
        case WStype_TEXT:
            DEBUG_SERIAL.printf("RECEIVE TXT: %s\n", payload);
            break;
        case WStype_BIN:
            DEBUG_SERIAL.printf("get binary length: %u\n", length);
            if (length > 6) {
                char address[23];
                addrFromPayload(address, payload);
                uint8 chop = payload[6];
                if (length == 7) {
                    DEBUG_SERIAL.printf("Got new connection from %s. Channel Operation: %d\n", address, chop);
                }
                if (length > 7) {
                    DEBUG_SERIAL.printf("Got new message of length %d from %s. Channel Operation: %d\n", length,
                                        address, chop);
                    String response = _on_message((char *) &(payload[7]));
                    char response_char[response.length() + 7];
                    memcpy(response_char, payload, sizeof(payload[0]) * 6);
                    response_char[6] = 2;
                    int i = 0;
                    while (i < response.length()) {
                        response_char[7 + i] = response[i];
                        i++;
                    }
                    _webSocket.sendBIN((const uint8_t *) response_char, response.length() + 6);

                    // Send close
                    response_char[6] = 3;
                    _webSocket.sendBIN((const uint8_t *) response_char, 7);
                }
            } else {
                DEBUG_SERIAL.printf("Got a short message: %d", length);
            }

            break;
    }
}


void Openport::connect() {
    HTTPClient httpClient;
    char path[128];
    sprintf(path, "https://%s/api/v1/request-port", _host);
    Serial.print("post to  ");
    Serial.println(path);
    httpClient.begin(String(path), FINGERPRINT);
    httpClient.addHeader("Content-Type", "application/x-www-form-urlencoded");
    char postData[128];
    sprintf(postData, "ws_token=%s", _key_token);
    Serial.println(postData);
    int responseCode = httpClient.POST(postData);
    if (responseCode>0) {
        Serial.print("HTTP Response code: ");
        Serial.println(responseCode);
        String payload = httpClient.getString();
        Serial.println(payload);

        DynamicJsonDocument doc(1024);
        deserializeJson(doc, payload);
        _server_port = doc["server_port"];
        _session_token = doc["session_token"].as<String>();
    }
    else {
        Serial.print("Error code: ");
        Serial.println(responseCode);
    }
    httpClient.end();
    _webSocket.beginSSL(_host, 443, "/ws");
    std::function<void(WStype_t, uint8_t *, size_t)> callback =
            std::bind(&Openport::webSocketEvent, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);

    _webSocket.onEvent(callback);
    _webSocket.setReconnectInterval(5000);
    _webSocket.enableHeartbeat(15000, 3000, 2);
    Serial.print("Connecting to attempted to ");
    Serial.println(_host);
}

unsigned long messageInterval = 5000;


void Openport::loop() {
    _webSocket.loop();
    if (_lastUpdate + messageInterval < millis()) {
        Serial.printf("is connected: %u\n", _webSocket.isConnected());
        _lastUpdate = millis();
    }
}