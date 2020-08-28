using namespace vex;

extern brain Brain;

// VEXcode devices
extern inertial iner;
extern controller Controller1;
extern motor Motor11;
extern motor Motor20;

/**
 * Used to initialize code/tasks/devices added using tools in VEXcode Pro.
 * 
 * This should be called at the start of your int main function.
 */
void  vexcodeInit( void );