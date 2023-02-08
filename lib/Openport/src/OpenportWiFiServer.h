#ifndef ARDUINO_WEBSOCKETS_OPENPORT_WIFI_SERVER_H
#define ARDUINO_WEBSOCKETS_OPENPORT_WIFI_SERVER_H

#include "WiFiServer.h"
#include "WiFiClient.h"

#define DEBUG_SERIAL Serial

class OpenportWiFiClient : public WiFiClient {
public:
    explicit OpenportWiFiClient(OpenportClient* openportClient, OpenportMessage* msg);

//    uint8_t status();

//    int connect(IPAddress ip, uint16_t port) override;
//
//    int connect(const char *host, uint16_t port) override;
//
//    int connect(const String &host, uint16_t port) override;

//    size_t write(uint8_t) override;

    size_t write(const uint8_t *buf, size_t size) override;
//    size_t write(const char *buf, size_t size);

//    size_t write_P(PGM_P buf, size_t size) override;

//    size_t write(Stream &stream) ;

    // This one is deprecated, use write(Stream& instead)
//    size_t write(Stream &stream, size_t unitSize) __attribute__ ((deprecated)) ;

    int available() override;
//
//    int read() override;

    int read(uint8_t *buf, size_t size) override;

//    int peek() override;

//    size_t peekBytes(uint8_t *buffer, size_t length) override;

//    size_t peekBytes(char *buffer, size_t length) {
//        return peekBytes((uint8_t *) buffer, length);
//    }

//    void flush() override { (void) flush(0); }

//    void stop() override { (void) stop(0); }

//    bool flush(unsigned int maxWaitMs);

    void stop() override;
//
    uint8_t connected() override;
//
    operator bool() override;
//

//    IPAddress remoteIP();

//    uint16_t remotePort();

//    IPAddress localIP();

//    uint16_t localPort();

//    static void setLocalPortStart(uint16_t port) { _localPort = port; }

//    size_t availableForWrite();

    friend class OpenportWifiServer;

//    using Print::write;

//    static void stopAll();
//
//    static void stopAllExcept(WiFiClient *c);

//    void keepAlive(uint16_t idle_sec = TCP_DEFAULT_KEEPALIVE_IDLE_SEC,
//                   uint16_t intv_sec = TCP_DEFAULT_KEEPALIVE_INTERVAL_SEC, uint8_t count = TCP_DEFAULT_KEEPALIVE_COUNT);
//
//    bool isKeepAliveEnabled() const;
//
//    uint16_t getKeepAliveIdle() const;
//
//    uint16_t getKeepAliveInterval() const;
//
//    uint8_t getKeepAliveCount() const;
//
//    __attribute__((unused)) void disableKeepAlive() { keepAlive(0, 0, 0); }

    // default NoDelay=False (Nagle=True=!NoDelay)
    // Nagle is for shortly delaying outgoing data, to send less/bigger packets
    // Nagle should be disabled for telnet-like/interactive streams
    // Nagle is meaningless/ignored when Sync=true
//    static void setDefaultNoDelay(bool noDelay);

//    static bool getDefaultNoDelay();

//    bool getNoDelay() const;
//
//    void setNoDelay(bool nodelay);

    // default Sync=false
    // When sync is true, all writes are automatically flushed.
    // This is slower but also does not allocate
    // temporary memory for sending data
//    static void setDefaultSync(bool sync);
//
//    static bool getDefaultSync();

//    bool getSync() const;
//
//    void setSync(bool sync);

    void addMessage(OpenportMessage* msg);
private:
    std::deque<OpenportMessage*> _messages;
    uint16_t _currentPosition = 0;

    uint16_t _remotePort;
    IPAddress _remoteIP;

    OpenportClient* _openportClient;

//    size_t write(Stream &stream);
};


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

private:
    OpenportClient *_openport;
    std::deque<OpenportWiFiClient*> _clients;
    std::deque<OpenportWiFiClient*> _new_clients;
    int _findClient(IPAddress clientIp, uint16_t clientPort);
    void _processMessages();
};


#endif //ARDUINO_WEBSOCKETS_OPENPORT_WIFI_SERVER_H
