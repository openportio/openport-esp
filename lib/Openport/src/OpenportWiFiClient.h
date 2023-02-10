#ifndef ARDUINO_WEBSOCKETS_OPENPORT_WIFI_CLIENT_H
#define ARDUINO_WEBSOCKETS_OPENPORT_WIFI_CLIENT_H

#include "WiFiServer.h"
#include "WiFiClient.h"
#include "OpenportClient.h"

#define DEBUG_SERIAL Serial

class OpenportWiFiClient : public WiFiClient {
public:
    OpenportWiFiClient();
    OpenportWiFiClient(OpenportClient* openportClient, OpenportMessage* msg);
    size_t write(const uint8_t *buf, size_t size) override;
    int available() override;
    int read(uint8_t *buf, size_t size) override;
    void stop() override;
    uint8_t connected() override;
    operator bool() override;
    IPAddress remoteIP();
    uint16_t remotePort();
    void addMessage(OpenportMessage* msg);
private:
    std::deque<OpenportMessage*> _messages;
    uint16_t _currentPosition = 0;
    uint16_t _remotePort;
    IPAddress _remoteIP;
    OpenportClient* _openportClient;
};

#endif //ARDUINO_WEBSOCKETS_OPENPORT_WIFI_CLIENT_H