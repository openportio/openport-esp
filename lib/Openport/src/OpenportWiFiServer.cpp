
#include "OpenportClient.h"
#include "OpenportWiFiServer.h"


OpenportWiFiServer::OpenportWiFiServer(OpenportClient *openport) : WiFiServer(0) {
    _openport = openport;
    _clients = std::deque<OpenportWiFiClient*>();
    _new_clients = std::deque<OpenportWiFiClient*>();
}

int OpenportWiFiServer::_findClient(IPAddress clientIp, uint16_t clientPort) {
    for (int i = 0; i < _clients.size(); i++) {
        auto client = _clients[i];
        if (client->remoteIP() == clientIp && client->remotePort() == clientPort) {
            return i;
        }
    }
    return -1;
}

OpenportWiFiClient OpenportWiFiServer::available2(uint8_t *status) {
//    DEBUG_SERIAL.println("OpenportWiFiServer::available");
    _openport->loop();
//    DEBUG_SERIAL.println("out of _openport->loop    ");

    OpenportMessage* message;
    std::deque<OpenportMessage *>* messages = _openport->getMessages();
    while(!messages->empty()) {
        message = messages->front();
        messages->pop_front();
        DEBUG_SERIAL.println("Got a message in the wifi server");
        DEBUG_SERIAL.println(message->getType());
        if (message->getType() == ChOpNew) {
            auto newClient = new OpenportWiFiClient(_openport, message);
            _clients.push_back(newClient);
            // TODO: remove clients when they are closed
            DEBUG_SERIAL.print("Nr of clients: ");
            DEBUG_SERIAL.println(_clients.size());
            _new_clients.push_back(newClient);
        } else if (message->getType() == ChOpCont) {
            auto oldClientIndex = _findClient(message->getClientIp(), message->getClientPort());
            if (oldClientIndex >=0) {
                auto oldClient = _clients[oldClientIndex];
                oldClient->addMessage(message);
            }
        } else if (message->getType() == ChOpClose) {
            auto oldClientIndex = _findClient(message->getClientIp(), message->getClientPort());
            if (oldClientIndex >=0) {
                auto oldClient = _clients[oldClientIndex];
                oldClient->stop(0);
                _clients.erase(_clients.begin() + oldClientIndex);
                delete oldClient;
            }
        } else {
            DEBUG_SERIAL.print("Unknown message type:");
            DEBUG_SERIAL.println(message->getType());
        }
    }
    DEBUG_SERIAL.println("out while loop ");

    if (! _new_clients.empty()) {
        auto client = _new_clients.front();
        _new_clients.pop_front();
        DEBUG_SERIAL.println("returning client");
        return *client;
    }
//    DEBUG_SERIAL.println("creating client");

    auto client = OpenportWiFiClient(nullptr, nullptr);
//    DEBUG_SERIAL.println("out while loop 2");
    return client;
}

//void OpenportWiFiServer::stop() {
//    // Not implemented
//}

bool OpenportWiFiServer::hasClient() {
    return !_new_clients.empty();
}
//
//void OpenportWiFiServer::begin(uint16_t port, uint8_t backlog) {
//    // Not implemented
//}

//void OpenportWiFiServer::setNoDelay(bool nodelay) {
//    // Not implemented
//}
//
//bool OpenportWiFiServer::getNoDelay() {
//    // Not implemented
//    return false;
//}

size_t OpenportWiFiServer::write(uint8_t b) {
    // Not implemented
    return -1;
}

size_t OpenportWiFiServer::write(const uint8_t *buf, size_t size) {
    // Not implemented
    return -1;
}

uint8_t OpenportWiFiServer::status() {
    // Not implemented
    return -1;
}

uint16_t OpenportWiFiServer::port() const {
    // Not implemented
    return -1;
}

void OpenportWiFiServer::close() {
    // Not implemented
}

//uint8_t OpenportWiFiClient::status() {
//    return WiFiClient::status();
//}
//
//int OpenportWiFiClient::connect(IPAddress ip, uint16_t port) {
//    return WiFiClient::connect(ip, port);
//}
//
//int OpenportWiFiClient::connect(const char *host, uint16_t port) {
//    return WiFiClient::connect(host, port);
//}
//
//int OpenportWiFiClient::connect(const String &host, uint16_t port) {
//    return WiFiClient::connect(host, port);
//}

