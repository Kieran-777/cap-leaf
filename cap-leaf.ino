#include <Wire.h>
#include <Adafruit_MPR121.h>
#include "MPR121Button.h"

Adafruit_MPR121 cap = Adafruit_MPR121();

MPR121Button button0(cap, 0);
MPR121Button button1(cap, 1);
MPR121Button button2(cap, 2);
MPR121Button button3(cap, 3);
MPR121Button button4(cap, 4);

MPR121Button* buttons[5] = { &button0, &button1, &button2, &button3, &button4 };
const int ledPins[5] = {3, 4, 5, 6, 7};

void scanI2C() {
  Serial.println("\nStarting I2C scan...");
  int devicesFound = 0;

  for (byte address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    byte error = Wire.endTransmission();

    if (error == 0) {
      Serial.print("I2C device found at 0x");
      if (address < 16) Serial.print("0");
      Serial.print(address, HEX);
      Serial.println("  !");

      devicesFound++;
    } else if (error == 4) {
      Serial.print("Unknown error at address 0x");
      if (address < 16) Serial.print("0");
      Serial.println(address, HEX);
    }
    delay(10);
  }

  if (devicesFound == 0) {
    Serial.println("No I2C devices found.");
  } else {
    Serial.print("Total I2C devices found: ");
    Serial.println(devicesFound);
  }
  Serial.println("I2C scan complete.\n");
}

void setup() {
  Serial.begin(9600);
  Wire.begin();

  scanI2C();

  // Try to initialize MPR121 sensor at 0x5A
  Serial.print("Initializing MPR121 at 0x5A... ");
  if (!cap.begin(0x5A)) {
    Serial.println("Failed!");
    Serial.println("Check wiring, sensor address, and power.");
    while (1);
  }
  Serial.println("Success!");

  // Setup LEDs
  for (int i = 0; i < 5; i++) {
    pinMode(ledPins[i], OUTPUT);
    digitalWrite(ledPins[i], LOW);
  }
  cap.setThresholds(12, 6);
}

void loop() {
  uint16_t touched = cap.touched();

  Serial.print("Touched bitmask: 0x");
  Serial.println(touched, HEX);

  for (int i = 0; i < 5; i++) {
    buttons[i]->update(touched);
    if (buttons[i]->isPressed()) {
      digitalWrite(ledPins[i], HIGH);
      Serial.print("Metal ");
      Serial.print(i);
      Serial.println(" touched.");
    } else {
      digitalWrite(ledPins[i], LOW);
    }
  }

  for (int i = 0; i < 5; i++) {
    Serial.print("Electrode ");
    Serial.print(i);
    Serial.print(": filtered=");
    Serial.print(cap.filteredData(i));
    Serial.print(" baseline=");
    Serial.println(cap.baselineData(i));
  }
  delay(200);
}
