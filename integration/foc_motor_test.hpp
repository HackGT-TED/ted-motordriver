#ifndef FOC_MOTOR_TEST_HPP
#define FOC_MOTOR_TEST_HPP

#include <Arduino.h>
#include "HydraFOCMotor.h"
#include "HydraFOCConfig.h"

#define MOTOR_PORT 0

// Loop counter
unsigned long loopCounter = 0;

// HydraFOC motor object
HydraFOCMotor motor(focMotorPins[MOTOR_PORT][0], focMotorPins[MOTOR_PORT][1], focMotorPins[MOTOR_PORT][2], focMotorPins[MOTOR_PORT][3], focMotorPins[MOTOR_PORT][4], focMotorPins[MOTOR_PORT][5], I2C1_SDA, focCurrentPins[MOTOR_PORT][0], focCurrentPins[MOTOR_PORT][1]);

void setup() {
    Serial.begin(SERIAL_BAUD_RATE);
    // Configure I2C
    if (MOTOR_PORT == 0) {
        Wire.begin(I2C0_SDA, I2C0_SCL);
    } else if (MOTOR_PORT == 1) {
        Wire.begin(I2C1_SDA, I2C1_SCL);
    } else {
        Serial.println("Invalid MOTOR_PORT defined. Please set to 0 or 1.");
        while (true); // Halt execution
    }

    // Configure driver pins
    pinMode(focDriverSleepPin, OUTPUT);
    pinMode(focDriverResetPin, OUTPUT);
    digitalWrite(focDriverSleepPin, HIGH); // Wake up driver
    digitalWrite(focDriverResetPin, HIGH); // Release reset

    // Initialize HydraFOC motor
    motor.begin(Direction::CW, 1.66f, true);
    motor.resetEncoder();

    delay(1000); // Wait for motor to stabilize
    Serial.println("FOC Motor Test Initialized.");

    // Set target position
    motor.setPosition(0.f);
}

void loop() {
    // Run FOC control loop
    motor.update();

    // Motor variable monitoring
    //motor.monitor();

    // Prints current sensing readings
    /*Serial.print("Current A: ");
    Serial.print(analogRead(focCurrentPins[0][0]));
    Serial.print(" | Current B: ");
    Serial.println(analogRead(focCurrentPins[0][1]));*/

    // Prints encoder angle with full precision (every 100 loop counts)
    if (loopCounter % 100 == 0) {
        Serial.println(motor.getPosition(), 6);  // 6 decimal places
    }

    loopCounter++;
}

#endif // FOC_MOTOR_TEST_HPP