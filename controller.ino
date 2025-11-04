//install arduino BLE library before staring


#include <Arduino.h>
#include <ArduinoBLE.h>

const int VRx = A0;
const int VRy = A1;
const int SW = 2;

BLEService carService("180C"); 
BLEStringCharacteristic commandChar("2A56", BLEWrite | BLENotify, 20);

void setup() {
  Serial.begin(9600);
  pinMode(SW, INPUT_PULLUP);

  if (!BLE.begin()) {
    Serial.println("Starting BLE failed!");
    while (1);
  }

  BLE.setLocalName("CarController");
  BLE.setAdvertisedService(carService);
  carService.addCharacteristic(commandChar);
  BLE.addService(carService);
  BLE.advertise();

  Serial.println("BLE Joystick Controller Ready");
}

void loop() {
  BLEDevice central = BLE.central();

  if (central) {
    Serial.print("Connected to: ");
    Serial.println(central.address());

    while (central.connected()) {
      int xValue = analogRead(VRx);
      int yValue = analogRead(VRy);
      bool pressed = !digitalRead(SW);

      // Center value around 512 and set thresholds
      String command = "5"; // default stop
      if (yValue > 700) command = "2";       // forward
      else if (yValue < 300) command = "8";  // backward
      else if (xValue < 300) command = "4";  // left
      else if (xValue > 700) command = "6";  // right

      if (pressed) command = "5"; // stop when pressed

      commandChar.writeValue(command);
      Serial.print("Sent: ");
      Serial.println(command);
      delay(100);
    }

    Serial.println("Disconnected");
  }
}
