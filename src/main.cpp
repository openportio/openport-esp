#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WebSocketsClient.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

const char *ssid = "";
const char *password = "";

unsigned long messageInterval = 5000;
bool connected = false;

#define DEBUG_SERIAL Serial

WiFiServer server(80);
WebSocketsClient webSocket;

const char* host = "test.openport.io";

const char* OPENPORT_SERVER = "https://test.openport.io";
const char* FINGERPRINT = "3F 51 88 B5 B3 06 42 24 1A 7E 03 E8 27 47 D5 0B 3D EC 53 70";
const char* OPENPORT_WS_TOKEN = "blahblahblah";
int server_port = -1;
String session_token = "to be set";

String getResponse(char *request) {
    String response = "HTTP/1.1 200 OK";
    response += "\nContent-Type: text/html";
    response += "\nConnection: close";  // the connection will be closed after completion of the response
    response += "\nRefresh: 5";  // refresh the page automatically every 5 sec
    response += "\n";
    response += "\n<!DOCTYPE HTML>";
    response += "\n<html>";
    response += "\n<h1>It is working!!!</h1>";
    response += "\n</html>";
    return response;
}

void webserverLoop() {
    // listen for incoming clients
    WiFiClient client = server.available();
    if (client) {
        Serial.println("new client");
        // a http request ends with a blank line
        if (client.connected()) {
            char buffer[1024];
            while (!client.available()) {
                delay(1);
            }
            int i = 0;
            while (i < 1024 && client.available()) {
                buffer[i++] = client.read();
            }
            Serial.println(buffer);

            String response = getResponse(buffer);
            Serial.println("response:");
            Serial.println(response);
            client.print(response);
        } else {
            Serial.println("client is not connected?");
        }
        // give the web browser time to receive the data
        delay(1);
        // close the connection:
        client.stop();
        Serial.println("client disconnected");
    }
}

void addrFromPayload(char *address, uint8_t *payload) {
    sprintf(address, "%d.%d.%d.%d:%d", int(payload[0]), int(payload[1]), int(payload[2]), int(payload[3]),
            int((payload[4] << 8) + payload[5]));
}


void webSocketEvent(WStype_t type, uint8_t *payload, size_t length) {
    String response2;
    DynamicJsonDocument doc(2000);
    String jsonStr;

    switch (type) {
        case WStype_DISCONNECTED:
            DEBUG_SERIAL.printf("Disconnected!\n");
            break;
        case WStype_CONNECTED:
            DEBUG_SERIAL.println("Websocket is connected");

            doc["token"] = session_token;
            doc["port"]   = server_port;

            serializeJson(doc, jsonStr);
            DEBUG_SERIAL.println("sending ");
            DEBUG_SERIAL.println(jsonStr);
            webSocket.sendTXT(jsonStr);
            break;
        case WStype_TEXT:
            DEBUG_SERIAL.printf("RECEIVE TXT: %s\n", payload);
            // send message to client
//            webSocket.sendTXT(num, "(ECHO MESSAGE) " + String((char *) payload));
            response2 = getResponse(""); // todo: payload
            webSocket.sendTXT(response2);
            break;
        case WStype_BIN:
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
                    String response = getResponse((char *) &(payload[7]));
                    char response_char[response.length() + 7];
                    memcpy(response_char, payload, sizeof(payload[0]) * 6);
                    response_char[6] = 2;
                    int i = 0;
                    while (i < response.length()) {
                        response_char[7 + i] = response[i];
                        i++;
                    }
                    webSocket.sendBIN((const uint8_t *) response_char, response.length() + 6);

                    // Send close
                    response_char[6] = 3;
                    webSocket.sendBIN((const uint8_t *) response_char, 7);
                }
            } else {
                DEBUG_SERIAL.printf("Got a short message: %d", length);
            }

            break;
    }
}

void connectToOpenport(const char* server, const char* ws_token) {
    HTTPClient httpClient;
    char path[128];
    sprintf(path, "%s/api/v1/request-port", server);
    Serial.print("post to  ");
    Serial.println(path);
    httpClient.begin(String(path), FINGERPRINT);
    httpClient.addHeader("Content-Type", "application/x-www-form-urlencoded");
    char postData[128];
    sprintf(postData, "ws_token=%s",ws_token);
    Serial.println(postData);
    int responseCode = httpClient.POST(postData);
    if (responseCode>0) {
        Serial.print("HTTP Response code: ");
        Serial.println(responseCode);
        String payload = httpClient.getString();
        Serial.println(payload);

        DynamicJsonDocument doc(1024);
        deserializeJson(doc, payload);
        server_port = doc["server_port"];
        session_token = doc["session_token"].as<String>();
    }
    else {
        Serial.print("Error code: ");
        Serial.println(responseCode);
    }
    // Free resources
    httpClient.end();


    webSocket.beginSSL(host, 443, "/ws");
    webSocket.onEvent(webSocketEvent);
//     use HTTP Basic Authorization this is optional remove if not needed
//    webSocket.setAuthorization("user", "Password");

    // try ever 5000 again if connection has failed
    webSocket.setReconnectInterval(5000);
    webSocket.enableHeartbeat(15000, 3000, 2);
    Serial.print("Connecting to attempted to ");
    Serial.println(host);
}

void setup() {
    DEBUG_SERIAL.begin(115200);
    DEBUG_SERIAL.setDebugOutput(true);

    DEBUG_SERIAL.println();
    DEBUG_SERIAL.println();
    DEBUG_SERIAL.println();

    for (uint8_t t = 4; t > 0; t--) {
        DEBUG_SERIAL.printf("[SETUP] BOOT WAIT %d...\n", t);
        DEBUG_SERIAL.flush();
        delay(1000);
    }

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    DEBUG_SERIAL.println("My IP:");
    DEBUG_SERIAL.println(WiFi.localIP());

    connectToOpenport(OPENPORT_SERVER, OPENPORT_WS_TOKEN);
    server.begin();
}

unsigned long lastUpdate = millis();

void loop() {
    webSocket.loop();
    if (lastUpdate + messageInterval < millis()) {
        Serial.printf("is connected: %u\n", webSocket.isConnected());
        lastUpdate = millis();
    }
    webserverLoop();
    delay(10);
}
