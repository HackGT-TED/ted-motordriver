#ifndef FOC_MOTOR_STANDALONE_TEST_HPP
#define FOC_MOTOR_STANDALONE_TEST_HPP

#include <Arduino.h>
#include <SimpleFOC.h>
#include "HydraFOCConfig.h"

// Motor port (0 or 1)
#define MOTOR_PORT 1

// Loop counter
unsigned long loopCounter = 0;

// Motor driver
BLDCDriver3PWM driver(focMotorPins[MOTOR_PORT][0], focMotorPins[MOTOR_PORT][1], focMotorPins[MOTOR_PORT][2], 
                      focMotorPins[MOTOR_PORT][3], focMotorPins[MOTOR_PORT][4], focMotorPins[MOTOR_PORT][5]);

// Motor object
BLDCMotor motor(11);

// Magnetic encoder
MagneticSensorI2C encoder(AS5600_I2C);

void setup() {
    // Initialize serial communication for debugging
    Serial.begin(SERIAL_BAUD_RATE);
    delay(2000);

    Serial.println("Motor test started");
    if (MOTOR_PORT == 0) {
        // Configure I2C
        Wire.begin(I2C0_SDA, I2C0_SCL);
    } else if (MOTOR_PORT == 1) {
        // Configure I2C
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

    // Initialize magnetic sensor hardware
    encoder.init();
    // MagneticSensorI2C::init() calls Wire.begin() without ESP32-S3 pin arguments.
    // Restore the motor port pins before the first sensor read.
    if (MOTOR_PORT == 0) {
        Wire.begin(I2C0_SDA, I2C0_SCL);
    } else {
        Wire.begin(I2C1_SDA, I2C1_SCL);
    }
    // Link the motor to the sensor
    motor.linkSensor(&encoder);

    // PWM frequency to be used [Hz]
    driver.pwm_frequency = 30000;
    // Power supply voltage [V]
    driver.voltage_power_supply = 12;

    driver.init();
    motor.linkDriver(&driver);

    // Maximal voltage to be set to the motor
    motor.voltage_limit = 2.8f;
    // Max current to be sent to the motor
    motor.current_limit = 1.0f;

    // choose FOC modulation (optional)
    motor.foc_modulation = FOCModulationType::SpaceVectorPWM;

    // Select control mode
    motor.controller = MotionControlType::torque;

    // Velocity PI controller parameters
    motor.PID_velocity.P = 0.2f;
    motor.PID_velocity.I = 16.f;
    motor.PID_velocity.D = 0;

    // Velocity low pass filtering time constant
    motor.LPF_velocity.Tf = 0.01f;

    // Maximal velocity of the controller
    motor.velocity_limit = 150;

    // Enable monitoring
    motor.useMonitoring(Serial);

    // Initialize motor
    motor.init();

    // Align sensor and start FOC
        if (!motor.initFOC()) {
            Serial.println("FOC initialization failed; torque output disabled.");
            while (true) {
                delay(1000);
            }
        }

    delay(1000); // Wait for motor to stabilize
    Serial.println("FOC Motor Standalone Test Initialized.");

    motor.move(0); // Move to initial position
}

void loop() {
    // Run FOC control loop
    motor.loopFOC();
    motor.move(0);

    // Motor variable monitoring
    //motor.monitor();

    // Prints encoder angle with full precision (every 100 loop counts)
    if (loopCounter % 100 == 0) {
        Serial.println(motor.shaft_angle, 6);  // 6 decimal places
    }

    loopCounter++;
}

#endif // FOC_MOTOR_STANDALONE_TEST_HPP
