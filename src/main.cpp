#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "../lib/Openport/src/Openport.h"

const char *ssid = "";
const char *password = "";

#define DEBUG_SERIAL Serial

WiFiServer server(80);

String getResponse(char* request) {
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

Openport openport = Openport("test.openport.io", "blahblahblah", &getResponse);

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

void setup() {
    DEBUG_SERIAL.begin(115200);
    DEBUG_SERIAL.setDebugOutput(true);
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

    openport.connect();
    server.begin();
}

void loop() {
    openport.loop();
    webserverLoop();
    delay(10);
}
