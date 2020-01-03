const int led_pin = 16;
const int button_pin = 14;

int previous_pressed = 0;
int pressed = 0;
int led_state = 0;

void setup() {
  pinMode(led_pin, OUTPUT);
  pinMode(button_pin, INPUT);
}

void loop() {
  pressed = digitalRead(button_pin);
  if (previous_pressed < pressed) {
    switch_led();
  }
  previous_pressed = pressed;
  delay(20);
}

void switch_led(){
  if (led_state == 0) {
    digitalWrite(led_pin, LOW); // Built-in led are working reverse
    led_state = 1;
    }
    else if (led_state == 1){
    digitalWrite(led_pin, HIGH);
    led_state = 0;
    }
  }
