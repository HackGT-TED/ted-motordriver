#ifndef OPEN_LOOP_VIBE_MOTOR_H
#define OPEN_LOOP_VIBE_MOTOR_H

#include <Arduino.h>
#include <SimpleFOC.h>

// OpenLoopVibeMotor: Wrapper for SimpleFOC motor and driver
class OpenLoopVibeMotor {
public:
    // Construct with motor driver pins and encoder i2c port
    OpenLoopVibeMotor(uint8_t pwmA, uint8_t pwmB, uint8_t pwmC, uint8_t enA, uint8_t enB, uint8_t enC);

    // Initialize the motor and driver
    void begin();

    // Set target velocity (rad/s)
    void setVelocity(float velocity);
    
    // Update FOC loop (call in loop)
    void update();

    // Motor monitoring
    void monitor();

private:
    BLDCMotor motor;
    BLDCDriver3PWM driver;
};

#endif