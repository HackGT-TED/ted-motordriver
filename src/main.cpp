//////////////////////////////////////////////////////////////
// Note: uncomment the following line to enable integration testing
// This will include an hpp file for testing purposes
// Be sure to comment out this line for production builds
//////////////////////////////////////////////////////////////
#define INTEGRATION_TESTING

#ifdef INTEGRATION_TESTING
#include "../integration/foc_motor_standalone_test.hpp" // Testing file to run
#endif
//////////////////////////////////////////////////////////////