//
// Created by Jan De Bleser on 19/01/23.
//

#include "OpenportClient.h"
//#include <ArduinoJson.h>
#include <ArduinoJson.h>
#include "tiny_websockets/network/esp8266/esp8266_tcp.hpp"
#include "tiny_websockets/network/generic_esp/generic_esp_clients.hpp"
#include "tiny_websockets/network/tcp_client.hpp"

#include <memory>
#include <functional>
#include <vector>


// USERTrust RSA Certification Authority
// Expires Mon, 18 Jan 2038 23:59:59 GMT

const char ssl_ca_cert[] PROGMEM = \
"-----BEGIN CERTIFICATE-----\n" \
"MIIF3jCCA8agAwIBAgIQAf1tMPyjylGoG7xkDjUDLTANBgkqhkiG9w0BAQwFADCB\n" \
"iDELMAkGA1UEBhMCVVMxEzARBgNVBAgTCk5ldyBKZXJzZXkxFDASBgNVBAcTC0pl\n" \
"cnNleSBDaXR5MR4wHAYDVQQKExVUaGUgVVNFUlRSVVNUIE5ldHdvcmsxLjAsBgNV\n" \
"BAMTJVVTRVJUcnVzdCBSU0EgQ2VydGlmaWNhdGlvbiBBdXRob3JpdHkwHhcNMTAw\n" \
"MjAxMDAwMDAwWhcNMzgwMTE4MjM1OTU5WjCBiDELMAkGA1UEBhMCVVMxEzARBgNV\n" \
"BAgTCk5ldyBKZXJzZXkxFDASBgNVBAcTC0plcnNleSBDaXR5MR4wHAYDVQQKExVU\n" \
"aGUgVVNFUlRSVVNUIE5ldHdvcmsxLjAsBgNVBAMTJVVTRVJUcnVzdCBSU0EgQ2Vy\n" \
"dGlmaWNhdGlvbiBBdXRob3JpdHkwggIiMA0GCSqGSIb3DQEBAQUAA4ICDwAwggIK\n" \
"AoICAQCAEmUXNg7D2wiz0KxXDXbtzSfTTK1Qg2HiqiBNCS1kCdzOiZ/MPans9s/B\n" \
"3PHTsdZ7NygRK0faOca8Ohm0X6a9fZ2jY0K2dvKpOyuR+OJv0OwWIJAJPuLodMkY\n" \
"tJHUYmTbf6MG8YgYapAiPLz+E/CHFHv25B+O1ORRxhFnRghRy4YUVD+8M/5+bJz/\n" \
"Fp0YvVGONaanZshyZ9shZrHUm3gDwFA66Mzw3LyeTP6vBZY1H1dat//O+T23LLb2\n" \
"VN3I5xI6Ta5MirdcmrS3ID3KfyI0rn47aGYBROcBTkZTmzNg95S+UzeQc0PzMsNT\n" \
"79uq/nROacdrjGCT3sTHDN/hMq7MkztReJVni+49Vv4M0GkPGw/zJSZrM233bkf6\n" \
"c0Plfg6lZrEpfDKEY1WJxA3Bk1QwGROs0303p+tdOmw1XNtB1xLaqUkL39iAigmT\n" \
"Yo61Zs8liM2EuLE/pDkP2QKe6xJMlXzzawWpXhaDzLhn4ugTncxbgtNMs+1b/97l\n" \
"c6wjOy0AvzVVdAlJ2ElYGn+SNuZRkg7zJn0cTRe8yexDJtC/QV9AqURE9JnnV4ee\n" \
"UB9XVKg+/XRjL7FQZQnmWEIuQxpMtPAlR1n6BB6T1CZGSlCBst6+eLf8ZxXhyVeE\n" \
"Hg9j1uliutZfVS7qXMYoCAQlObgOK6nyTJccBz8NUvXt7y+CDwIDAQABo0IwQDAd\n" \
"BgNVHQ4EFgQUU3m/WqorSs9UgOHYm8Cd8rIDZsswDgYDVR0PAQH/BAQDAgEGMA8G\n" \
"A1UdEwEB/wQFMAMBAf8wDQYJKoZIhvcNAQEMBQADggIBAFzUfA3P9wF9QZllDHPF\n" \
"Up/L+M+ZBn8b2kMVn54CVVeWFPFSPCeHlCjtHzoBN6J2/FNQwISbxmtOuowhT6KO\n" \
"VWKR82kV2LyI48SqC/3vqOlLVSoGIG1VeCkZ7l8wXEskEVX/JJpuXior7gtNn3/3\n" \
"ATiUFJVDBwn7YKnuHKsSjKCaXqeYalltiz8I+8jRRa8YFWSQEg9zKC7F4iRO/Fjs\n" \
"8PRF/iKz6y+O0tlFYQXBl2+odnKPi4w2r78NBc5xjeambx9spnFixdjQg3IM8WcR\n" \
"iQycE0xyNN+81XHfqnHd4blsjDwSXWXavVcStkNr/+XeTWYRUc+ZruwXtuhxkYze\n" \
"Sf7dNXGiFSeUHM9h4ya7b6NnJSFd5t0dCy5oGzuCr+yDZ4XUmFF0sbmZgIn/f3gZ\n" \
"XHlKYC6SQK5MNyosycdiyA5d9zZbyuAlJQG03RoHnHcAP9Dc1ew91Pq7P8yF1m9/\n" \
"qS3fuQL39ZeatTXaw2ewh0qpKJ4jjv9cJ2vhsE/zB+4ALtRZh8tSQZXq9EfX7mRB\n" \
"VXyNWQKV3WKdwrnuWih0hKWbt5DHDAff9Yk2dDLWKMGwsAvgnEzDHNb842m1R0aB\n" \
"L6KCq9NjRHDEjf8tM7qtj3u1cIiuPhnPQCjY/MiQu12ZIvVS5ljFH4gxQ+6IHdfG\n" \
"jjxDah2nGN59PRbxYvnKkKj9\n" \
"-----END CERTIFICATE-----\n";

