/*----------------------------------------------------------------------------*/
/*                                                                            */
/*    Module:       main.cpp                                                  */
/*    Author:       james                                                     */
/*    Created:      Mon Aug 31 2020                                           */
/*    Description:  V5 project                                                */
/*                                                                            */
/*----------------------------------------------------------------------------*/

// ---- START VEXCODE CONFIGURED DEVICES ----
// ---- END VEXCODE CONFIGURED DEVICES ----

#include "vex.h"

using namespace vex;

// A global instance of competition
competition Competition;

// create instance of jetson class to receive location and other
// data from the Jetson nano
//
ai::jetson  jetson_comms;

/*----------------------------------------------------------------------------*/
// Create a robot_link on PORT1 using the unique name robot_32456_1
// The unique name should probably incorporate the team number
// and be at least 12 characters so as to generate a good hash
//
// The Demo is symetrical, we send the same data and display the same status on both
// manager and worker robots
// Comment out the following definition to build for the worker robot
// #define  MANAGER_ROBOT    1

#if defined(MANAGER_ROBOT)
#pragma message("building for the manager")
ai::robot_link       link( PORT16, "robot_32456_1", linkType::manager );
#else
#pragma message("building for the worker")
ai::robot_link       link( PORT10, "robot_32456_1", linkType::worker );
#endif

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*                          Auto_Isolation Task                              */
/*                                                                           */
/*  This task is used to control your robot during the autonomous isolation  */
/*  phase of a VEX AI Competition.                                           */
/*                                                                           */
/*  You must modify the code to add your own robot specific commands here.   */
/*---------------------------------------------------------------------------*/

void auto_Isolation(void) {
  // ..........................................................................
  // Insert autonomous user code here.
  // ..........................................................................
}


/*---------------------------------------------------------------------------*/
/*                                                                           */
/*                        Auto_Interaction Task                              */
/*                                                                           */
/*  This task is used to control your robot during the autonomous interaction*/
/*  phase of a VEX AI Competition.                                           */
/*                                                                           */
/*  You must modify the code to add your own robot specific commands here.   */
/*---------------------------------------------------------------------------*/


void auto_Interaction(void) {
  // ..........................................................................
  // Insert autonomous user code here.
  // ..........................................................................
}


/*---------------------------------------------------------------------------*/
/*                                                                           */
/*                          AutonomousMain Task                              */
/*                                                                           */
/*  This task is used to control your robot during the autonomous phase of   */
/*  a VEX Competition.                                                       */
/*                                                                           */
/*---------------------------------------------------------------------------*/

bool firstAutoFlag = true;

void autonomousMain(void) {
  // ..........................................................................
  // The first time we enter this function we will launch our Isolation routine
  // When the field goes disabled after the isolation period this task will die
  // When the field goes enabled for the second time this task will start again
  // and we will enter the interaction period. 
  // ..........................................................................

  if(firstAutoFlag)
    auto_Isolation();
  else 
    auto_Interaction();

  firstAutoFlag = false;
}


/*----------------------------------------------------------------------------*/

motor drive_left = motor(PORT12, ratio18_1, false);
motor drive_right = motor(PORT13, ratio18_1, false);
motor arm_left = motor(PORT2, ratio18_1, false);
motor arm_right = motor(PORT20, ratio18_1, true);
motor intake_fwd = motor(PORT3, ratio18_1, false);

motor_group arm = motor_group(arm_left, arm_right);

drivetrain Drivetrain = drivetrain(drive_left, drive_right, 319.19, 295, 40, mm, 1);
controller Controller1 = controller(primary);

