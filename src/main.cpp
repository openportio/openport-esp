#include <Arduino.h>
#ifdef ESP8266
#include <ESP8266WiFi.h>
#include "OpenportEsp8266TCPServer.h"
#elif defined(ESP32)
#include <WiFi.h>
#include "OpenportEsp32TCPServer.h"
#endif

#include "OpenportClient.h"
#include "OpenportWiFiServer.h"
#include "OpenportWiFiClient.h"

#include <typeinfo>
#include <ArduinoWebsockets.h>

const char *ssid = "";
const char *password = "";

#define DEBUG_SERIAL Serial

WiFiServer server(80);
OpenportClient openport_http = OpenportClient("test.openport.io", "blahblahblah");
OpenportClient openport_ws = OpenportClient("test.openport.io", "blahblahblah");

OpenportWiFiServer openport_http_server = OpenportWiFiServer(&openport_http);
OpenportWiFiServer openport_ws_server = OpenportWiFiServer(&openport_ws);

#ifdef ESP8266

OpenportEsp8266TCPServer* openportEsp8266TcpServer = new OpenportEsp8266TCPServer(openport_ws_server);
websockets::network::TcpServer* tcpServer = openportEsp8266TcpServer;
#elif defined(ESP32)
OpenportEsp32TCPServer* openportEsp32TcpServer = new OpenportEsp32TCPServer(openport_ws_server);
websockets::network::TcpServer* tcpServer = openportEsp32TcpServer;
#endif
websockets::WebsocketsServer ws_server = websockets::WebsocketsServer(tcpServer);

std::unique_ptr<char> getHTTPResponse(uint8_t* request) {
    DEBUG_SERIAL.printf("HTTP Request: %s\n", request);

    char ws_host[sizeof(openport_ws.getRemoteHost()) + sizeof(openport_ws.getRemotePort()) + 2];
    sprintf(ws_host, "%s:%d", openport_ws.getRemoteHost(), openport_ws.getRemotePort());

    char response2[] = "HTTP/1.1 200 OK" \
     "\nContent-Type: text/html" \
     "\nConnection: close" \
     "\nRefresh: 5" \
    "\n" \
    "\n<!DOCTYPE HTML>\n" \
    "<html lang=\"en\">\n" \
    "<head>\n" \
    "<meta charset=\"UTF-8\">\n" \
    "<meta http-equiv=\"X-UA-Compatible\" content=\"IE=edge\">\n" \
    "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n" \
    "<title>Client</title>\n" \
    "</head>\n" \
    "<body>\n" \
    "<h1>Openport Client</h1>\n" \
    "<p><b id=\"last-msg\"></b></p>\n" \
    "   <script>\n" \
    "const ws = new WebSocket(\"ws://%s\");\n" \
    "ws.onopen = function (event) {\n" \
    "ws.send(\"Hello\");\n" \
    "}\n" \
    "\n" \
    "ws.onmessage = function (event) {\n" \
    "document.getElementById(\"last-msg\").innerText = event.data;\n" \
    "}\n" \
    "</script>\n" \
    "</body>\n" \
    "</html>\n" ;

    auto response = new char[sizeof(response2) + sizeof(ws_host)];
    sprintf(response, response2, ws_host);

    return std::unique_ptr<char>(response);
}

void webserverLoop(WiFiServer *server ) {
    // listen for incoming clients
    WiFiClient* client = nullptr;
    DEBUG_SERIAL.println("webserverloop here 1");
    // Needs to be initialized here, because otherwise the type information is lost.
    WiFiClient tmpClient;
    OpenportWiFiClient tmpClient2 = OpenportWiFiClient();
    if (dynamic_cast<OpenportWiFiServer*>(server) != nullptr){
//        DEBUG_SERIAL.println("checking for openport client");
        tmpClient2 = (dynamic_cast<OpenportWiFiServer *>(server))->available();
//        DEBUG_SERIAL.println("got openport client");
        if (tmpClient2){
            client = &tmpClient2;
        }
    } else {
        tmpClient = server->available();
        if (tmpClient){
            client = &tmpClient;
        }
    }

    delay(100);
    if (client != nullptr){
        Serial.println("new client");
        Serial.println(typeid(*client).name());
        // a http request ends with a blank line
        if (client->connected()) {
            Serial.println("client is connected");
            uint8_t  readBuffer[1024];
            while (!client->available()) {
                delay(1);
            }
            Serial.println("client is available");
            int i = 0;
            // todo: this is not right, we should be reading until the end of the request
            while (i < 1024 && client->available()) {
                i = client->read(readBuffer, 1024);
            }
            Serial.printf("client read %d bytes\n", i);
            Serial.println(reinterpret_cast<const char *>(readBuffer));


            auto response = getHTTPResponse(readBuffer);
            Serial.printf("response: %d\n", strlen(response.get()));
//            Serial.println(response.get());
            client->write(response.get(), strlen(response.get()));
        } else {
            Serial.println("client is not connected?");
        }
        // give the web browser time to receive the data
        delay(1);
        // close the connection:
        DEBUG_SERIAL.print("webserverloop closing client");

        client->stop();
    } else{
        DEBUG_SERIAL.println("no client for you");
    }
    DEBUG_SERIAL.println("webserverloop here99");
}

