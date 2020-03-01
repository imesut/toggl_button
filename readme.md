# What is Toggl Button?

Toggl Button, is a ESP866 based physical button project to start and stop [Toggl](https://toggl.com) time entries.

[Toggl](https://toggl.com) is a productivity total which is designed for tracking times by hand and it provides automatic collection of digital time. Toggl allows you to self evaluate your productivity and focus.

This project is prepared by Toggl's [official rest APIs](https://github.com/toggl/toggl_api_docs) and web sockets.


# How To Build it?

## How to Set Up?

1. Getting Token
2. Updating Token

## Wires and Screws

## Cost

# References and Sources

Thanks to the open sources communities. I'm very thankful to the library developers who make technical details abstracted.

1. First setup guide for [LoLin NodeMCU](http://henrysbench.capnfatz.com/henrys-bench/arduino-projects-tips-and-more/arduino-esp8266-lolin-nodemcu-getting-started/) 
1. [Wifi Manager library](https://randomnerdtutorials.com/wifimanager-with-esp8266-autoconnect-custom-parameter-and-manage-your-ssid-and-password/) is a helpful tool to change password when WiFi network has changed.
1. [ArduinoWebsockets](https://github.com/gilmaimon/ArduinoWebsockets) library from [Gil Maimon](https://github.com/gilmaimon), became my favorite library. Gil M. was actively seeking for open issues about his library. He released [a patch for Toggl's websocket compatibility](https://github.com/gilmaimon/ArduinoWebsockets/issues/67) by adding a request parameter. This problem was the most time consuming task in this project.


# Project To-Do's

- ~~**Initial Setups**~~
- ~~**Manage Button States**~~
- ~~**Manage Wifi credentials**~~
- ~~**Toggl Socket research**~~
- ~~**Add Toggl's Socket as state management**~~
- ~~**Starting time entries from button**~~
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
