#include <ArduinoWebsockets.h>
#include "OpenportWiFiServer.h"



class OpenportEsp8266TCPServer : public websockets::network::Esp8266TcpServer {
public:
    explicit OpenportEsp8266TCPServer(OpenportWiFiServer server) : server(server) {}

protected:
    OpenportWiFiServer server;
    //TODO: This needs a full implementation, because in the real class, this field is not protected, but private.
};