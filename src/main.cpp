/*
* esp8266 simple WebSocket server
* https://www.mischainti.org
*
* The server response with the
* echo of the message you send
* and send a broadcast every 5secs
*
*/

#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <WebSocketsServer.h>
#include <EthernetClient.h>

const char *ssid = "";
const char *password = "";
const uint8_t wsPort = 81;

unsigned long messageInterval = 5000;
bool connected = false;

#define DEBUG_SERIAL Serial

WebSocketsServer webSocket = WebSocketsServer(wsPort);
WiFiServer server(80);

//EthernetClient tcpClient;
WiFiClient tcpClient;

String getResponse(String request){
    // if you've gotten to the end of the line (received a newline
    // character) and the line is blank, the http request has ended,
    // so you can send a reply
    // send a standard http response header

    String response = "HTTP/1.1 200 OK";

    response += "\nContent-Type: text/html";
    response += "\nConnection: close";  // the connection will be closed after completion of the response
//                    response += "\nRefresh: 5";  // refresh the page automatically every 5 sec
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
        // an http request ends with a blank line
        bool currentLineIsBlank = true;
        if (client.connected()) {
            String readString = "";
            while (!client.available()){
                delay(1);
            }
            while(client.available()){
                readString += (char) client.read();
            }
            Serial.println(readString);

            String response = getResponse(readString);
            Serial.println("response:");
            Serial.println(response);
            client.print(response);
        } else{
            Serial.println("client is not connected?");
        }
        // give the web browser time to receive the data
        delay(1);
        // close the connection:
        client.stop();
        Serial.println("client disconnected");
    }
}



void webSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length) {
    String response2;

    switch (type) {
        case WStype_DISCONNECTED:
            DEBUG_SERIAL.printf("[%u] Disconnected!\n", num);
            break;
        case WStype_CONNECTED: {
            IPAddress ip = webSocket.remoteIP(num);
            DEBUG_SERIAL.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);
            // send message to client
            webSocket.sendTXT(num, "Connected");
        }
            break;
        case WStype_TEXT:
            DEBUG_SERIAL.printf("[%u] RECEIVE TXT: %s\n", num, payload);
            // send message to client
//            webSocket.sendTXT(num, "(ECHO MESSAGE) " + String((char *) payload));
            // send data to all connected clients
            // webSocket.broadcastTXT("message here");
            response2 = getResponse(""); // todo: payload
            webSocket.sendTXT(num, response2);

//
//            if (tcpClient.connect(WiFi.localIP(), 80)) {
//
//                Serial.println("Connected to local server");
//                tcpClient.write((char *) payload); // Send what is received from the websocket
//
//                // callback to server so it can process the request.
//                webserverLoop();
//
//                // Read the response
//                String readString = "";
//                while (tcpClient.available()){
//                    readString += tcpClient.readString();
//                }
//                webSocket.sendTXT(num, readString);
//
//                if (!tcpClient.connected()) {
//                    Serial.println();
//                    Serial.println("disconnecting.");
//                    tcpClient.stop();
//                }
//            } else {
//                Serial.println("connection failed");
//            }
            break;
        case WStype_BIN:
            DEBUG_SERIAL.printf("[%u] get binary length: %u\n", num, length);
            hexdump(payload, length);
            // send message to client
            // webSocket.sendBIN(num, payload, length);
            break;
    }
}



void setup() {
    DEBUG_SERIAL.begin(115200);

//  DEBUG_SERIAL.setDebugOutput(true);

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

    DEBUG_SERIAL.println("WebSocket complete uri is: ");
    DEBUG_SERIAL.print("ws://");
    DEBUG_SERIAL.print(WiFi.localIP());
    DEBUG_SERIAL.print(":");
    DEBUG_SERIAL.print(wsPort);
    DEBUG_SERIAL.println("/");

    webSocket.begin();
    webSocket.onEvent(webSocketEvent);

    server.begin();

}


unsigned long lastUpdate = millis();

void loop() {
    webSocket.loop();
//    if (lastUpdate + messageInterval < millis()) {
//        DEBUG_SERIAL.println("[WSc] SENT: Simple broadcast client message!!");
//        webSocket.broadcastTXT("Simple broadcast client message!!");
//        lastUpdate = millis();
//    }

    webserverLoop();
}

//#include <WiFi.h>


