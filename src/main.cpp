#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "../lib/Openport/src/OpenportClient.h"
#include "../.pio/libdeps/Debug/ArduinoWebsockets/src/tiny_websockets/server.hpp"
#include "../lib/Openport/src/OpenportWiFiServer.h"
#include "../lib/Openport/src/OpenportEsp8266TCPServer.h"
#include "../.pio/libdeps/Debug/ArduinoWebsockets/src/tiny_websockets/network/tcp_server.hpp"
#include <typeinfo>
const char *ssid = "";
const char *password = "";

#define DEBUG_SERIAL Serial

WiFiServer server(80);
OpenportClient openport_http = OpenportClient("test.openport.io", "blahblahblah");
OpenportClient openport_ws = OpenportClient("test.openport.io", "blahblahblah");

OpenportWiFiServer openport_http_server = OpenportWiFiServer(&openport_http);
OpenportWiFiServer openport_ws_server = OpenportWiFiServer(&openport_ws);

OpenportEsp8266TCPServer openportEsp8266TcpServer = OpenportEsp8266TCPServer(openport_ws_server);
websockets::WebsocketsServer ws_server = websockets::WebsocketsServer(&openportEsp8266TcpServer);

String getHTTPResponse(uint8_t* request) {
    String ws_host = String(openport_ws.getRemoteHost());
    ws_host = "notset";
    String response = "HTTP/1.1 200 OK";
    response += "\nContent-Type: text/html";
//    response += "\nConnection: close";  // the connection will be closed after completion of the response
//    response += "\nRefresh: 5";  // refresh the page automatically every 5 sec
    response += "\n";
    response += "\n<!DOCTYPE HTML>\n" \
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
"const ws = new WebSocket(\"ws://" + ws_host + "\");\n" \
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
return response;
}


void webserverLoop(WiFiServer *server ) {
    // listen for incoming clients
    WiFiClient* client = nullptr;
    // Needs to be initialized here, because otherwise the type information is lost.
    WiFiClient tmpClient;
    OpenportWiFiClient tmpClient2 = OpenportWiFiClient(nullptr, nullptr);

    if (dynamic_cast<OpenportWiFiServer*>(server) != nullptr){
        tmpClient2 = (dynamic_cast<OpenportWiFiServer *>(server))->available();
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
            Serial.println("client read");
            Serial.println(reinterpret_cast<const char *>(readBuffer));


            String response = getHTTPResponse(readBuffer);
            Serial.println("response:");
            Serial.println(response);
            char* response2 = (char*) malloc(response.length() + 1);
            response.toCharArray(response2, response.length() + 1);
            const uint8_t *buf = reinterpret_cast<const uint8_t *>(response2);
            client->write(buf, response.length());
            free(response2);
        } else {
            Serial.println("client is not connected?");
        }
        // give the web browser time to receive the data
        delay(1);
        // close the connection:
        client->stop();
    } else{
//        DEBUG_SERIAL.println("no client for you");
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

    setTimeUsingSNTP();
    while (!openport_http.connect()){
        DEBUG_SERIAL.println("Failed to connect to OpenportClient (HTTP)");
        delay(1000);
    }
//    while (!openport_ws.connect()){
//        DEBUG_SERIAL.println("Failed to connect to OpenportClient (WS)");
//        delay(1000);
//    }
    server.begin();
//    ws_server.listen(81);
}

void wsServerLoop() {
    websockets::WebsocketsClient client = ws_server.accept();
    if(client.available()) {
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

void loop() {
    openport_http.loop();
//    openport_ws.loop();
    webserverLoop(&server);
    webserverLoop(&openport_http_server);
//    DEBUG_SERIAL.println("looping");
//    wsServerLoop();
    delay(1000);
}
