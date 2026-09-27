#include "EncoderMotor.h"

EncoderMotor::EncoderMotor(uint8_t pwmA, uint8_t pwmB, uint8_t pwmC, uint8_t enA, uint8_t enB, uint8_t enC, uint8_t dirPin, uint8_t current0, uint8_t current1)
    : motor(11),
      driver(pwmA, pwmB, pwmC, enA, enB, enC),
      encoder(AS5600_I2C),
      targetPosition(0)
{
}

void EncoderMotor::begin(Direction encDir, float encOffset, bool skipAlign, TwoWire* wire) {
    //SimpleFOCDebug::enable(&Serial);
    
    // initialise magnetic sensor hardware
    encoder.init(wire);
    // link the motor to the sensor
    motor.linkSensor(&encoder);

    // pwm frequency to be used [Hz]
    driver.pwm_frequency = 30000;
    // power supply voltage [V]
    driver.voltage_power_supply = 12;

    driver.init();
    motor.linkDriver(&driver);

    // choose FOC modulation (optional)
    motor.foc_modulation = FOCModulationType::SpaceVectorPWM;

    // Sets motion control type
    motor.controller = MotionControlType::angle;

    // default parameters in defaults.h

    // velocity PI controller parameters
    motor.PID_velocity.P = 0.1f;
    motor.PID_velocity.I = 10.f;
    motor.PID_velocity.D = 0.f;

    // maximal voltage to be set to the motor
    motor.voltage_limit = 2.8f;
    //maximal current to be sent to the motor
    motor.current_limit = 1.0f;

    // velocity low pass filtering time constant
    // the lower the less filtered
    motor.LPF_velocity.Tf = 0.01f;

    // angle P controller
    motor.P_angle.P = 20;
    // maximal velocity of the controller
    motor.velocity_limit = 150;
    
    // comment out if not needed
    //motor.useMonitoring(Serial);
    // Set monitoring variables to include q and d currents, along with velocity and angle
    //motor.monitor_variables = _MON_CURR_Q;
    //motor.monitor_downsample = 100; // default 10

    // link current sense to driver and motor BEFORE motor.init()
    // DISABLED: Current sensing interferes with external ADC usage
    //currentSense.linkDriver(&driver);

    // initialize motor
    motor.init();
    
    // initialize current sense AFTER motor.init()
    // DISABLED: Current sensing interferes with external ADC usage
    //currentSense.init();
    //motor.linkCurrentSense(&currentSense);
    
    //Calibration parameters
    motor.zero_electric_angle = encOffset;
    motor.sensor_direction = encDir;
    //currentSense.skip_align = skipAlign; // skip current sense alignment

    // align sensor and start FOC
    motor.initFOC();
}

void EncoderMotor::resetEncoder() {
    motor.sensor_offset = motor.shaft_angle;
}

void EncoderMotor::resetEncoder(float newOffset) {
    motor.sensor_offset = newOffset;
}

void EncoderMotor::setPosition(float position) {
    targetPosition = position;
}

void EncoderMotor::update() {
    motor.loopFOC();
    motor.move(targetPosition);
}

void EncoderMotor::monitor() {
    motor.monitor();
}