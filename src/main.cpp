//////////////////////////////////////////////////////////////
// Note: uncomment the following line to enable integration testing
// This will include an hpp file for testing purposes
// Be sure to comment out this line for production builds
//////////////////////////////////////////////////////////////
//#define INTEGRATION_TESTING

#ifdef INTEGRATION_TESTING
#include "../integration/foc_motor_standalone_test.hpp" // Testing file to run

#else
#include <Arduino.h>
#include <memory>
#include "HydraFOCConfig.h"
#include "actuators/EncoderMotor.h"
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

const float positionGain = 1.0f;

EncoderMotor motors[NUM_FOC_MOTORS] = {
    EncoderMotor(
        focMotorPins[0][0], focMotorPins[0][1], focMotorPins[0][2],
        focMotorPins[0][3], focMotorPins[0][4], focMotorPins[0][5],
        I2C0_SDA, focCurrentPins[0][0], focCurrentPins[0][1]),
    EncoderMotor(
        focMotorPins[1][0], focMotorPins[1][1], focMotorPins[1][2],
        focMotorPins[1][3], focMotorPins[1][4], focMotorPins[1][5],
        I2C1_SDA, focCurrentPins[1][0], focCurrentPins[1][1])
};

void setup() {
    // Initialize built-in LED pin
    pinMode(2, OUTPUT);

    // Initialize serial debug output
    Serial.begin(SERIAL_BAUD);
    delay(2000);
    Serial.println("[Motor Driver] UART Slave Mode...");

    pinMode(focDriverSleepPin, OUTPUT);
    pinMode(focDriverResetPin, OUTPUT);
    digitalWrite(focDriverSleepPin, HIGH);
    digitalWrite(focDriverResetPin, HIGH);

    Wire.begin(I2C0_SDA, I2C0_SCL);
    Wire1.begin(I2C1_SDA, I2C1_SCL);

    for  (uint8_t i = 0; i < NUM_FOC_MOTORS; i++) {
        TwoWire* wire = (i == 0) ? &Wire : &Wire1;
        motors[i].begin(motorDirs[i], encoderElectricAngles[i], true, wire);
        motors[i].resetEncoder(encoderOffsets[i]);
    }
    

    for (int i = 0; i < NUM_FOC_MOTORS; ++i) {
        motors[i].setPosition(0.0f);
    }

    delay(1000); // Wait for motor to stabilize

    // Initialize UART and protocol
    core = uart.getProtocol();

    // Handler for receiving commands
    core->setReadHandler([&](std::shared_ptr<SongbirdCore::Packet> pkt){
        if (pkt->getHeader() == AMPLITUDE_PACKET_HEADER && pkt->getPayloadLength() == 4) {
            float magnitude = pkt->readFloat();
            pkt->readFloat();

            float position = magnitude * positionGain;
            for (int i = 0; i < NUM_FOC_MOTORS; ++i) {
                motors[i].setPosition(position);
            }

            Serial.print("Received command: ");
            Serial.print("Magnitude: ");
            Serial.print(magnitude, 6);
            Serial.print(", Position: ");
            Serial.println(position, 6);

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

}

void loop() {
    for (int i = 0; i < NUM_FOC_MOTORS; ++i) {
        motors[i].update();
    }
    uart.updateData();
}
#endif
//////////////////////////////////////////////////////////////