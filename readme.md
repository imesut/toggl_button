# What is Toggl Button?

Toggl Button, is a ESP866 based physical button project to start and stop [Toggl](https://toggl.com) time entries.

[Toggl](https://toggl.com) is a productivity total which is designed for tracking times by hand and it provides automatic collection of digital time. Toggl allows you to self evaluate your productivity and focus.

This project is prepared by Toggl's [official rest APIs](https://github.com/toggl/toggl_api_docs) and web sockets.


## How to Set Up?

1. Getting Token
2. Updating Token

# How To Guide

**TODO: Add first step instructions**

After the box has been prepared, I noticed that Toggl Button started to not send http request.

After some research [[Reference 1](https://github.com/Links2004/arduinoWebSockets/issues/271#issuecomment-357328117)], I figured out ESP8266 doesn't allow multiple SSL connections. Toggl's all API's was SSL based, they didn't use insecure services. So there is two decisions at the hand;

1. Buiding a backend layer which proxies the Toggl services as a single web socket service (The device will have just one SSL connection)
2. Reducing the connection count to one (The device will manage connections, before http requests, socket will be closed, after the request it would be opened again.)

I selected option 2. It's not so safe, I know. (If you're regarding to do this project like me, you can consider ESP 32 as well, it allows multiple connections.) I've also see some options like building ESP8266 firmware from scratch by modifying two same time connections. (This is not a practical option for me.)

So, lets start workaround.

# References and Sources

Thanks to the open sources communities. I'm very thankful to the library developers who make technical details abstracted.

1. First setup guide for [LoLin NodeMCU](http://henrysbench.capnfatz.com/henrys-bench/arduino-projects-tips-and-more/arduino-esp8266-lolin-nodemcu-getting-started/) 
1. [Wifi Manager library](https://randomnerdtutorials.com/wifimanager-with-esp8266-autoconnect-custom-parameter-and-manage-your-ssid-and-password/) is a helpful tool to change password when WiFi network has changed.
1. [ArduinoWebsockets](https://github.com/gilmaimon/ArduinoWebsockets) library from [Gil Maimon](https://github.com/gilmaimon), became my favorite library. Gil M. was actively seeking for open issues about his library. He released [a patch for Toggl's websocket compatibility](https://github.com/gilmaimon/ArduinoWebsockets/issues/67) by adding a request parameter. This problem was the most time consuming task in this project.


# Project To-Do's

- Stopping time entries from button (A minor problem is remaining)
- Add preference for tag / project selection
- Physical Bring-up
- Taking Photos
- Documentation
  - Initial Setup Guide
  - Material List & Costs
  - Demonstration video
  - Technical Notes
- Submitting step by step guide to other platforms