//size_t OpenportWiFiClient::write(uint8_t msg) {
//
//    return WiFiClient::write(msg);
//}

size_t OpenportWiFiClient::write(const uint8_t *buf, size_t size) {
    DEBUG_SERIAL.println("OpenportWiFiClient::write");

    const char *payload = reinterpret_cast<const char *>(buf);

    DEBUG_SERIAL.println(payload);
    OpenportMessage openportMsg = OpenportMessage(payload, size, ChOpCont, _remoteIP, _remotePort);
    _openportClient->send(&openportMsg);
    return size;
}

//size_t OpenportWiFiClient::write_P(const char *buf, size_t size) {
//    return WiFiClient::write_P(buf, size);
//}

//size_t OpenportWiFiClient::write(Stream &stream) {
//    return WiFiClient::write(stream);
//}
//
//size_t OpenportWiFiClient::write(Stream &stream, size_t unitSize) {
//    return WiFiClient::write(stream, unitSize);
//}
//
//int OpenportWiFiClient::available() {
//    return WiFiClient::available();
//}
//
//int OpenportWiFiClient::read() {
//    return WiFiClient::read();
//}

int OpenportWiFiClient::read(uint8_t *buf, size_t size) {
    OpenportMessage* msg = _messages.front();
    DEBUG_SERIAL.println("OpenportWiFiClient::read");
    DEBUG_SERIAL.printf("msg->getPayloadSize(): %d \n", msg->getPayloadSize());
    DEBUG_SERIAL.printf("_currentPosition: %d \n", _currentPosition);
    size_t bytesLeftToRead = msg->getPayloadSize() - _currentPosition;
    DEBUG_SERIAL.printf("bytesLeftToRead: %d \n", bytesLeftToRead);
    auto end = min(bytesLeftToRead, size);
    for (int i = 0; i < end; i++) {
        buf[i] = msg->getPayload()[i + _currentPosition];
    }
    if (bytesLeftToRead > size){  // still more to read
        _currentPosition = msg->getPayloadSize() - bytesLeftToRead;
    } else {
        DEBUG_SERIAL.println("OpenportWiFiClient::pop");

        _messages.pop_front();
        DEBUG_SERIAL.println("OpenportWiFiClient::delete");

        delete msg;
        _currentPosition = 0;
    }
    return end;
}
//
//int OpenportWiFiClient::peek() {
//    return WiFiClient::peek();
//}


bool OpenportWiFiClient::stop(unsigned int maxWaitMs) {
    OpenportMessage msg = OpenportMessage("", 0, ChOpClose, _remoteIP, _remotePort);
    _openportClient->send(&msg);
    return true;
}
//
//uint8_t OpenportWiFiClient::connected() {
//    return WiFiClient::connected();
//}




//
//void OpenportWiFiClient::stopAll() {
//
//}
//
//void OpenportWiFiClient::stopAllExcept(WiFiClient *c) {
//
//}
//
//bool OpenportWiFiClient::getDefaultNoDelay() {
//    return false;
//}
//
//
//void OpenportWiFiClient::setDefaultSync(bool sync) {
//
//}
//
//bool OpenportWiFiClient::getDefaultSync() {
//    return false;
//}
//
//
OpenportWiFiClient::operator bool() {
    return _openportClient != nullptr;
}


void OpenportWiFiClient::addMessage(OpenportMessage *msg) {
    _messages.push_back(msg);
}

OpenportWiFiClient::OpenportWiFiClient(OpenportClient* openportClient, OpenportMessage* msg) {
    _messages = std::deque<OpenportMessage*>();
    if (msg != nullptr) {
        _messages.push_back(msg);
        _remoteIP = IPAddress(msg->getClientIp());
        _remotePort = msg->getClientPort();
    }
    _openportClient = openportClient;
    _currentPosition = 0;
}

uint8_t OpenportWiFiClient::connected() {
    return true; // todo: finetune
}

int OpenportWiFiClient::available() {
    return !_messages.empty();
}
