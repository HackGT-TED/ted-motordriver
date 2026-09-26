//////////////////////////////////////////////////////////////
// Note: uncomment the following line to enable integration testing
// This will include an hpp file for testing purposes
// Be sure to comment out this line for production builds
//////////////////////////////////////////////////////////////
//#define INTEGRATION_TESTING

#ifdef INTEGRATION_TESTING
#include <SimpleFOC.h>
#include "HydraFOCMotor.h"
#include "../integration/open_loop_vibe_motor_test.hpp" // Testing file to run
#endif
//////////////////////////////////////////////////////////////

#include <Arduino.h>
#include <memory>
#include "HydraFOCConfig.h"
#include "OpenLoopVibeMotor.h"
#include "SongbirdCore.h"
#include "SongbirdUART.h"

#define SERIAL_BAUD 115200

//Serial node object
SongbirdUART uart("UART Node");
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

void setup() {
    // Initialize OpenLoopVibeMotor
    vibeMotor.begin();
    vibeMotor.setVelocity(0.0f);

    delay(1000); // Wait for motor to stabilize

    // Initialize UART and protocol
    core = uart.getProtocol();

    core->setReadHandler([&](std::shared_ptr<SongbirdCore::Packet> pkt){
        if (pkt->getHeader() == 0x10 && pkt->getPayloadLength() == 4) {
        // Read float from packet payload
        float velocity = pkt->readFloat();
        // Set motor velocity
        vibeMotor.setVelocity(velocity);
        }
    });

    uart.begin(SERIAL_BAUD);
}

void loop() {
    // Update motor
    vibeMotor.update();
    // Update UART data
    uart.updateData();
}