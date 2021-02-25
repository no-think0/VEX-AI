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

controller Controller1 = controller(primary);

motor drive_left = motor(PORT10, ratio18_1, true);
motor drive_right = motor(PORT20, ratio18_1, false);
drivetrain Drivetrain = drivetrain(drive_left, drive_right, 319.19, 256.54, 152.4, mm, (60/84));

motor arm_left = motor(PORT1, ratio18_1, true);
motor arm_right = motor(PORT11, ratio18_1, false);
motor_group arm = motor_group(arm_left, arm_right);

motor intake_fwd = motor(PORT12, ratio18_1, false);

// local storage for latest data from the Jetson Nano
static MAP_RECORD       local_map;

bool move = false;
bool find = false;
bool score = false;

int NRx = 44*25.4;
int NRy = 44*25.4;
int NRd = 35;

void Move(){
  FILE *fp = fopen("/dev/serial2","wb");
  while(true){
    wait(10,msec);

    // drive_left.spin(forward, Controller1.Axis3.value(), percent);
    // drive_right.spin(forward, Controller1.Axis2.value(), percent);

    // arm.spin(forward, (Controller1.ButtonL1.pressing() - Controller1.ButtonL2.pressing())*80, percent);
        
    // intake_fwd.spin(forward, (Controller1.ButtonR1.pressing() - Controller1.ButtonR2.pressing())*80, percent);
    

    if(score){
      score = false;
      while((180 + local_map.pos.az*180/3.1415) > 90){
        drive_right.spin(forward, 5, percent);
        drive_left.spin(reverse, 5, percent);
        // fprintf(fp, "%.0f\n",(180 + local_map.pos.az*180/3.1415));
      }

      drive_right.spin(forward, 0, percent);
      drive_left.spin(forward, 0, percent);

      wait(1,sec);


      while(-1*local_map.pos.x < NRx){
        drive_right.spin(forward, 10, percent);
        drive_left.spin(forward, 10, percent);
        fprintf(fp, "%f, %d\n",local_map.pos.x, NRx);
      }

      

      drive_right.spin(forward, 0, percent);
      drive_left.spin(forward, 0, percent);

      wait(1,sec);

      while((180 + local_map.pos.az*180/3.1415) > 10){
        drive_right.spin(forward, 5, percent);
        drive_left.spin(reverse, 5, percent);
      }

      drive_right.spin(forward, 0, percent);
      drive_left.spin(forward, 0, percent);

      wait(1,sec);
      

      while(-1*local_map.pos.y < NRy){
        fprintf(fp, "%f, %d\n",local_map.pos.y, NRy);
        drive_right.spin(forward, 10, percent);
        drive_left.spin(forward, 10, percent);
      }

      drive_right.spin(forward, 0, percent);
      drive_left.spin(forward, 0, percent);

      wait(1,sec);

      while((180 + local_map.pos.az*180/3.1415) > NRd){
        drive_right.spin(forward, 5, percent);
        drive_left.spin(reverse, 5, percent);
      }

      drive_right.spin(forward, 0, percent);
      drive_left.spin(forward, 0, percent);

    }
    


    if(local_map.boxobj[0].classID == 1){
      find = true;
    }else if(RangeFinderA.distance(mm) > 70 && RangeFinderA.distance(mm) < 600){
      find = false;

      intake_fwd.spin(forward, 100, percent);

      drive_right.spin(forward, 10, percent);
      drive_left.spin(forward, 10, percent);

    }else{
      find = false;
      intake_fwd.spin(forward, 0, percent);
      drive_right.spin(forward, 0, percent);
      drive_left.spin(reverse, 0, percent);
      // score = true;
    }

    if(find){
      if(local_map.boxobj[0].x < 120){
        drive_right.spin(forward, 5, percent);
        drive_left.spin(reverse, 5, percent);
        move = false;
      }else if(local_map.boxobj[0].x > 200){
        drive_left.spin(forward, 5, percent);
        drive_right.spin(reverse, 5, percent);
        move = false;
      }else{
        move = true;
      }

      if(move){
        if(local_map.boxobj[0].depth > 200){
          drive_right.spin(forward, 40, percent);
          drive_left.spin(forward, 40, percent);
        }else if(local_map.boxobj[0].depth < 200){
          drive_right.spin(reverse, 40, percent);
          drive_left.spin(reverse, 40, percent);
        }else{
          drive_left.spin(forward, 0, percent);
          drive_right.spin(reverse, 0, percent);
        }
      }

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
    thread mythread = thread (Move);

    // Set up callbacks for autonomous and driver control periods.
    Competition.autonomous(autonomousMain);

    // print through the controller to the terminal (vexos 1.0.12 is needed)
    // As USB is tied up with Jetson communications we cannot use
    // printf for debug.  If the controller is connected
    // then this can be used as a direct connection to USB on the controller
    // when using VEXcode.
    
    // FILE *fp = fopen("/dev/serial2","wb");
    

    while(1) {
      

        // get last map data
        jetson_comms.get_data( &local_map );

        // set our location to be sent to partner robot
        link.set_remote_location( local_map.pos.x, local_map.pos.y, local_map.pos.az );

        // fprintf(fp, "Start: %ld :End\n", local_map.boxobj[MAX_OBJECT-1].size());
        // fprintf(fp, "box x:%f\n", local_map.boxobj[0].prob);
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