#ifdef ESP8266

X509List *serverTrustedCA = new X509List(ssl_ca_cert);
#endif


OpenportClient::OpenportClient(char *host, char *key_token) {
    _host = host;
    _key_token = key_token;
//    _messages = std::deque< OpenportMessage* >();

    DEBUG_SERIAL.printf("&_wifiClient: %p", &_wifiClient);
//#ifdef ESP8266
//    _wifiClient.setTrustAnchors(serverTrustedCA);
//    _wifiClient.setBufferSizes(512, 512);
//
//
//    auto *genericTcpClient = new websockets::network::SecuredEsp8266TcpClient(_wifiClient);
//#elif defined(ESP32)
//
//    char *tmpCrt = new char[strlen(ssl_ca_cert) + 1];
//    memcpy(tmpCrt, ssl_ca_cert, strlen(ssl_ca_cert) + 1);
//    _wifiClient.setCACert(tmpCrt);
//    auto* genericTcpClient = new websockets::network::SecuredEsp32TcpClient(_wifiClient);
//#endif
//
//    auto sharedPtr = std::shared_ptr<websockets::network::TcpClient>(genericTcpClient);
//    _webSocket = websockets::WebsocketsClient(sharedPtr);
}

void addrFromPayload(char *address, const char *payload) {
    sprintf(address, "%d.%d.%d.%d:%d", int(payload[0]), int(payload[1]), int(payload[2]), int(payload[3]),
            int((payload[4] << 8) + payload[5]));
}

void OpenportClient::send(OpenportMessage *msg) {
    DEBUG_SERIAL.println("OpenportClient::Sending message: ");
    DEBUG_SERIAL.println(msg->getPayload());
//    DEBUG_SERIAL.println("OpenportClient::Raw message: ");
    char* rawData = msg->getRawData();
//    DEBUG_SERIAL.println(rawData);
    _webSocket.sendBinary(rawData, msg->getRawDataLength());
}

