//
// Created by Jan De Bleser on 19/01/23.
//

#ifndef ARDUINO_WEBSOCKETS_OPENPORTCLIENT_H
#define ARDUINO_WEBSOCKETS_OPENPORTCLIENT_H

//#include <ArduinoWebsockets.h>
#include <WiFiClientSecure.h>
#include <ESP8266HTTPClient.h>

#include <ArduinoJson.h>
#include <deque>
#include "../../../.pio/libdeps/Debug/ArduinoWebsockets/src/tiny_websockets/client.hpp"
#include "../../../.pio/libdeps/Debug/ArduinoWebsockets/src/tiny_websockets/message.hpp"
#include "../../../.pio/libdeps/Debug/ArduinoJson/ArduinoJson.h"

#define DEBUG_SERIAL Serial

enum ChannelOperation: std::int8_t
{
    ChOpUnknown = 0x00,
    ChOpNew      = 0x01,
    ChOpCont     = 0x02,
    ChOpClose    = 0x03,
};


class OpenportMessage {
public:
    OpenportMessage(const char *rawData, uint16_t length);
    OpenportMessage(const char *payload, uint16_t payloadLength, uint8_t type, IPAddress clientIp, uint16_t clientPort);
    int getType();
    const char* getPayload();
    IPAddress getClientIp();
    uint16_t getClientPort();

    std::unique_ptr<char> getRawData();

    int getRawDataLength();

    uint16_t getPayloadSize();

private:
    int8_t _type;
    const char* _rawData;
    IPAddress _clientIp;
    uint16_t _clientPort;
    const char *_payload;
    uint16_t _payloadSize;
};


class OpenportClient {
public:
    OpenportClient(char *host, char *key_token);

    bool connect();

    void loop();

    const char *getRemoteHost();

    int getRemotePort();
    void send(OpenportMessage *msg);

    char *getRemoteAddress();
    std::deque<OpenportMessage*>* getMessages();

private:
    char *_host;
    String _ws_host;
    char *_key_token;
    String _session_token{};
    int _server_port{};
    unsigned long _lastUpdate = millis();
    websockets::WebsocketsClient _webSocket;
    std::deque< OpenportMessage* > _messages;
    void webSocketEvent(websockets::WebsocketsEvent event, String data);

    void webSocketMessage(websockets::WebsocketsMessage message);

    bool sendHttpRequest();

    void requestWSPortForward();

    bool connectWS();
};

void setTimeUsingSNTP();

#endif //ARDUINO_WEBSOCKETS_OPENPORTCLIENT_H
