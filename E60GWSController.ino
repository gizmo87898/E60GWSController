// Copyright (c) Sandeep Mistry. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <CAN.h>
unsigned long sinceLast100msLoop = 0;
unsigned long sinceLast200msLoop = 0;
unsigned long sinceLast1000msLoop = 0;
byte gearCounter = 0x0D;
int gear = 0; //0 = P, 1 = R, 2 = N, 3 = D, 4 = M/S

bool displayOutput = false;
void setup() {
  Serial.begin(115200);
  while (!Serial);
  CAN.setPins(25,26);
  Serial.println("BMW E60 GWS Controller");

  // start the CAN bus at 500 kbps
  if (!CAN.begin(500E3)) {
    Serial.println("Starting CAN failed!");
    while (1);
  }
}
void sendShifterAwake() {
    CAN.beginPacket(0x4DD);
    CAN.write(0);
    CAN.write(1);
    CAN.write(2);
    CAN.write(3);
    CAN.write(0);
    CAN.endPacket();
}
void nextGear() {
  sendGear();
  gear++;
  if(gear == 5) {
    gear = 0;
  }
  Serial.println(gear);
}
void sendGear() {
    switch (gear) {
      case 0: //Park
        CAN.beginPacket(0x1D2);
        CAN.write(0xE1);
        CAN.write(0x0C);
        CAN.write(0x08);
        CAN.write(gearCounter);
        CAN.write(0xCF);
        CAN.endPacket();
        gearCounter=gearCounter+0x10;
        if (gearCounter==0xED)
        {
           gearCounter=0x0D;
        }
        Serial.println("Park Sent");
        break;
     case 1: //Neutral
        CAN.beginPacket(0x1D2);
        CAN.write(0xB4);
        CAN.write(0x0C);
        CAN.write(0x08);
        CAN.write(gearCounter);
        CAN.write(0xCF);
        CAN.endPacket();
        gearCounter=gearCounter+0x10;
        if (gearCounter==0xED)
        {
           gearCounter=0x0D;
        }
        Serial.println("Neutral Sent");
        break;
     case 2: //Drive
        CAN.beginPacket(0x1D2);
        CAN.write(0x78);
        CAN.write(0x0C);
        CAN.write(0x8F);
        CAN.write(gearCounter);
        CAN.write(0xF0);
        CAN.endPacket();
        gearCounter=gearCounter+0x10;
        if (gearCounter==0xED)
        {
           gearCounter=0x0D;
        }
        Serial.println("Drive Sent");
        break;
     case 3: //Reverse
        CAN.beginPacket(0x1D2);
        CAN.write(0xD2);
        CAN.write(0x0C);
        CAN.write(0x8F);
        CAN.write(gearCounter);
        CAN.write(0xF0);
        CAN.endPacket();
        gearCounter=gearCounter+0x10;
        if (gearCounter==0xED)
        {
           gearCounter=0x0D;
        }
        Serial.println("Reverse Sent");
        break;
     case 4:
        CAN.beginPacket(0x1D2);
        CAN.write(0x78);
        CAN.write(0x5C);
        CAN.write(0x8F);
        CAN.write(gearCounter);
        CAN.write(0xF0);
        CAN.endPacket();
        gearCounter=gearCounter+0x10;
        if (gearCounter==0xED)
        {
           gearCounter=0x0D;
        }
        Serial.println("Sport Sent");
        break;
  }
 }
void loop() {
  unsigned long currentLoop = millis();
  String data;
  int packetSize = CAN.parsePacket();
  // check if data is available
  if (Serial.available() > 0) {
    // read the incoming string:
    String incomingString = Serial.readString();
    incomingString.trim();
    // prints the received data
    Serial.print("I received: \"");
    Serial.print(incomingString);
    Serial.println("\"");
    if(incomingString.equals("0")) {
      displayOutput = true;
      Serial.println("CAN messages output on.");
    }
    if(incomingString.equals("1")) {
      displayOutput = false;
      Serial.println("CAN messages output off.");
    }
    else {
      Serial.println("0 or 1 to disable or enable output of can messages");
    }
  }
  
  if (packetSize) {
    
    while (CAN.available()) {
      data.concat(String(CAN.read(), HEX));
    }
    if(displayOutput == true) {
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
      }  
      else {
        Serial.print(" len: ");
        Serial.print(packetSize);
        Serial.print("  data: ");
        // only print packet data for non-RTR packets
        Serial.println(data);
      }
    }

    if(String(CAN.packetId(), HEX) == "198") {
      if(String(data[6]) == "5") {
        Serial.println("Park button pressed");
      }
      if(String(data[5]) == "9") {
        Serial.println("Side Button Pressed");
      }
      if(String(data[2]) == "1") {
        Serial.println("Forward 1");
      }
      if(String(data[2]) == "2") {
        Serial.println("Forward 2");
      }
      if(String(data[2]) == "3") {
        Serial.println("Backward 1");
      }
      if(String(data[2]) == "4") {
        Serial.println("Backward 2");
      }
      if(String(data[2]) == "7") {
        Serial.println("Sport Mode Selected");
      }
      if(String(data[2]) == "5") {
        Serial.println("Downshift");
      }
      if(String(data[2]) == "6") {
        Serial.println("Upshift");
      }
    }
    }
    
  // send packet: id is 11 bits, packet can contain up to 8 bytes of data
    if (currentLoop - sinceLast100msLoop > 100) {
      sinceLast100msLoop = currentLoop;
      sendShifterAwake();
    }
    if (currentLoop - sinceLast200msLoop > 200) {
      sinceLast200msLoop = currentLoop;
      sendGear();
    }
    if (currentLoop - sinceLast1000msLoop > 1000) {
      sinceLast1000msLoop = currentLoop;
      nextGear();
    }

}
