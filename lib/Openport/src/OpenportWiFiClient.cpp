#include "OpenportWiFiClient.h"

OpenportWiFiClient::OpenportWiFiClient() {
    _openportClient = nullptr;
}

OpenportWiFiClient::OpenportWiFiClient(OpenportClient* openportClient, OpenportMessage* msg) {
//    DEBUG_SERIAL.printf("OpenportWiFiClient::OpenportWiFiClient %p \n", msg);
    if (msg != nullptr) {
//        DEBUG_SERIAL.println("msg is not null");
//        DEBUG_SERIAL.printf("msg->getPayloadSize(): %d \n", msg->getPayloadSize());
//        DEBUG_SERIAL.printf("msg->getClientIp(): %s \n", msg->getClientIp().toString().c_str());
//        DEBUG_SERIAL.printf("msg->getClientPort(): %d \n", msg->getClientPort());

//    #    _messages.push_back(msg);  //First message is the "create tunnel" message
        _remoteIP = IPAddress((uint32_t) msg->getClientIp());
        assert (_remoteIP == msg->getClientIp());
        _remotePort = msg->getClientPort();
        assert (_remotePort == msg->getClientPort());

    } else {
//        Serial.println("msg is null");
    }
    _openportClient = openportClient;
    _currentPosition = 0;
}


size_t OpenportWiFiClient::write(const uint8_t *buf, size_t size) {
//    DEBUG_SERIAL.printf("OpenportWiFiClient::write size: %d \n", size);
//    DEBUG_SERIAL.printf("OpenportWiFiClient::buf size: %s \n", buf);

    const char *payload = reinterpret_cast<const char *>(buf);

//    DEBUG_SERIAL.println(payload);
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
    for (uint i = 0; i < end; i++) {
        buf[i] = msg->getPayload()[i + _currentPosition];
    }
    if (bytesLeftToRead > size){  // still more to read
        _currentPosition = msg->getPayloadSize() - bytesLeftToRead;
    } else {
        DEBUG_SERIAL.println("OpenportWiFiClient::pop");

        _messages.pop_front();
        DEBUG_SERIAL.println("OpenportWiFiClient::delete");

        free(msg);
        _currentPosition = 0;
    }
    return end;
}

void OpenportWiFiClient::stop() {
    DEBUG_SERIAL.printf("OpenportWiFiClient::stop _openportClient: %p\n", _openportClient);
    if (!this->operator bool()){
        return;
    }


    OpenportMessage msg = OpenportMessage("", 0, ChOpClose, _remoteIP, _remotePort);
    _openportClient->send(&msg);

    for (auto msg : _messages) {
        free(msg);
    }
    // todo: remove from lists?
    // _openportWiFiServer->removeClient(this);
}

OpenportWiFiClient::operator bool() {
    return _openportClient != nullptr;
}

void OpenportWiFiClient::addMessage(OpenportMessage *msg) {
    _messages.push_back(msg);
}

uint8_t OpenportWiFiClient::connected() {
    DEBUG_SERIAL.println("OpenportWiFiClient::connected()");
    return true; // todo: finetune
}

int OpenportWiFiClient::available() {
    return !_messages.empty();
}

IPAddress OpenportWiFiClient::remoteIP() {
    return _remoteIP;
}

uint16_t OpenportWiFiClient::remotePort() {
    return _remotePort;
}


