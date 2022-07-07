// Copyright (c) Sandeep Mistry. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <CAN.h>
#include <Arduino.h>
#include <BleGamepad.h>

BleGamepad bleGamepad;

unsigned long sinceLast100msLoop = 0;
unsigned long sinceLast200msLoop = 0;
unsigned long sinceLast1000msLoop = 0;
unsigned long sinceLast5sLoop = 0;
byte gearCounter = 0x0D;
int gear = 0; //0 = P, 1 = R, 2 = N, 3 = D, 4 = M
bool upshifting = false;
bool downshifting = false;
bool displayOutput = false;
void setup() {
  Serial.begin(115200);
  bleGamepad.begin();
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
    Serial.println("Shifter Awake Sent");
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
     case 1: //Reverse
        
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
     case 2: //Neutral
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
     case 3: //Drive
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
  bool sideButton = false;
  if (bleGamepad.isConnected())
    {
        
        
        switch(gear) {
          case 0:
            bleGamepad.press(BUTTON_1);
            Serial.println("Button 1 Down");
            bleGamepad.release(BUTTON_1);
            break;
          case 1:
            bleGamepad.press(BUTTON_2);
            Serial.println("Button 2 Down");
            bleGamepad.release(BUTTON_2);
            break;
          case 2: 
            bleGamepad.press(BUTTON_3);
            Serial.println("Button 3 Down");
            bleGamepad.release(BUTTON_3);
            break;
          case 3:
            bleGamepad.press(BUTTON_4);
            Serial.println("Button 4 Down");
            bleGamepad.release(BUTTON_4);
            break;
          case 4:
            bleGamepad.press(BUTTON_5);
            Serial.println("Button 5 Down");
            bleGamepad.release(BUTTON_5);
            break;
        }
        if(sideButton == true) {
          bleGamepad.press(BUTTON_6);
          Serial.println("Button 6 Down");
          bleGamepad.release(BUTTON_6);
        }
        if(upshifting == true) {
          bleGamepad.press(BUTTON_7);
          Serial.println("Button 7 Down");
          bleGamepad.release(BUTTON_7);
        }
        if(downshifting == true) {
          bleGamepad.press(BUTTON_8);
          Serial.println("Button 8 Down");
          bleGamepad.release(BUTTON_8);
        }
        
    }
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
        gear = 0;
      }
      if(String(data[5]) == "9") {
        Serial.println("Side Button Pressed");
        sideButton = true;
      }
      if(String(data[2]) == "1") {
        Serial.println("Forward 1");
        switch(gear) {
          case 0: //p
            gear = 2;
            break;
          case 1: //r
            gear = 1;
            Serial.println("Shifter in R but up recieved?");
            break;
          case 2: //n
            if(sideButton == true) {
              gear = 1;
            }
            else {
              gear = 2;
            }

            break;
          case 3: //d
            gear = 2;
            break;
          case 4: //s
            Serial.println("Shifter in Sport but up recieved?");
            break;
        }
        
      }
      if(String(data[2]) == "2") {
        Serial.println("Forward 2");
        switch(gear) {
          case 0: //p
            gear = 1;
            break;
          case 1: //r
            gear = 1;
            Serial.println("Shifter in R but up 2 recieved?");
            break;
          case 2: //n
            gear = 1;
            break;
          case 3: //d
            gear = 2;
            break;
          case 4: //s
            Serial.println("Shifter in Sport but up recieved?");
            break;
        }
      }
      if(String(data[2]) == "3") {
        Serial.println("Backward 1");
        
        switch(gear) {
          case 0: //p
            gear = 3;
            break;
          case 1: //r
            gear = 2;
            break;
          case 2: //n
            gear = 2;
            break;
          case 3: //d
            //do nothing
            break;
          case 4: //s
            Serial.println("Shifter in Sport but back 1 recieved?");
            break;
        }
      }
      if(String(data[2]) == "4") {
        Serial.println("Backward 2");
        switch(gear) {
          case 0: //p
            gear = 3;
            break;
          case 1: //r
            gear = 2;
            break;
          case 2: //n
            gear = 3;
            break;
          case 3: //d
            //do nothing
            break;
          case 4: //s
            Serial.println("Shifter in Sport but back 2 recieved?");
            break;
        }
      }
      if(String(data[2]) == "7") {
        Serial.println("Sport Mode Selected");
        gear = 4;
      }
      if(String(data[2]) == "5") {
        Serial.println("Downshift");
        downshifting = true;
      }
      else {
        downshifting = false;
      }
      if(String(data[2]) == "6") {
        Serial.println("Upshift");
      }
      else {
        upshifting = false;
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

    }
    if (currentLoop - sinceLast5sLoop > 5000) {
      sinceLast5sLoop = currentLoop;

    }

}