void OpenportClient::webSocketMessage(const websockets::WebsocketsMessage message) {
    Serial.print("Got Message: ");
    Serial.println(message.data());
    uint length = message.length();
    DEBUG_SERIAL.printf("get binary length: %u\n", length);
    if (length > 6) {
        auto msg = new OpenportMessage(message.c_str(), length);
        _messages.push_back(msg);
        DEBUG_SERIAL.print("Messages in queue: ");
        DEBUG_SERIAL.println(_messages.size());
    } else {
        DEBUG_SERIAL.printf("Got a short message: %d", length);
    }
}

void OpenportClient::webSocketEvent(const websockets::WebsocketsEvent event, const String data) {
    if (event == websockets::WebsocketsEvent::ConnectionOpened) {
        Serial.println("Connection Opened");
        requestWSPortForward();
    } else if (event == websockets::WebsocketsEvent::ConnectionClosed) {
        Serial.println("Connection Closed");
    } else if (event == websockets::WebsocketsEvent::GotPing) {
        Serial.println("Got a Ping!");
    } else if (event == websockets::WebsocketsEvent::GotPong) {
        Serial.println("Got a Pong!");
    } else {
        Serial.printf("Got an unknown event: %d \n", event);
    }
}

void OpenportClient::requestWSPortForward() {
    DynamicJsonDocument doc(2000);
    String jsonStr;
    doc["token"] = _session_token;
    doc["port"] = _server_port;
    serializeJson(doc, jsonStr);
    DEBUG_SERIAL.println("sending ");
    DEBUG_SERIAL.println(jsonStr);
    _webSocket.send(jsonStr.c_str());
}

// TODO: Print a message when the client is connected
// TODO: Stop connecting when a fatal error is returned from the server.

bool OpenportClient::sendHttpRequest() {
    Serial.printf("Free Memory at OpenportClient::sendHttpRequest: %d\n", ESP.getFreeHeap());

    HTTPClient httpClient;
    char path[128];
    sprintf(path, "https://%s/api/v1/request-port", _host);
    Serial.print("post to  ");
    Serial.println(path);
//    Serial.print("deleting _wificlient  ");
//
////    delete &_wifiClient;
//    Serial.print("recreating _wificlient  ");

//    _wifiClient = WiFiClientSecure();
////    _wifiClient.setCACert(ssl_ca_cert);
//    char *tmpCrt = new char[strlen(ssl_ca_cert) + 1];
//     memcpy(tmpCrt, ssl_ca_cert, strlen(ssl_ca_cert) + 1);
//
//    _wifiClient.setCACert(tmpCrt);
//    httpClient.begin(_wifiClient, String(path));

    WiFiClientSecure tmpClient = WiFiClientSecure();
        char *tmpCrt = new char[strlen(ssl_ca_cert) + 1];
     memcpy(tmpCrt, ssl_ca_cert, strlen(ssl_ca_cert) + 1);

    tmpClient.setCACert(tmpCrt);
    httpClient.begin(tmpClient, String(path));

    httpClient.addHeader("Content-Type", "application/x-www-form-urlencoded");
    char postData[128];
    sprintf(postData, "ws_token=%s", _key_token);
    Serial.println(postData);

    int responseCode = httpClient.POST(postData);
    if (responseCode == 200) {
        Serial.print("HTTP request successful");
        String payload = httpClient.getString();
        Serial.println(payload);

        DynamicJsonDocument doc(1024);
        deserializeJson(doc, payload);
        _server_port = doc["server_port"];
        _session_token = doc["session_token"].as<String>();
        _ws_host = doc["server_ip"].as<String>();
    } else {
        Serial.print("HTTP Response code: ");
        Serial.println(responseCode);
        String payload = httpClient.getString();
        Serial.println(payload);
        httpClient.end();
        return false;
    }
    httpClient.end();
    return true;
}

