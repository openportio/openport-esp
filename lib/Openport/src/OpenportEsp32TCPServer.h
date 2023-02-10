#include <ArduinoWebsockets.h>
#include "OpenportWiFiServer.h"



class OpenportEsp32TCPServer : public websockets::network::Esp32TcpServer {
public:
    explicit OpenportEsp32TCPServer(OpenportWiFiServer server) : server(server) {}

protected:
    OpenportWiFiServer server;
    //TODO: This needs a full implementation, because in the real class, this field is not protected, but private.
};