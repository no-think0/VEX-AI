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
#define  MANAGER_ROBOT    1

#if defined(MANAGER_ROBOT)
#pragma message("building for the manager")
ai::robot_link       link( PORT11, "robot_32456_1", linkType::manager );
#else
#pragma message("building for the worker")
ai::robot_link       link( PORT11, "robot_32456_1", linkType::worker );
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

FILE *fp = fopen("/dev/serial2","wb");

controller Controller1 = controller(primary);

motor left_fwd = motor(PORT7, ratio18_1, false);
motor left_bck = motor(PORT11, ratio18_1, true);
motor right_fwd = motor(PORT8, ratio18_1, true);
motor right_bck = motor(PORT9, ratio18_1, false);

motor_group drive_left = motor_group(left_fwd, left_bck);
motor_group drive_right = motor_group(right_fwd, right_bck);

// local storage for latest data from the Jetson Nano
static MAP_RECORD       local_map;

int closestBlue;
bool blueFound = false;

void checkBlue() {
  while(true){
    fprintf(fp,"Found: %d\n",blueFound);
    int closestBlue;
    bool blueFound = false;

    fprintf(fp,"ID: %ld\n",local_map.boxobj[0].classID);
    fprintf(fp,"X: %ld\n",local_map.boxobj[0].x);

    for(int i=0; i<local_map.boxnum; i++) {
      if(local_map.boxobj[i].classID == 1 && !blueFound) {
        closestBlue = i;
        blueFound = true;
      }
    }
    
    this_thread::sleep_for(300);
  }
}

void Stuff() {
  while(true){
    while(!blueFound) {
      drive_left.spin(forward, 5, percent);
      drive_right.spin(reverse, 5, percent);
    }
    drive_left.stop();
    drive_right.stop();
    while(local_map.boxobj[closestBlue].x < 120) {
       drive_left.spin(reverse, 5, percent);
       drive_right.spin(forward, 5, percent);
    }
    drive_left.stop();
    drive_right.stop();
    while(local_map.boxobj[closestBlue].x > 200) {
      drive_left.spin(forward, 5, percent);
      drive_right.spin(reverse, 5, percent);
    }      
    drive_left.stop();
    drive_right.stop();
    while(local_map.boxobj[closestBlue].depth > 200) {
      drive_left.spin(forward, 5, percent);
      drive_right.spin(reverse, 5, percent);
    }
  }
}

int main() {
    // Initializing Robot Configuration. DO NOT REMOVE!
    vexcodeInit();

    // RUn at about 15Hz
    int32_t loop_time = 66;

    // start the status update display
    thread t1(dashboardTask);
    
    thread t2 = thread(Stuff);
    thread t3 = thread(checkBlue);

    // Set up callbacks for autonomous and driver control periods.
    Competition.autonomous(autonomousMain);

    // print through the controller to the terminal (vexos 1.0.12 is needed)
    // As USB is tied up with Jetson communications we cannot use
    // printf for debug.  If the controller is connected
    // then this can be used as a direct connection to USB on the controller
    // when using VEXcode.
    //
    //FILE *fp = fopen("/dev/serial2","wb");

    while(1) {
        // get last map data
        jetson_comms.get_data( &local_map );

        // set our location to be sent to partner robot
        link.set_remote_location( local_map.pos.x, local_map.pos.y, local_map.pos.az );

        //fprintf(fp, "%.2f %.2f %.2f\n", local_map.pos.x, local_map.pos.y, local_map.pos.az  );

        // request new data    
        // NOTE: This request should only happen in a single task.    
        jetson_comms.request_map();

        // Allow other tasks to run
        this_thread::sleep_for(loop_time);
    }
}