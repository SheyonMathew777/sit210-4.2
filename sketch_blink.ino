#include <Arduino.h>

// Task 4.2C - Multiple Interrupts Implementation
// Using Arduino Nano 33 IoT with slide switch and HC-SR04 ultrasonic sensor

// Pin Definitions
const int LED1_PIN = 10;    // LED controlled by switch
const int LED2_PIN = 11;    // LED controlled by sensor
const int SWITCH_PIN = 2;   // Slide switch with internal pull-up
const int TRIG_PIN = 4;     // HC-SR04 trigger pin
const int ECHO_PIN = 3;     // HC-SR04 echo pin (with voltage divider)

// Variables
volatile bool led1State = false;  // State of LED1
volatile bool switchPressed = false;  // Switch state
unsigned long lastBlinkTime = 0;  // Last LED blink time
const unsigned long BLINK_INTERVAL = 500;  // Blink interval in milliseconds

// Constants
const int DISTANCE_THRESHOLD = 20;  // Distance threshold in cm
const unsigned long DEBOUNCE_TIME = 200;  // Debounce time in milliseconds
volatile unsigned long lastSwitchTime = 0;  // Last switch press time

// LED2 and object detection state
bool objectDetected = false;
bool led2State = false;

void setup() {
  // Initialize Serial communication
  Serial.begin(9600);
  while (!Serial) {
    ; // Wait for Serial port to connect
  }
  Serial.println("Task 4.2C - Multiple Interrupts Started");

  // Configure pins
  pinMode(LED1_PIN, OUTPUT);
  pinMode(LED2_PIN, OUTPUT);
  pinMode(SWITCH_PIN, INPUT_PULLUP);  // Enable internal pull-up
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  // Initialize LEDs to OFF
  digitalWrite(LED1_PIN, LOW);
  digitalWrite(LED2_PIN, LOW);

  // Attach interrupt only for the switch
  attachInterrupt(digitalPinToInterrupt(SWITCH_PIN), switchISR, FALLING);
}

void loop() {
  // Handle switch debouncing
  if (switchPressed && (millis() - lastSwitchTime > DEBOUNCE_TIME)) {
    switchPressed = false;
    led1State = !led1State;
    digitalWrite(LED1_PIN, led1State);
    Serial.println("Switch pressed - LED1 toggled");
  }

  // Trigger ultrasonic sensor and check for object
  bool detected = triggerUltrasonic();
  if (detected != objectDetected) {
    objectDetected = detected;
    Serial.println(objectDetected ? "Object detected - LED2 will blink" : "Object no longer detected - LED2 will stop blinking");
  }

  // Handle LED2 blinking when object is detected
  if (objectDetected) {
    if (millis() - lastBlinkTime >= BLINK_INTERVAL) {
      led2State = !led2State;
      digitalWrite(LED2_PIN, led2State);
      lastBlinkTime = millis();
    }
  } else {
    digitalWrite(LED2_PIN, LOW);
    led2State = false;
  }
}

// Interrupt Service Routine for switch
void switchISR() {
  if (millis() - lastSwitchTime > DEBOUNCE_TIME) {
    switchPressed = true;
    lastSwitchTime = millis();
  }
}

// Function to trigger ultrasonic sensor and measure distance
bool triggerUltrasonic() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 30000); // 30ms timeout
  int distance = duration * 0.034 / 2;
  return (distance < DISTANCE_THRESHOLD && distance > 0);
}
