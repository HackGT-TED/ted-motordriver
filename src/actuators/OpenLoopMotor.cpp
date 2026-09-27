#include "OpenLoopMotor.h"

OpenLoopMotor::OpenLoopMotor(uint8_t pwmA, uint8_t pwmB, uint8_t pwmC, uint8_t enA, uint8_t enB, uint8_t enC)
    : motor(11),
      driver(pwmA, pwmB, pwmC, enA, enB, enC),
      targetVelocity(0)
{
}

void OpenLoopMotor::begin() {
    // PWM frequency to be used [Hz]
    driver.pwm_frequency = 30000;
    // Power supply voltage [V]
    driver.voltage_power_supply = 12;

    driver.init();
    motor.linkDriver(&driver);

    // Maximal voltage to be set to the motor
    motor.voltage_limit = 2.24f;
    // Max current to be sent to the motor
    motor.current_limit = 0.8f;

    // Select open-loop control before initFOC(); no sensor is linked in this test.
    motor.controller = MotionControlType::velocity_openloop;

    // Velocity PI controller parameters
    motor.PID_velocity.P = 0.2f;
    motor.PID_velocity.I = 16.f;
    motor.PID_velocity.D = 0;

    // Velocity low pass filtering time constant
    motor.LPF_velocity.Tf = 0.01f;

    // Maximal velocity of the controller
    motor.velocity_limit = 150;

    // Enable monitoring
    //motor.useMonitoring(Serial);

    // Initialize motor
    motor.init();
    
    // start FOC
    motor.initFOC();
}

void OpenLoopMotor::update() {
    motor.loopFOC();
    motor.move(targetVelocity);
}

void OpenLoopMotor::setVelocity(float velocity) {
    targetVelocity = velocity;
}

void OpenLoopMotor::monitor() {
    motor.monitor();
}