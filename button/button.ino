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
int led_state = -1;
int ws_open=0;

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
  // Serial.setDebugOutput(true); // Open for debug.
  pinMode(led_pin, OUTPUT);
  pinMode(button_pin, INPUT);
  digitalWrite(led_pin, LOW);
  setup_wifi();
  Serial.println("wifi init succeessful.");
  send_timer_request("get_current_entry");
  // Because of the workaround, each api request re-inits web socket after response
  // init_socket();
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
  Serial.println("Wifi connected");
}

// WEBSOCKET METHODS
void init_socket() {
  ws_open = 1;
  client.setInsecure();
  client.onMessage(onMessageCallback);
  client.onEvent(onEventsCallback);
  client.connect(websockets_server);
  // Returns session_id
  client.send("{\"type\": \"authenticate\", \"api_token\": \"" +  api_key + "\"}");
  client.ping();
}

void onEventsCallback(WebsocketsEvent event, String data) {
  if (event == WebsocketsEvent::ConnectionOpened) { Serial.println("Connnection Opened"); }
  else if (event == WebsocketsEvent::ConnectionClosed) { Serial.println("Connnection Closed"); }
  else if (event == WebsocketsEvent::GotPing) { client.send("{\"type\":\"pong\"}"); }
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
    // Looking for current running entry id, if update state comes with same id then turn led off
    // Toggl Websocket doesn't return an inten like "stop" so we're catching it from UPDATE intent
    // To be sure aat and stop values can be compared;
    //     "stop": "2020-06-20T13:36:18Z"
    //     "at": "2020-06-20T13:36:19.487041Z"
    // As seen above, update time and stop time is so close, just 1.5 seconds.
    // TODO: At hreshold can be determnined.
    
    if(parse_entry_id(res) == running_entry_id){

      Serial.println("led off");
      digitalWrite(led_pin, LOW);
      led_state = 0;
      
    }
  }
}

// WEB REQUESTS

void send_timer_request(String action) {
  
  Serial.println("led state");
  Serial.println(led_state);
  Serial.println("id");
  Serial.println(running_entry_id);

  if (action == "toggle") {
    if (led_state == 1) {
      if (running_entry_id != "") {
        send_api_request("/time_entries/" + running_entry_id + "/stop", "PUT", "");
        // TODO: Add 200 status control
        digitalWrite(led_pin, LOW);
        led_state = 0;
      } else { Serial.println("No running record has found"); }
    }
    
    // Start new entry
    else if (led_state < 1) {
      send_api_request("/time_entries/start", "POST", "{\"time_entry\":{\"description\":\"" + default_entry_name + "\",\"tags\":[\"" + tag + "\"],\"created_with\":\"" + client_name + "\"}}");
      // TODO: Add 200 status control
      digitalWrite(led_pin, HIGH);
      led_state = 1;
    }
  }
  else if (action == "get_current_entry") {
    String response = send_api_request("/time_entries/current", "GET", "");
    Serial.println(response);
    String entry_id = parse_entry_id(response);
    Serial.println(entry_id);
    if (entry_id != "") {
      digitalWrite(led_pin, HIGH);
      led_state = 1;
      running_entry_id = entry_id;
      Serial.println("running_entry_id: " + running_entry_id);
    } else{
      digitalWrite(led_pin, LOW);
      led_state = 0;
    }
    Serial.println("First cycle ok.");
  }
}


String parse_entry_id(String input) {
  Serial.println("DEBUG - Entry Retriever");
  Serial.println(input);
  int startchar = input.indexOf("id");
  Serial.println(startchar);
  if (startchar >= 0) {
    int endchar = input.indexOf(",", startchar);
    Serial.println(endchar);
    return input.substring(startchar + 4, endchar);
  }
  return "";
}

String send_api_request(String path, String req_method, String body) {
  // Workaround: Before sending HTTP request, Web socket will be closed
  if (ws_open == 1){
      client.close();
      ws_open = 0;
    }
  
  // Regular API call
  Serial.println("Calling " + path + " as " + req_method + " request with body: " + body);
  BearSSL::WiFiClientSecure secure_client;
  // I did not include Toggl's domain ssl fingerprintln.
  // Because fingerprintln can be added into code and it require change it when domain ssl is updated.
  // For further security and avoid MITM fingerprintln checkup can be controlled.
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
  Serial.println("http Response status code: ");
  Serial.println(status_code);
  Serial.println("http body: ");
  Serial.println(payload);
  //running_entry_id = "";
  
  // Workaround: Re-init web socket
  init_socket();
  return payload;
}

//Gray Area
// wifiManager.resetSettings();