bool OpenportClient::connectWS() {// Connect websocket
    Serial.printf("Free Memory at OpenportClient::connectWS: %d\n", ESP.getFreeHeap());

#ifdef ESP8266
    _wifiClient.setBufferSizes(512, 512);
    _webSocket.setTrustAnchors(serverTrustedCA);
#elif defined(ESP32)
    //    setCACert(ssl_ca_cert);
#endif




//    WiFiClientSecure tmpClient = WiFiClientSecure();
    char *tmpCrt = new char[strlen(ssl_ca_cert) + 1];
    memcpy(tmpCrt, ssl_ca_cert, strlen(ssl_ca_cert) + 1);
//
//    tmpClient.setCACert(tmpCrt);

//    auto* genericTcpClient = new websockets::network::SecuredEsp32TcpClient(tmpClient);
//    auto sharedPtr = std::shared_ptr<websockets::network::TcpClient>(genericTcpClient);
//    _webSocket = websockets::WebsocketsClient();
    _webSocket.setCACert(tmpCrt);

    std::function<void(websockets::WebsocketsMessage)> onMessageCallback =
            std::bind(&OpenportClient::webSocketMessage, this, std::placeholders::_1);
    std::function<void(websockets::WebsocketsEvent, String)> onEventCallback =
            std::bind(&OpenportClient::webSocketEvent, this, std::placeholders::_1, std::placeholders::_2);

    _webSocket.onMessage(onMessageCallback);
    _webSocket.onEvent(onEventCallback);

    char fullPath[128];
    sprintf(fullPath, "wss://%s:443/ws", _ws_host.c_str());
    Serial.print("WS Connecting to ");
    Serial.println(fullPath);
    Serial.printf("Free Memory at OpenportClient::connectWS 1: %d\n", ESP.getFreeHeap());

    // todo: set insecure after certificate expiration date
//    _webSocket.setInsecure();1
    bool connected = _webSocket.connect(fullPath);
    Serial.printf("Free Memory at OpenportClient::connectWS 2: %d\n", ESP.getFreeHeap());
    if (!connected) {
        Serial.println("WS Connect failed");
        Serial.print("Close reason: ");
        Serial.println(_webSocket.getCloseReason());
        if (!_webSocket.ping()) {
            Serial.println("WS Ping failed");
            return false;
        } else {
            Serial.println("WS Ping succeeded");
        }
    }
    Serial.printf("Free Memory at OpenportClient::connectWS 3: %d\n", ESP.getFreeHeap());
    if (!_webSocket.ping()) {
        Serial.println("WS Ping failed");
        return false;
    } else {
        Serial.println("WS Ping succeeded");
    }
    Serial.printf("Free Memory at OpenportClient::connectWS 4: %d\n", ESP.getFreeHeap());

//    _webSocket.setReconnectInterval(5000);
//    _webSocket.enableHeartbeat(15000, 3000, 2);
    Serial.print("Connecting attempted to ");
    Serial.println(_ws_host);
    Serial.print("http://");
    Serial.print(_ws_host);
    Serial.print(":");
    Serial.println(_server_port);

    return true;
}

bool OpenportClient::connect() {
    Serial.printf("Free Memory at OpenportClient::connect: %d\n", ESP.getFreeHeap());

//     Free the memory previously used.
//    _webSocket.disconnect();

    if (!sendHttpRequest()) {
        return false;
    }

//    if (!sendHttpRequest()) {
//        return false;
//    }
//
//    delay(1000000);

    return connectWS();
}

unsigned long messageInterval = 5000;


void OpenportClient::loop() {
//    DEBUG_SERIAL.println("OpenportClient::loop");
    _webSocket.poll();
    if (_lastUpdate + messageInterval < millis()) {
        Serial.printf("is connected: %u\n", _webSocket.available());
        _lastUpdate = millis();
        if (!_webSocket.available()) {
            Serial.println("WS not connected");
            connect();
        }
    }
}