int main() {
    // Initializing Robot Configuration. DO NOT REMOVE!
    vexcodeInit();

    // local storage for latest data from the Jetson Nano
    static MAP_RECORD       local_map;

    // RUn at about 15Hz
    int32_t loop_time = 66;

    // start the status update display
    thread t1(dashboardTask);

    // Set up callbacks for autonomous and driver control periods.
    Competition.autonomous(autonomousMain);

    // print through the controller to the terminal (vexos 1.0.12 is needed)
    // As USB is tied up with Jetson communications we cannot use
    // printf for debug.  If the controller is connected
    // then this can be used as a direct connection to USB on the controller
    // when using VEXcode.
    
    FILE *fp = fopen("/dev/serial2","wb");
    

    while(1) {
        
        drive_left.spin(forward, Controller1.Axis3.value(), percent);
        drive_right.spin(forward, Controller1.Axis2.value(), percent);

        if(Controller1.ButtonL1.pressing() || Controller1.ButtonL2.pressing()){
          arm.spin(forward, Controller1.ButtonL1.pressing() - Controller1.ButtonL2.pressing(), percent);
        } else{
          arm.stop(hold);
        }
        
        intake_fwd.spin(forward, Controller1.ButtonR1.pressing() - Controller1.ButtonR2.pressing(), percent);

        // get last map data
        jetson_comms.get_data( &local_map );

        // set our location to be sent to partner robot
        link.set_remote_location( local_map.pos.x, local_map.pos.y, local_map.pos.az );

        // fprintf(fp, "Start: %ld :End\n", local_map.boxobj[MAX_OBJECT-1].size());

        // fprintf(fp, "(%ld, %ld), (%ld, %ld), %ld, %f, %f",  local_map.boxobj[MAX_OBJECT-1].x, local_map.boxobj[MAX_OBJECT-1].y, local_map.boxobj[MAX_OBJECT-1].width, local_map.boxobj[MAX_OBJECT-1].height, local_map.boxobj[MAX_OBJECT-1].classID, local_map.boxobj[MAX_OBJECT-1].depth, local_map.boxobj[MAX_OBJECT-1].prob);
        // fprintf(fp, "START: %f, %ld, (%ld, %ld), %ld :END\n",  local_map.pos.y, local_map.mapnum, local_map.boxobj[MAX_OBJECT-1].x, local_map.boxobj[MAX_OBJECT-1].y, local_map.boxobj[MAX_OBJECT-1].classID);

        // fprintf(fp, "%.2f %.2f %.2f\n", local_map.pos.x, local_map.pos.y, local_map.pos.az  );
        // fprintf(fp, "%ld, %ld\n %ld, %ld, (%f, %f, %f), %f, %f, %f\n (%ld, %ld), (%ld, %ld), %ld, %f, %f,\n %ld, %ld, %f, %f, %f", local_map.boxnum, local_map.mapnum, local_map.pos.framecnt, local_map.pos.status, local_map.pos.x, local_map.pos.y, local_map.pos.z, local_map.pos.az, local_map.pos.el, local_map.pos.rot, local_map.boxobj[MAX_OBJECT-1].x, local_map.boxobj[MAX_OBJECT-1].y, local_map.boxobj[MAX_OBJECT-1].width, local_map.boxobj[MAX_OBJECT-1].height, local_map.boxobj[MAX_OBJECT-1].classID, local_map.boxobj[MAX_OBJECT-1].depth, local_map.boxobj[MAX_OBJECT-1].prob, local_map.mapobj[MAX_OBJECT-1].age, local_map.mapobj[MAX_OBJECT-1].classID, local_map.mapobj[MAX_OBJECT-1].positionX, local_map.mapobj[MAX_OBJECT-1].positionY, local_map.mapobj[MAX_OBJECT-1].positionZ);
        // fprintf(fp, "Start: %ld, %ld, (%f, %f, %f), %f, %f, %f :End\n", local_map.pos.framecnt, local_map.pos.status, local_map.pos.x, local_map.pos.y, local_map.pos.z, local_map.pos.az, local_map.pos.el, local_map.pos.rot);
        // fprintf(fp, "Start: %ld :End\n", local_map.pos.framecnt);
        // fprintf(fp, "Start: %ld :End\n", local_map.pos.status);
        // fprintf(fp, "Start: (%f, %f, %f) :End\n", local_map.pos.x, local_map.pos.y, local_map.pos.z);
        // fprintf(fp, "Start:(%f, %f, %f) :End\n", local_map.pos.az, local_map.pos.el, local_map.pos.rot);
  
        

        // request new data    
        // NOTE: This request should only happen in a single task.    
        jetson_comms.request_map();

        // Allow other tasks to run
        this_thread::sleep_for(loop_time);
    }
}