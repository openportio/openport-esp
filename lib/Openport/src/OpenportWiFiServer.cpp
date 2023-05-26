
#include "OpenportClient.h"
#include "OpenportWiFiClient.h"
#include "OpenportWiFiServer.h"


OpenportWiFiServer::OpenportWiFiServer(OpenportClient *openport) : WiFiServer(0) {
    _openport = openport;
    _clients = std::deque<OpenportWiFiClient*>();
    _newClients = std::deque<OpenportWiFiClient*>();
}

int OpenportWiFiServer::_findClient(IPAddress clientIp, uint16_t clientPort) {
    for (uint i = 0; i < _clients.size(); i++) {
        auto client = _clients[i];

        DEBUG_SERIAL.printf("Comparing %s:%d to %s:%d\n", client->remoteIP().toString().c_str(), client->remotePort(), clientIp.toString().c_str(), clientPort);
        if (client->remoteIP() == clientIp && client->remotePort() == clientPort) {
            return i;
        }
    }
    return -1;
}

OpenportWiFiClient OpenportWiFiServer::available(uint8_t *status) {
//    DEBUG_SERIAL.println("OpenportWiFiServer::available");
    _openport->loop();
//    DEBUG_SERIAL.println("out of _openport->loop    ");
    _processMessages();

    if (! _newClients.empty()) {
        auto client = _newClients.front();
//        DEBUG_SERIAL.println("popping new client");
        _newClients.pop_front();
//        DEBUG_SERIAL.println("returning client");
        return *client;
    }
    auto client = OpenportWiFiClient(nullptr, nullptr, nullptr);
//    DEBUG_SERIAL.println("returning empty client");

    return client;
}

void OpenportWiFiServer::_processMessages() {
    DEBUG_SERIAL.println("Processing messages");
    OpenportMessage* msg;
    std::deque<OpenportMessage *>* messages = _openport->getMessages();
    while(!messages->empty()) {
        msg = messages->front();
        messages->pop_front();
        DEBUG_SERIAL.printf("Got a msg in the wifi server of size: %d\n", msg->getPayloadSize());
        DEBUG_SERIAL.printf("Got a msg with payload %s\n", msg->getPayload());
        DEBUG_SERIAL.printf("msg type: %d\n", msg->getType());
        if (msg->getType() == ChOpNew) {
            OpenportWiFiClient* newClient = new OpenportWiFiClient(_openport, msg, this);
            _clients.push_back(newClient);
            // TODO: remove clients when they are closed
            DEBUG_SERIAL.printf("Adding client %s:%d: Nr of clients: %d", newClient->remoteIP().toString().c_str(), newClient->remotePort(), _clients.size());
            assert(msg->getClientIp() == newClient->remoteIP());
            DEBUG_SERIAL.println("IPs match");
            assert(msg->getClientPort() == newClient->remotePort());
            _newClients.push_back(newClient);
            delete msg;
        } else if (msg->getType() == ChOpCont) {
            auto oldClientIndex = _findClient(msg->getClientIp(), msg->getClientPort());
            DEBUG_SERIAL.printf("finding client: Nr of clients: %d\n", _clients.size());
            if (oldClientIndex >=0) {
                auto oldClient = _clients[oldClientIndex];
                oldClient->addMessage(msg);
                DEBUG_SERIAL.printf("client found: Nr of clients: %d\n", _clients.size());
            } else{
                DEBUG_SERIAL.println("Could not find client");
                delete msg;
            }
        } else if (msg->getType() == ChOpClose) {
            DEBUG_SERIAL.print("Got a close msg");

            auto oldClientIndex = _findClient(msg->getClientIp(), msg->getClientPort());
            if (oldClientIndex >=0) {
                auto oldClient = _clients[oldClientIndex];
                oldClient->stop();
                _clients.erase(_clients.begin() + oldClientIndex);
                delete oldClient;
            }
        } else {
            DEBUG_SERIAL.print("Unknown msg type:");
            DEBUG_SERIAL.println(msg->getType());
            delete msg;
        }
    }
    DEBUG_SERIAL.println("out while loop ");
}

bool OpenportWiFiServer::hasClient() {
    _processMessages();
    DEBUG_SERIAL.printf("OpenportWiFiServer::hasClient: %d", _newClients.size());
    return !_newClients.empty();
}

std::deque<OpenportWiFiClient*>* OpenportWiFiServer::clients() {
    return &_clients;
}

void OpenportWiFiServer::removeClient(OpenportWiFiClient *client) {
    auto oldClientIndex = _findClient(client->remoteIP(), client->remotePort());
    if (oldClientIndex >=0) {
        DEBUG_SERIAL.printf("Client found at index: %d\n", oldClientIndex);
        _clients.erase(_clients.begin() + oldClientIndex);
    }
    DEBUG_SERIAL.println("Removed client");
}

