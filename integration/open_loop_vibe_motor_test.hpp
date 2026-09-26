#ifndef OPEN_LOOP_VIBE_MOTOR_TEST_HPP
#define OPEN_LOOP_VIBE_MOTOR_TEST_HPP

#include <Arduino.h>
#include "HydraFOCConfig.h"
#include "OpenLoopVibeMotor.h"

// Motor port (0 or 1)
#define MOTOR_PORT 0

// Loop counter
unsigned long loopCounter = 0;

// Vibration parameters
float vibAmplitude = 100.0f;
float vibFreq = 20.0f;

// Timer
uint64_t vibStart = 0;

OpenLoopVibeMotor vibeMotor(
    focMotorPins[MOTOR_PORT][0],
    focMotorPins[MOTOR_PORT][1],
    focMotorPins[MOTOR_PORT][2],
    focMotorPins[MOTOR_PORT][3],
    focMotorPins[MOTOR_PORT][4],
    focMotorPins[MOTOR_PORT][5]);

float getVibrationCommand(uint64_t time_ms) {
    // Gets vibration signal based on elapsed time and wave params
    float t = (float)time_ms / 1000.0f;
    return vibAmplitude * sin(2.0f * _PI * t * vibFreq);
}

void setup() {
    Serial.begin(SERIAL_BAUD_RATE);
    delay(2000);

    Serial.println("OpenLoopVibeMotor test started");

    if (MOTOR_PORT >= NUM_FOC_MOTORS) {
        Serial.println("Invalid MOTOR_PORT defined. Please set to 0 or 1.");
        while (true) {
            delay(1000);
        }
    }

    // Configure driver pins
    pinMode(focDriverSleepPin, OUTPUT);
    pinMode(focDriverResetPin, OUTPUT);
    digitalWrite(focDriverSleepPin, HIGH); // Wake up driver
    digitalWrite(focDriverResetPin, HIGH); // Release reset

    // Initialize OpenLoopVibeMotor
    vibeMotor.begin();
    vibeMotor.setVelocity(0.0f);

    delay(1000); // Wait for motor to stabilize
    Serial.println("OpenLoopVibeMotor Test Initialized.");

    // Logs vibration start time
    vibStart = millis();
}

void loop() {
    // Command open-loop motor velocity with sinusoidal vibration profile
    vibeMotor.setVelocity(getVibrationCommand(millis() - vibStart));

    // Motor variable monitoring
    // vibeMotor.monitor();

    if (loopCounter % 100 == 0) {
        // Serial.println(getVibrationCommand(millis() - vibStart), 6);
    }

    loopCounter++;
}

#endif // OPEN_LOOP_VIBE_MOTOR_TEST_HPP