void openportWebserverLoop(OpenportWiFiServer *server ) {
    server->available();
    DEBUG_SERIAL.printf("Server has %d clients \n", server->clients()->size());

    auto it = server->clients()->begin();

    while (it != server->clients()->end()) {
        OpenportWiFiClient* client = *it;
        DEBUG_SERIAL.printf("Checking client\n");
        DEBUG_SERIAL.printf("Checking client %s:%d\n", client->remoteIP().toString().c_str(), client->remotePort());

        if (client->connected() && client->available()) {
            uint8_t requestBuffer[4096];
            Serial.println("client is available");
            int i = client->read(requestBuffer, 4096);
            Serial.printf("client read %d bytes\n", i);
            Serial.println(reinterpret_cast<const char *>(requestBuffer));
            auto response = getHTTPResponse(requestBuffer);
//            Serial.printf("response: %d\n", strlen(response));
//            Serial.println(response);
            client->write(reinterpret_cast<const uint8_t*>(response.get()), strlen(response.get()));
            client->stop();
            delete client;
//            it = server->clients()->erase(it);  // commented out because the delete function already does this
        } else {
            it++;
        }
    }
}

void setup() {

    DEBUG_SERIAL.begin(115200);
    DEBUG_SERIAL.setDebugOutput(true);
    DEBUG_SERIAL.println();
    Serial.printf("Free Memory: %d\n", ESP.getFreeHeap());

    for (uint8_t t = 4; t > 0; t--) {
        DEBUG_SERIAL.printf("[SETUP] BOOT WAIT %d...\n", t);
        DEBUG_SERIAL.flush();
        delay(1000);
    }
    Serial.printf("Free Memory at BOOT: %d\n", ESP.getFreeHeap());

    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    DEBUG_SERIAL.println("My IP:");
    DEBUG_SERIAL.println(WiFi.localIP());
    Serial.printf("Free Memory after wifi connect: %d\n", ESP.getFreeHeap());

    setTimeUsingSNTP();
    Serial.printf("Free Memory after SNTP: %d\n", ESP.getFreeHeap());
    while (!openport_http.connect()){
        DEBUG_SERIAL.println("Failed to connect to OpenportClient (HTTP)");
        delay(1000);
    }
    Serial.printf("Free Memory after openport connect: %d\n", ESP.getFreeHeap());
//
//    while (!openport_ws.connect()){
//        DEBUG_SERIAL.println("Failed to connect to OpenportClient (WS)");
//        delay(1000);
//    }
    server.begin();
//    ws_server.listen(81);
    Serial.printf("Free Memory at the end of init: %d\n", ESP.getFreeHeap());
}

void wsServerLoop() {
    Serial.print("ws_server.poll()... ");
    if (!ws_server.poll()) {
        return;
    }
    websockets::WebsocketsClient client = ws_server.accept();
    Serial.print("client.available()...\n ");

    if(client.available()) {
        Serial.print("Got a ws client: ");

        websockets::WebsocketsMessage msg = client.readBlocking();

        // log
        Serial.print("Got WS Message: ");
        Serial.println(msg.data());

        // return echo
        client.send("Echo: " + msg.data());

        // close the connection
        client.close();
    }
}
int previousMemory = 0;
void loop() {

//    openport_http.loop();
//    openport_ws.loop();
//    webserverLoop(&server);
    DEBUG_SERIAL.println("webserverLoop");
//    webserverLoop(&openport_http_server);
    openportWebserverLoop(&openport_http_server);
//    DEBUG_SERIAL.println("looping");
//    wsServerLoop();
    Serial.printf("Free Memory: %d\n", ESP.getFreeHeap());
    DEBUG_SERIAL.printf("Lost Memory: %d\n", previousMemory - ESP.getFreeHeap());
    previousMemory = ESP.getFreeHeap();
    delay(1000);
}
