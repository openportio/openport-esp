#ifndef ARDUINO_WEBSOCKETS_OPENPORT_WIFI_SERVER_H
#define ARDUINO_WEBSOCKETS_OPENPORT_WIFI_SERVER_H
#include "OpenportWiFiClient.h"

class OpenportWiFiServer : public WiFiServer {
public:
    OpenportWiFiServer(OpenportClient *openport);

    OpenportWiFiClient available(uint8_t *status = NULL);

    bool hasClient();

//    void begin() override;
//
//    void begin(uint16_t port);

    void begin(uint16_t port, uint8_t backlog);

//    void setNoDelay(bool nodelay);
//
//    bool getNoDelay();

//    size_t write(uint8_t b) override;

//    virtual size_t write(const uint8_t *buf, size_t size);

    uint8_t status();

    uint16_t port() const;

    void close();

    void stop();

    std::deque<OpenportWiFiClient *> * clients();

    void removeClient(OpenportWiFiClient *client);

private:
    OpenportClient *_openport;
    std::deque<OpenportWiFiClient*> _clients;
    std::deque<OpenportWiFiClient*> _newClients;
    int _findClient(IPAddress clientIp, uint16_t clientPort);
    void _processMessages();
};


#endif //ARDUINO_WEBSOCKETS_OPENPORT_WIFI_SERVER_H
