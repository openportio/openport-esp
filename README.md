Openport-ESP
====

This is the official library for ESP devices for the openport.io service.


Installation
------------

- platformio:

```yaml
dependencies:
  openport-esp:
    git: https://github.com/openportio/openport-esp.git
```

You will need to use a token to authenticate the client. 
Since the library is still in BETA, please contact
[sales@openport.io](mailto:sales@openport.io)  to get a token.


Usage
---

```C++

#include secrets.h
#include "OpenportClient.h"
OpenportClient openport_http = OpenportClient("openport.io", OPENPORT_TOKEN, 80);

void setup(){
    openport_http.connect();
}

void loop(){
    openport_http.loop();
    std::deque<OpenportMessage *> messages = openport_http.getMessages();
    for (OpenportMessage *message : messages) {
        Serial.println(message->getClientIp());
        Serial.println(message->getClientPort());
        Serial.print(message->getRawData());
        Serial.println();
        auto msg = "Hello from ESP";
        OpenportMessage *response = OpenportMessage(
                msg,
                msg.length(),
                ChOpNew, 
                message->getClientIp(), 
                message->getClientPort());
        openport_http.send(response);
    }
}
```

Important:
---
To run this library smoothly with SSL, you need to add (or overwrite) the following line in your sdkconfig.h file:

```
#define CONFIG_ARDUINO_LOOP_STACK_SIZE 18192
```

Examples
---
See the examples folder for more information.