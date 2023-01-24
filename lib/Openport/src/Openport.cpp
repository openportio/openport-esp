//
// Created by Jan De Bleser on 19/01/23.
//

#include "Openport.h"
//#include <ArduinoJson.h>
#include "../../../.pio/libdeps/Debug/ArduinoJson/ArduinoJson.h"


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

X509List *serverTrustedCA = new X509List(ssl_ca_cert);

WiFiClientSecure wifiClient;

Openport::Openport(char *host, char *key_token, f_string on_message) {
    _host = host;
    _key_token = key_token;
    _on_message = on_message;
    wifiClient.setTrustAnchors(serverTrustedCA);
}

void addrFromPayload(char *address, const char *payload) {
    sprintf(address, "%d.%d.%d.%d:%d", int(payload[0]), int(payload[1]), int(payload[2]), int(payload[3]),
            int((payload[4] << 8) + payload[5]));
}

void Openport::webSocketMessage(const websockets::WebsocketsMessage message) {
    Serial.print("Got Message: ");
    Serial.println(message.data());

    int length = message.length();
    const char *payload = message.c_str();

    DEBUG_SERIAL.printf("get binary length: %u\n", length);
    if (length > 6) {
        char address[23];
        addrFromPayload(address, payload);
        uint8 chop = payload[6];
        if (length == 7) {
            DEBUG_SERIAL.printf("Got new connection from %s. Channel Operation: %d\n", address, chop);
        }
        if (length > 7) {
            DEBUG_SERIAL.printf("Got new message of length %d from %s. Channel Operation: %d\n", length,
                                address, chop);
            String response = _on_message((char *) &(payload[7]));
            char response_char[response.length() + 7];
            memcpy(response_char, payload, sizeof(payload[0]) * 6);
            response_char[6] = 2;
            uint i = 0;
            while (i < response.length()) {
                response_char[7 + i] = response[i];
                i++;
            }
            _webSocket.sendBinary(response_char, response.length() + 6);

            // Send close
            response_char[6] = 3;
            _webSocket.sendBinary(response_char, 7);
        }
    } else {
        DEBUG_SERIAL.printf("Got a short message: %d", length);
    }}

void Openport::webSocketEvent(const websockets::WebsocketsEvent event, const String data) {
    if(event == websockets::WebsocketsEvent::ConnectionOpened) {
        Serial.println("Connection Opened");
        requestWSPortForward();
    } else if(event ==  websockets::WebsocketsEvent::ConnectionClosed) {
        Serial.println("Connection Closed");
    } else if(event ==  websockets::WebsocketsEvent::GotPing) {
        Serial.println("Got a Ping!");
    } else if(event ==  websockets::WebsocketsEvent::GotPong) {
        Serial.println("Got a Pong!");
    }
}

void Openport::requestWSPortForward() {
    DynamicJsonDocument doc(2000);
    String jsonStr;
    doc["token"] = _session_token;
    doc["port"] = _server_port;
    serializeJson(doc, jsonStr);
    DEBUG_SERIAL.println("sending ");
    DEBUG_SERIAL.println(jsonStr);
    _webSocket.send(jsonStr.c_str());
}

// TODO: keep track of connection status with ping/pong

bool Openport::sendHttpRequest(){

    HTTPClient httpClient;
    char path[128];
    sprintf(path, "https://%s/api/v1/request-port", _host);
    Serial.print("post to  ");
    Serial.println(path);

    httpClient.begin(wifiClient, String(path));
    httpClient.addHeader("Content-Type", "application/x-www-form-urlencoded");
    char postData[128];
    sprintf(postData, "ws_token=%s", _key_token);
    Serial.println(postData);
    int responseCode = httpClient.POST(postData);
    if (responseCode==200) {
        Serial.print("HTTP request successful");
        String payload = httpClient.getString();
        Serial.println(payload);

        DynamicJsonDocument doc(1024);
        deserializeJson(doc, payload);
        _server_port = doc["server_port"];
        _session_token = doc["session_token"].as<String>();
        _ws_host = doc["server_ip"].as<String>();
    }
    else {
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

bool Openport::connectWS() {// Connect websocket
#ifdef ESP8266
    _webSocket.setTrustAnchors(serverTrustedCA);
#elif defined(ESP32)
    //    setCACert(ssl_ca_cert);
#endif
    std::function<void(websockets::WebsocketsMessage)> onMessageCallback =
            std::bind(&Openport::webSocketMessage, this, std::placeholders::_1);
    std::function<void(websockets::WebsocketsEvent, String)> onEventCallback =
            std::bind(&Openport::webSocketEvent, this, std::placeholders::_1, std::placeholders::_2);

    _webSocket.onMessage(onMessageCallback);
    _webSocket.onEvent(onEventCallback);

    char fullPath[128];
    sprintf(fullPath, "wss://%s:443/ws", _ws_host.c_str());
    Serial.print("WS Connecting to ");
    Serial.println(fullPath);

    // todo: set insecure after certificate expiration date
//    _webSocket.setInsecure();
    bool connected = _webSocket.connect(fullPath);
    if (!connected) {
        Serial.println("WS Connect failed");
        Serial.print("Close reason: ");
        Serial.println(_webSocket.getCloseReason());
        if (!_webSocket.ping()){
            Serial.println("WS Ping failed");
            return false;
        } else {
            Serial.println("WS Ping succeeded");
        }
    }
    if (!_webSocket.ping()){
        Serial.println("WS Ping failed");
        return false;
    } else {
        Serial.println("WS Ping succeeded");
    }

//    _webSocket.setReconnectInterval(5000);
//    _webSocket.enableHeartbeat(15000, 3000, 2);
    Serial.print("Connecting attempted to ");
    Serial.println(_host);
    return true;
}

bool Openport::connect() {
    if (!sendHttpRequest()) {
        return false;
    }
    return connectWS();
}

unsigned long messageInterval = 5000;


void Openport::loop() {
    _webSocket.poll();
    if (_lastUpdate + messageInterval < millis()) {
        Serial.printf("is connected: %u\n", _webSocket.available());
        _lastUpdate = millis();
    }
}

void setTimeUsingSNTP() {
    // Synchronize time using SNTP. This is necessary to verify that
    // the TLS certificates offered by the server are currently valid.
    Serial.print("Setting time using SNTP");
    configTime(8 * 3600, 0, "pool.ntp.org", "time.nist.gov");
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