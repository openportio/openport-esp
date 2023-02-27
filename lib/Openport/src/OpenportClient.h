//
// Created by Jan De Bleser on 19/01/23.
//

#ifndef ARDUINO_WEBSOCKETS_OPENPORTCLIENT_H
#define ARDUINO_WEBSOCKETS_OPENPORTCLIENT_H

#include <WiFiClientSecure.h>

#ifdef ESP8266
#include <ESP8266HTTPClient.h>
#elif defined(ESP32)

#include <HTTPClient.h>

#endif


#include <ArduinoJson.h>
#include <deque>

#include <ArduinoWebsockets.h>

#define DEBUG_SERIAL Serial

enum ChannelOperation : std::int8_t {
    ChOpUnknown = 0x00,
    ChOpNew = 0x01,
    ChOpCont = 0x02,
    ChOpClose = 0x03,
};

const uint OPENPORT_MSG_HEADER_LENGTH = 7;

class OpenportMessage {
public:
    OpenportMessage(const char *rawData, uint16_t length);

    OpenportMessage(const char *payload, uint16_t payloadLength, uint8_t type, IPAddress clientIp, uint16_t clientPort);

    ~OpenportMessage();

    int getType();

    const char *getPayload();

    IPAddress getClientIp();

    uint16_t getClientPort();

    char *getRawData();

    uint16_t getRawDataLength();

    uint16_t getPayloadSize();

private:
    int8_t _type;
    char *_rawData;
    IPAddress _clientIp;
    uint16_t _clientPort;
    uint16_t _payloadSize;
};


class OpenportClient {
public:
    OpenportClient(const char *host, const char *key_binding_token, const char *unique_client_id, int local_port);

    OpenportClient(const char *host, const char *key_binding_token);

    OpenportClient(const char *host, const char *key_binding_token, int local_port);

    ~OpenportClient();
    bool connect();

    void loop();

    const char *getRemoteHost();

    int getRemotePort();

    void send(OpenportMessage *msg);

    std::unique_ptr<char> getRemoteAddress();

    std::deque<OpenportMessage *> *getMessages();

private:
    char *_host;
    char *_key_binding_token;
    char *_unique_client_id;
    int _local_port = -1;
    char *_ws_host = new char[1];
    char *_session_token = new char[1];
    int _server_port = -1;

    unsigned long _lastUpdate = millis();
    websockets::WebsocketsClient _webSocket;
    std::deque<OpenportMessage *> _messages;

    WiFiClientSecure _wifiClient;

    void webSocketEvent(websockets::WebsocketsEvent event, String data);

    void webSocketMessage(websockets::WebsocketsMessage message);

    bool sendHttpRequest();

    void requestWSPortForward();

    bool connectWS();

};

void setTimeUsingSNTP();

#endif //ARDUINO_WEBSOCKETS_OPENPORTCLIENT_H
