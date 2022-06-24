// Copyright (c) Sandeep Mistry. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <CAN.h>
unsigned long sinceLastShifterAwakeLoop = 0;
void setup() {
  Serial.begin(9600);
  while (!Serial);
  CAN.setPins(25,26);
  Serial.println("CAN Sender");

  // start the CAN bus at 500 kbps
  if (!CAN.begin(500E3)) {
    Serial.println("Starting CAN failed!");
    while (1);
  }
}

void loop() {
  unsigned long currentLoop = millis();
  int packetSize = CAN.parsePacket();


  if (packetSize) {
    // received a packet
    Serial.print("R: ");

    if (CAN.packetExtended()) {
      Serial.print("extended ");
    }

    if (CAN.packetRtr()) {
      // Remote transmission request, packet contains no data
      Serial.print("RTR ");
    }

    Serial.print("id: 0x");
    Serial.print(CAN.packetId(), HEX);

    if (CAN.packetRtr()) {
      Serial.print(" and requested length ");
      Serial.println(CAN.packetDlc());
    } else {
      Serial.print(" len: ");
      Serial.print(packetSize);
      Serial.print("  data: ");
      // only print packet data for non-RTR packets
      while (CAN.available()) {
        Serial.print(CAN.read(), HEX);
        Serial.print(" ");
      }
      Serial.println();

    }
   }
  // send packet: id is 11 bits, packet can contain up to 8 bytes of data

  CAN.beginPacket(0x4DD);
  CAN.write(0);
  CAN.write(1);
  CAN.write(2);
  CAN.write(3);
  CAN.write(0);
  CAN.endPacket();


  delay(100);
}
