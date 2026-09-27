//////////////////////////////////////////////////////////////
// Note: uncomment the following line to enable integration testing
// This will include an hpp file for testing purposes
// Be sure to comment out this line for production builds
//////////////////////////////////////////////////////////////
//#define INTEGRATION_TESTING

#ifdef INTEGRATION_TESTING
#include "../integration/comms_test.hpp" // Testing file to run

#else
#include <Arduino.h>
#include <memory>
#include "HydraFOCConfig.h"
#include "actuators/OpenLoopVibeMotor.h"
#include "SongbirdCore.h"
#include "SongbirdUART.h"

#define SERIAL_BAUD 115200
#define COMMS_BAUD 38400
#define AMPLITUDE_PACKET_HEADER 0x10

//Serial node object with software serial on pins 14 (RX) and 15 (TX)
SoftwareSerial serial(14, 15);
SongbirdUART uart("UART Node", serial);
//Serial protocol object
std::shared_ptr<SongbirdCore> core;

// Motor port (0 or 1)
#define MOTOR_PORT 0

OpenLoopVibeMotor vibeMotor(
    focMotorPins[MOTOR_PORT][0],
    focMotorPins[MOTOR_PORT][1],
    focMotorPins[MOTOR_PORT][2],
    focMotorPins[MOTOR_PORT][3],
    focMotorPins[MOTOR_PORT][4],
    focMotorPins[MOTOR_PORT][5]);

float vibGain = 50.0f;     // Gain for the vibration signal

uint64_t vibStart = 0; // Start time for vibration signal

float getVibrationCommand(uint64_t time_ms, float vibAmplitude, float vibFreq) {
    // Gets vibration signal based on elapsed time and wave params
    float t = (float)time_ms / 1000.0f;
    return vibAmplitude * vibGain *sin(2.0f * _PI * t * vibFreq);
}

void setup() {
    // Initialize built-in LED pin
    pinMode(2, OUTPUT);

    // Initialize serial debug output
    Serial.begin(SERIAL_BAUD);
    delay(2000);
    Serial.println("[Motor Driver] UART Slave Mode...");

    // Initialize OpenLoopVibeMotor
    vibeMotor.begin();
    vibeMotor.setVelocity(0.0f);

    delay(1000); // Wait for motor to stabilize

    // Initialize UART and protocol
    core = uart.getProtocol();

    core->setReadHandler([&](std::shared_ptr<SongbirdCore::Packet> pkt){
        if (pkt->getHeader() == AMPLITUDE_PACKET_HEADER && pkt->getPayloadLength() == 8) {
            // Read float from packet payload
            float amplitude = pkt->readFloat();
            float frequency = pkt->readFloat();
            // Set motor velocity
            vibeMotor.setVelocity(getVibrationCommand(millis() - vibStart, amplitude, frequency));
            Serial.print("Received command: ");
            Serial.print("Amplitude: ");
            Serial.print(amplitude, 6);
            Serial.print(", Frequency: ");
            Serial.println(frequency, 6);

            digitalWrite(2, HIGH); // Turn on built-in LED to indicate packet received
        }
    });

    // Initialize the UART node
    if (!uart.begin(COMMS_BAUD)) {
        Serial.println("[Comms Test] Failed to initialize UART node.");
        while (true) {
            delay(1000);
        }
    }

    // Logs vibration start time
    vibStart = millis();
}

void loop() {
    // Update motor
    vibeMotor.update();
    // Update UART data
    uart.updateData();
}
#endif
//////////////////////////////////////////////////////////////