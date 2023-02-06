
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

OpenportWiFiClient OpenportWiFiServer::available(uint8_t *status) {
    DEBUG_SERIAL.println("OpenportWiFiServer::available");
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
                oldClient->stop();
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
    auto client = OpenportWiFiClient(nullptr, nullptr);
    return client;
}

bool OpenportWiFiServer::hasClient() {
    return !_new_clients.empty();
}

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

size_t OpenportWiFiClient::write(const uint8_t *buf, size_t size) {
    DEBUG_SERIAL.println("OpenportWiFiClient::write");

    const char *payload = reinterpret_cast<const char *>(buf);

    DEBUG_SERIAL.println(payload);
    OpenportMessage openportMsg = OpenportMessage(payload, size, ChOpCont, _remoteIP, _remotePort);
    _openportClient->send(&openportMsg);
    return size;
}

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

void OpenportWiFiClient::stop() {
    DEBUG_SERIAL.println("OpenportWiFiClient::stop");
    OpenportMessage msg = OpenportMessage("", 0, ChOpClose, _remoteIP, _remotePort);
    _openportClient->send(&msg);
    // todo: remove from lists?
    // _openportWiFiServer->removeClient(this);
}

OpenportWiFiClient::operator bool() {
    DEBUG_SERIAL.println("OpenportWiFiClient::operator bool()");
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
    DEBUG_SERIAL.println("OpenportWiFiClient::connected()");
    return true; // todo: finetune
}

int OpenportWiFiClient::available() {
    return !_messages.empty();
}
