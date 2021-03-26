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

#include <vector>
#include <string>

#include <list.cpp>
#include <iostream>


using namespace vex;

// A global instance of competition
competition Competition;

// create instance of jetson class to receive location and other
// data from the Jetson nano

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

motor camera_motor = motor(PORT9, ratio18_1, false);

// local storage for latest data from the Jetson Nano
static MAP_RECORD       local_map;

// for(i = 0; i < local_map.boxnum; i++){
//   if(local_map.boxobj[i].classID == 1){
//     ballNumber = i;
//   }
// }

// local_map.boxobj[ballNumnber].x

bool move = false;
bool find = false;
bool score = false;
bool pickUp = true;
bool liftUp = true;
int target = 3;
int boxNumber;
int distanceMultiplier;
int Objects;
bool moveGoal = false;

list<int> myBoxNumber;
list<float> myDepth;


// void Goal(){
//   while(true){
//     drive_right.spin(forward, 0, percent);
//     drive_left.spin(forward, 0, percent);
    
//     for(int i = 0; i < Objects; i++){
//       myBoxNumber.pop(0);
//       myDepth.pop(0);
//     }

//     Objects = 0;
  
//     for(int i = 0; i < local_map.boxnum; i++){
//       if(local_map.boxobj[i].classID == target){
//         Objects++;
//         myBoxNumber.append(i);
//         myDepth.append(local_map.boxobj[i].depth);
//       }
//     }

//     for(int i = 0; i<Objects)

//     wait(300,msec);
//   }
// }

void Goal(){
  while(true){
    if(!moveGoal){
      drive_right.spin(forward, 0, percent);
      drive_left.spin(forward, 0, percent);
    }
    
    for(int i = 0; i < local_map.boxnum; i++){
      if(local_map.boxobj[i].classID == target){
        boxNumber = i;
      }
    }

    wait(300,msec);
  }
}

void Move(){
  thread myGoal = thread(Goal);
  FILE *fp = fopen("/dev/serial2","wb");
  camera_motor.spinTo(220,degrees);
  target = 1;

  // intake_fwd.spin(forward,100,percent);
  while(true){
    wait(10,msec);

    if(!moveGoal && target == 2 && target != local_map.boxobj[boxNumber].classID){
      drive_right.spin(reverse, 5, percent);
      drive_left.spin(forward, 5, percent);
    }

    if(!liftUp && moveGoal){
      find = false;
      fprintf(fp, "move");

      wait(2,sec);

      drive_right.spin(forward, 30, percent);
      drive_left.spin(forward, 30, percent);

      wait(4,sec);

      drive_right.spin(forward, 0, percent);
      drive_left.spin(forward, 0, percent);

      intake_fwd.spin(reverse, 100, percent);

      wait(1,sec);

      intake_fwd.stop();

      drive_right.spin(reverse, 30, percent);
      drive_left.spin(reverse, 30, percent);

      wait(1,sec);

      drive_right.spin(forward, 0, percent);
      drive_left.spin(forward, 0, percent);
      
      moveGoal = false;

      intake_fwd.stop();
      
      fprintf(fp, "done");

    
    }

    fprintf(fp, "cam: %.0f \n", local_map.boxobj[boxNumber].depth);
    fprintf(fp, "sonic: %.0f \n", RangeFinder1.distance(mm));

    // drive_left.spin(forward, Controller1.Axis3.value(), percent);
    // drive_right.spin(forward, Controller1.Axis2.value(), percent);

    // arm.spin(forward, (Controller1.ButtonL1.pressing() - Controller1.ButtonL2.pressing())*80, percent);
        
    // intake_fwd.spin(forward, (Controller1.ButtonR1.pressing() - Controller1.ButtonR2.pressing())*80, percent);    


    if(local_map.boxobj[boxNumber].classID == target){
      find = true;
    }else if((RangeFinder1.distance(mm) > 70 && RangeFinder1.distance(mm) < 800) && target != 2){
      find = false;
      pickUp = false;
      

      intake_fwd.spin(forward, 100, percent);

      drive_right.spin(forward, 10, percent);
      drive_left.spin(forward, 10, percent);

    }else if(RangeFinder1.distance(mm) < 70 && target != 2){
      target = 2;      
      find = true;
      camera_motor.spinTo(90, degrees);

    }else if(pickUp && target != 2){
      find = false;
      drive_right.spin(forward, 10, percent);
      drive_left.spin(reverse, 10, percent);
    }else if (!moveGoal){
      find = false;
      if(!pickUp){
        intake_fwd.spin(forward, 5, percent);
      }else{
        intake_fwd.stop(hold);
      }
      drive_right.spin(forward, 0, percent);
      drive_left.spin(reverse, 0, percent);
    }

    if(find){
      if(local_map.boxobj[boxNumber].x < 120){
        drive_right.spin(forward, 5, percent);
        drive_left.spin(reverse, 5, percent);
        move = false;
      }else if(local_map.boxobj[boxNumber].x > 200){
        drive_left.spin(forward, 5, percent);
        drive_right.spin(reverse, 5, percent);
        move = false;
      }else if(local_map.boxobj[boxNumber].x > 120 && local_map.boxobj[boxNumber].x < 200){
        move = true;
      }else{
        drive_right.spin(reverse, 5, percent);
        drive_left.spin(forward, 5, percent);        
      }

      if(move){
        if(target == 2){
          distanceMultiplier = 7;
        }else{
          distanceMultiplier = 1;
        }

        if(local_map.boxobj[boxNumber].depth > 200 * distanceMultiplier){
          drive_right.spin(forward, 40, percent);
          drive_left.spin(forward, 40, percent);
        }else if(target == 2 && liftUp){
          liftUp = false;
          moveGoal = true;

          drive_right.spin(forward, 0, percent);
          drive_left.spin(forward, 0, percent); 

          camera_motor.spinTo(340, degrees);

          arm_left.spin(forward,80,percent);
          arm_right.spin(forward,80,percent);

          wait(700,msec);

          arm_left.stop(hold);
          arm_right.stop(hold);

          fprintf(fp, "lift up");

        }else if(!moveGoal){
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

    while(!LimitSwitch1.pressing()){
      camera_motor.spin(reverse, 10, percent);
    }
    camera_motor.stop(hold);
    camera_motor.resetPosition();
        
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