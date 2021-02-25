#include "vex.h"

using namespace vex;

// A global instance of brain used for printing to the V5 brain screen
brain Brain;

sonar RangeFinderA = sonar(Brain.ThreeWirePort.C);

// // VEXcode generated functions
// // define variable for remote controller enable/disable
// bool RemoteControlCodeEnabled = true;
// // define variables used for controlling motors based on controller inputs
// bool DrivetrainLNeedsToBeStopped_Controller1 = true;
// bool DrivetrainRNeedsToBeStopped_Controller1 = true;

// // define a task that will handle monitoring inputs from Controller1
// int rc_auto_loop_function_Controller1() {
//   // process the controller input every 20 milliseconds
//   // update the motors based on the input values
//   while(true) {
    
//     // if(Controller1.ButtonL1.pressing() || Controller1.ButtonL2.pressing()){
//     //   arm.spin(forward, Controller1.ButtonL1.pressing() - Controller1.ButtonL2.pressing(), percent);
//     // } else{
//     //   arm.stop(hold);
//     // }
    
//     // intake_fwd.spin(forward, Controller1.ButtonR1.pressing() - Controller1.ButtonR2.pressing(), percent);

//     if(RemoteControlCodeEnabled) {
//       // calculate the drivetrain motor velocities from the controller joystick axies
//       // left = Axis3
//       // right = Axis2
//       int drivetrainLeftSideSpeed = Controller1.Axis3.position();
//       int drivetrainRightSideSpeed = Controller1.Axis2.position();
      
//       // check if the value is inside of the deadband range
//       if (drivetrainLeftSideSpeed < 5 && drivetrainLeftSideSpeed > -5) {
//         // check if the left motor has already been stopped
//         if (DrivetrainLNeedsToBeStopped_Controller1) {
//           // stop the left drive motor
//           drive_left.stop();
//           // tell the code that the left motor has been stopped
//           DrivetrainLNeedsToBeStopped_Controller1 = false;
//         }
//       } else {
//         // reset the toggle so that the deadband code knows to stop the left motor next time the input is in the deadband range
//         DrivetrainLNeedsToBeStopped_Controller1 = true;
//       }
//       // check if the value is inside of the deadband range
//       if (drivetrainRightSideSpeed < 5 && drivetrainRightSideSpeed > -5) {
//         // check if the right motor has already been stopped
//         if (DrivetrainRNeedsToBeStopped_Controller1) {
//           // stop the right drive motor
//           drive_right.stop();
//           // tell the code that the right motor has been stopped
//           DrivetrainRNeedsToBeStopped_Controller1 = false;
//         }
//       } else {
//         // reset the toggle so that the deadband code knows to stop the right motor next time the input is in the deadband range
//         DrivetrainRNeedsToBeStopped_Controller1 = true;
//       }
      
//       // only tell the left drive motor to spin if the values are not in the deadband range
//       if (DrivetrainLNeedsToBeStopped_Controller1) {
//         drive_left.setVelocity(drivetrainLeftSideSpeed, percent);
//         drive_left.spin(forward);
//       }
//       // only tell the right drive motor to spin if the values are not in the deadband range
//       if (DrivetrainRNeedsToBeStopped_Controller1) {
//         drive_right.setVelocity(drivetrainRightSideSpeed, percent);
//         drive_right.spin(forward);
//       }
//     }
//     // wait before repeating the process
//     wait(20, msec);
//   }
//   return 0;
// }

/**
 * Used to initialize code/tasks/devices added using tools in VEXcode Pro.
 * 
 * This should be called at the start of your int main function.
 */
void vexcodeInit( void ) {
  // task rc_auto_loop_task_Controller1(rc_auto_loop_function_Controller1);
}