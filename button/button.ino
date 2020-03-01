// INCLUDES
#include <WiFiManager.h> //3rd party library
#include <ArduinoWebsockets.h> // 3rd party library - use last version
#include <ESP8266HTTPClient.h>
#include <base64.h>

using namespace websockets;

//PINS
const int led_pin = 12; // Modify if you're using different pins
const int button_pin = 14; // Modify if you're using different pins

// STATES
int previous_pressed = 0;
int pressed = 0;
int led_state = 0;

//CREDENTIALS
String api_key = "YOUR TOGGL API KEY HERE"; //Replace with your API key
String websockets_server = "wss://stream.toggl.com:443/ws";
String auth_key = "Basic " + base64::encode(api_key + ":" + "api_token");
String client_name = "toggl_button";
String running_entry_id = "";

// VARIABLES
String default_entry_name = "Toggl Button Entry";
String tag = "Toggl Button";

WebsocketsClient client;


//________________________________________________________________


////
//// SETUP & EVENTS
////
void setup() {
  Serial.begin(115200);
  // Serial.setDebugOutput(true); // Open if a problem occurring.
  pinMode(led_pin, OUTPUT);
  pinMode(button_pin, INPUT);
  digitalWrite(led_pin, LOW);
  setup_wifi();
  Serial.println("wifi init succeed.");
  init_socket();
  send_timer_request("get_current_entry");
}



////
//// LOOP
////
void loop() {
  if (client.available()) {
    client.poll();
  }
  pressed = digitalRead(button_pin);
  //Button pressed
  if (previous_pressed < pressed) {
    Serial.println("Button pressed, sending toggle command");
    send_timer_request("toggle");
  }
  previous_pressed = pressed;
  delay(200);
}


//________________________________________________________________


////
//// UTILITIES
////

// WIFI
void setup_wifi() {
  WiFiManager wifiManager;
  wifiManager.autoConnect("Setup your Toggl Button");
  Serial.print("Wifi connected");
}

// WEBSOCKET METHODS
void init_socket() {
  client.setInsecure();
  client.onMessage(onMessageCallback);
  client.onEvent(onEventsCallback);
  client.connect(websockets_server);
  // Returns session_id
  client.send("{\"type\": \"authenticate\", \"api_token\": \"" +  api_key + "\"}");
  client.ping();
}

void onEventsCallback(WebsocketsEvent event, String data) {
  if (event == WebsocketsEvent::ConnectionOpened) {
    Serial.println("Connnection Opened");
  }
  else if (event == WebsocketsEvent::ConnectionClosed) {
    Serial.println("Connnection Closed");
  }
  else if (event == WebsocketsEvent::GotPing) {
    client.send("{\"type\":\"pong\"}");
  }
}

void onMessageCallback(WebsocketsMessage message) {
  String res = message.data();
  Serial.println(res);
  if (res.indexOf("INSERT") >= 0) {
    // ACTION TO TURN LED ON
    digitalWrite(led_pin, HIGH);
    Serial.println("led on");
    led_state = 1;
  }
  else if (res.indexOf("UPDATE") >= 0) {
    // ACTION TO TURN LED OFF
    // TODO look for current running entry id, if update state comes with same id then turn led off
    Serial.println("led off");
    digitalWrite(led_pin, LOW);
    led_state = 0;
  }
}

// WEB REQUESTS

void send_timer_request(String action) {
  if (action == "toggle") {
    if (led_state == 1) {
      String entry_id = get_current_entry_id_from_Toggl();
      if (entry_id != "") {
        send_toggl_request("/time_entries/" + entry_id + "/stop", "PUT", "");
      }
      else {
        Serial.println("No running record has found");
      }
    }
    // Start new entry
    else if (led_state == 0) {
      send_toggl_request("/time_entries/start", "POST", "{\"time_entry\":{\"description\":\"" + default_entry_name + "\",\"tags\":[\"" + tag + "\"],\"created_with\":\"" + client_name + "\"}}");
    }
  }
  else if (action == "get_current_entry") {
    String entry_id = get_current_entry_id_from_Toggl();
    if (entry_id != "") {
      digitalWrite(led_pin, HIGH);
      led_state = 1;
      running_entry_id = entry_id;
    }
  }
}

String send_toggl_request(String path, String req_method, String body) {
  Serial.println("Calling " + path + " as " + req_method + " request with body: " + body);
  BearSSL::WiFiClientSecure secure_client;
  // I did not include Toggl's domain ssl fingerprint.
  // Because fingerprint can be added into code and it require change it when domain ssl is updated.
  // For further security and avoid MITM fingerprint checkup can be controlled.
  secure_client.setInsecure();
  HTTPClient req;

  req.begin(secure_client, "https://www.toggl.com/api/v8" + path);

  req.addHeader("Accept", "*/*");
  req.addHeader("Content-Type", "application/json");
  req.addHeader("Authorization", auth_key);
  int status_code;
  if (req_method == "POST") { status_code = req.POST(body); }
  else if (req_method == "GET") { status_code = req.GET(); }
  else if (req_method == "PUT") { status_code = req.PUT(body); }
  String payload = req.getString();
  req.end();
  secure_client.stop();
  Serial.print("Response status code: ");
  Serial.print(status_code);
  Serial.print(" - body: ");
  Serial.print(payload);
  running_entry_id = "";
  return payload;
}

String get_current_entry_id_from_Toggl() {
  String entry_id = send_toggl_request("/time_entries/current", "GET", "");
  int startchar = entry_id.indexOf("\"id\": ");
  if (startchar >= 0) {
    int endchar = entry_id.indexOf(",", startchar);
    return entry_id.substring(startchar, endchar);
  }
  return "";
}


// TODO REMOVE LED TOGGLE CODE BASE

void switch_led() {
  if (led_state == 0) {
    digitalWrite(led_pin, HIGH);
    led_state = 1;
  }
  else if (led_state == 1) {
    digitalWrite(led_pin, LOW);
    led_state = 0;
  }
}

//Gray Area
// wifiManager.resetSettings();
