#ifndef ENCODER_MOTOR_H
#define ENCODER_MOTOR_H

#include <Arduino.h>
#include <SimpleFOC.h>


// EncoderMotor: Wrapper for SimpleFOC motor and driver
class EncoderMotor {
public:
    // Construct with motor driver pins and encoder i2c port
    EncoderMotor(uint8_t pwmA, uint8_t pwmB, uint8_t pwmC, uint8_t enA, uint8_t enB, uint8_t enC, uint8_t dirPin, uint8_t current0, uint8_t current1);

    // Initialize the motor and driver
    void begin(Direction encDir, float encOffset, bool skipAlign, TwoWire* wire = &Wire);

    // Resets the encoder
    void resetEncoder();

    // Resets the encoder with a new offset
    void resetEncoder(float newOffset);

    // Set target position (rad)
    void setPosition(float position);
    
    // Update FOC loop (call in loop)
    void update();

    // Motor monitoring
    void monitor();

private:
    BLDCMotor motor;
    BLDCDriver3PWM driver;
    MagneticSensorI2C encoder;
    float targetPosition;
};

#endif