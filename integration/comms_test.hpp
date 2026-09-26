#ifndef COMMS_TEST_HPP
#define COMMS_TEST_HPP

#include <Arduino.h>
#include "SongbirdCore.h"
#include "SongbirdUART.h"

#define SERIAL_BAUD 115200
#define COMMS_BAUD 38400

//Serial node object with software serial on pins 14 (RX) and 15 (TX)
SoftwareSerial serial(14, 15);
SongbirdUART uart("UART Node", serial);
//Serial protocol object
std::shared_ptr<SongbirdCore> core;

void setup() {
    // Initialize built-in LED pin
    pinMode(2, OUTPUT);

    // Initialize debug output
    Serial.begin(SERIAL_BAUD);
    delay(2000);
    Serial.println("[Comms Test] UART Slave Test...");

    // Initialize UART and protocol
    core = uart.getProtocol();

    // Test handler
    core->setReadHandler([&](std::shared_ptr<SongbirdCore::Packet> pkt){
        if (pkt->getHeader() == 0x10 && pkt->getPayloadLength() == 4) {
            // Read float from packet payload
            float velocity = pkt->readFloat();
            Serial.print("Received velocity: ");
            Serial.println(velocity);
            //Turn on built in LED
            digitalWrite(2, HIGH);
        }
    });

    // Initialize the UART node
    if (!uart.begin(COMMS_BAUD)) {
        Serial.println("[Comms Test] Failed to initialize UART node.");
        while (true) {
            delay(1000);
        }
    }
}

void loop() {
    uart.updateData();

    // Tries to read available serial data
    /*if (serial.available() > 0) {
        // Echo received data to Serial Monitor
        while (serial.available() > 0) {
            uint8_t byteReceived = serial.read();
            Serial.write(byteReceived);
        }
    }*/
}

#endif // COMMS_TEST_HPP