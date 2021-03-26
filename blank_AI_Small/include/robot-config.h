using namespace vex;

extern brain Brain;
extern controller Controller1;
extern motor drive_left;
extern motor drive_right;
extern drivetrain Drivetrain;
extern motor arm_left;
extern motor arm_right;
extern motor_group arm;
extern motor intake_fwd;
extern motor camera_motor;
extern sonar RangeFinder1;
extern limit LimitSwitch1;


/**
 * Used to initialize code/tasks/devices added using tools in VEXcode Pro.
 *
 * This should be called at the start of your int main function.
 */
void vexcodeInit(void);
