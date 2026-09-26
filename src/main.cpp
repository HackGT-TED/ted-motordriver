//////////////////////////////////////////////////////////////
// Note: uncomment the following line to enable integration testing
// This will include an hpp file for testing purposes
// Be sure to comment out this line for production builds
//////////////////////////////////////////////////////////////
#define INTEGRATION_TESTING

#ifdef INTEGRATION_TESTING
#include <SimpleFOC.h>
#include "HydraFOCMotor.h"
#include "../integration/open_loop_vibe_motor_test.hpp" // Testing file to run
#endif
//////////////////////////////////////////////////////////////