const char *OpenportClient::getRemoteHost() {
    return _ws_host.c_str();
}

int OpenportClient::getRemotePort() {
    return _server_port;
}

std::unique_ptr<char> OpenportClient::getRemoteAddress() {
    char *remoteAddress = new char[128];
    sprintf(remoteAddress, "%s:%d", _ws_host.c_str(), _server_port);
    return std::unique_ptr<char>(remoteAddress);
}

std::deque<OpenportMessage *> *OpenportClient::getMessages() {
//    DEBUG_SERIAL.printf("%p OpenportClient::getMessages (%d)\n", this, _messages.size());
    return &_messages;
}


void setTimeUsingSNTP() {
    // Synchronize time using SNTP. This is necessary to verify that
    // the TLS certificates offered by the server are currently valid.
    Serial.print("Setting time using SNTP");
    configTime(1 * 3600, 0, "pool.ntp.org", "time.nist.gov");
    time_t now = time(nullptr);
    while (now < 8 * 3600 * 2) {
        delay(500);
        Serial.print(".");
        now = time(nullptr);
    }
    Serial.println("");
    struct tm timeinfo;
    gmtime_r(&now, &timeinfo);
    Serial.print("Current time: ");
    Serial.print(asctime(&timeinfo));
}


OpenportMessage::OpenportMessage(const char *rawData, uint16_t length) {
    DEBUG_SERIAL.println("Creating a message from raw data");
    DEBUG_SERIAL.println(
            "IP: " + String((int) rawData[0]) + "." + String((int) rawData[1]) + "." + String((int) rawData[2]) + "." +
            String((int) rawData[3]));
    DEBUG_SERIAL.println("port: " + String((int) rawData[4]) + "<< 8 + " + String((int) rawData[5]));
    DEBUG_SERIAL.println("type: " + String((int) rawData[6]));

    _rawData = new char[length];
    memcpy(_rawData, rawData, length);
    _clientIp = IPAddress(rawData[0], rawData[1], rawData[2], rawData[3]);
    _clientPort = rawData[4] << 8 | rawData[5];
    _type = rawData[6];
    _payloadSize = length - OPENPORT_MSG_HEADER_LENGTH;
    DEBUG_SERIAL.printf("_payloadSize: %d\n", _payloadSize);
}

OpenportMessage::OpenportMessage(const char *payload, uint16_t payloadLength, uint8_t type, IPAddress clientIp,
                                 uint16_t clientPort) {
    _payloadSize = payloadLength;
    _type = type;
    _clientIp = clientIp;
    _clientPort = clientPort;

    _rawData = new char[_payloadSize + OPENPORT_MSG_HEADER_LENGTH];
    _rawData[0] = _clientIp[0];
    _rawData[1] = _clientIp[1];
    _rawData[2] = _clientIp[2];
    _rawData[3] = _clientIp[3];
    _rawData[4] = _clientPort >> 8;
    _rawData[5] = _clientPort & 0xFF;
    _rawData[6] = _type;
    uint i = 0;
    while (i < _payloadSize) {
        _rawData[OPENPORT_MSG_HEADER_LENGTH + i] = payload[i];
        i++;
    }
}

OpenportMessage::~OpenportMessage() {
    DEBUG_SERIAL.printf("OpenportMessage::~OpenportMessage %d\n", _payloadSize);
    delete[] _rawData;
}


char* OpenportMessage::getRawData() {
    return _rawData;

}

const char *OpenportMessage::getPayload() {
    return _rawData + OPENPORT_MSG_HEADER_LENGTH;
}

uint16_t OpenportMessage::getRawDataLength() {
    return _payloadSize + OPENPORT_MSG_HEADER_LENGTH;
}

int OpenportMessage::getType() {
    return _type;
}

IPAddress OpenportMessage::getClientIp() {
    return _clientIp;
}

uint16_t OpenportMessage::getClientPort() {
    return _clientPort;
}

uint16_t OpenportMessage::getPayloadSize() {
    return _payloadSize;
}


