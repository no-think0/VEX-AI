/*----------------------------------------------------------------------------*/
/*                                                                            */
/*    Module:       main.cpp                                                  */
/*    Author:       Jaden                                                     */
/*    Created:      Mon Aug 31 2020                                           */
/*    Description:  V5 project                                                */
/*                                                                            */
/*----------------------------------------------------------------------------*/

// ---- START VEXCODE CONFIGURED DEVICES ----
// ---- END VEXCODE CONFIGURED DEVICES ----

#include "vex.h"

#include <vector>
#include <string>

#include <list>
#include <iostream>


using namespace vex;
using namespace std;

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

FILE *fp = fopen("/dev/serial2","wb");

// local storage for latest data from the Jetson Nano
static MAP_RECORD       local_map;

bool move = false;
bool Find = false;

int target = 1;
int boxNumber;
int distanceMultiplier;


bool notFound = false;
int boxDirection = 1;

bool boxFound = false;

bool pickUp = false;

int accuracy = 50;

bool go = false;

list<float> myDepth;
list<float> myDepthCheck;


void detectBox(){
  while(true){    
    if(RangeFinder1.distance(mm)>400 || RangeFinder1.distance(mm)< 70){
      myDepth.clear();
      myDepthCheck.clear();
    
      for(int i = 0; i < local_map.boxnum; i++){
        if(local_map.boxobj[i].classID == target){
          myDepth.push_back(local_map.boxobj[i].depth);
          myDepthCheck.push_back(local_map.boxobj[i].depth);
        }
      }

      myDepth.sort();

      for(int i = 0; i<myDepth.size(); i++){
        if(myDepth.front() == myDepthCheck.front()){
          boxNumber = i;
          boxFound = true;
        }else{
          myDepthCheck.pop_front();
        }
      }

      if(!boxFound){
        boxNumber = -1;
      }

      boxFound = false;
    }

    wait(10,msec);
  }
}

// void detectBox(){
//   while(true){

//     for(int i = 0; i < local_map.boxnum; i++){
//       if(local_map.boxobj[i].classID == target){
//         boxNumber = i;
//         boxFound = true;
//       }
//     }

//     if(!boxFound){
//       boxNumber = -1;
//     }

//     boxFound = false;

//     if(local_map.boxobj[boxNumber].x <=320){
//       if(local_map.boxobj[boxNumber].x<160){
//         boxDirection = 1;
//       }else if(local_map.boxobj[boxNumber].x>160){
//         boxDirection = -1;
//       }
//     }
    
//     // fprintf(fp, "1: %d\n", boxDirection);
//     // fprintf(fp, "2: %ld\n", (local_map.boxobj[boxNumber].x));

//     wait(10,msec);
//   }
// }



void findBox(){
  target = 1;

  while(true){

    if(RangeFinder1.distance(mm)<70 && pickUp){
      target = 2;
      pickUp = true;
      camera_motor.spinTo(90, degrees);
    }

    // fprintf(fp,"depth: %0.f\n",local_map.boxobj[boxNumber].depth);
    // fprintf(fp,"accuracy: %d\n",accuracy);

    if(local_map.boxobj[boxNumber].depth < 650 && local_map.boxobj[boxNumber].depth > 0){
      accuracy = round((local_map.boxobj[boxNumber].depth/2*100)*10);
    }else{
      accuracy = 30;
    }
  
    if(boxNumber == -1 && (RangeFinder1.distance(mm)>300 || pickUp)){
      fprintf(fp,"sonic: %0.1f\n", RangeFinder1.distance(mm));
      drive_right.spin(fwd, 5*boxDirection, percent);
      drive_left.spin(reverse, 5*boxDirection, percent);

    } else if(local_map.boxobj[boxNumber].x <=320 && (local_map.boxobj[boxNumber].x<(155-accuracy) || local_map.boxobj[boxNumber].x>(165+accuracy)) && RangeFinder1.distance(mm)>300){
      drive_right.spin(reverse, (local_map.boxobj[boxNumber].x-160)/15, percent); 
      drive_left.spin(fwd, (local_map.boxobj[boxNumber].x-160)/15, percent);
      
    }else{
      if(target == 2 && local_map.boxobj[boxNumber].depth < 1200){
        drive_right.spin(fwd,40,percent);
        drive_left.spin(fwd,40,percent);
      }

      intake_fwd.spin(reverse, 0.25, percent);
    }

    if(RangeFinder1.distance(mm)>70 && RangeFinder1.distance(mm)<400){
      intake_fwd.spin(fwd,100,percent);
    }
    wait(10,msec);
  }
}

int main() {
    // Initializing Robot Configuration. DO NOT REMOVE!
    vexcodeInit();

    // RUn at about 15Hz
    int32_t loop_time = 66;

    thread t1(dashboardTask);        

    while(!LimitSwitch1.pressing()){
      camera_motor.spin(reverse, 10, percent);
    }

    camera_motor.stop(hold);
    camera_motor.resetPosition();

    camera_motor.spinTo(180,degrees);

    // start the status update display
    thread t2(findBox);
    thread t3(detectBox);

    // Set up callbacks for autonomous and driver control periods.
    Competition.autonomous(autonomousMain);

    // print through the controller to the terminal (vexos 1.0.12 is needed)
    // As USB is tied up with Jetson communications we cannot use
    // printf for debug.  If the controller is connected
    // then this can be used as a direct connection to USB on the controller
    // when using VEXcode.    

    while(1) {
        // get last map data
        jetson_comms.get_data( &local_map );

        // set our location to be sent to partner robot
        link.set_remote_location( local_map.pos.x, local_map.pos.y, local_map.pos.az );

        // request new data    
        // NOTE: This request should only happen in a single task.    
        jetson_comms.request_map();

        // Allow other tasks to run
        this_thread::sleep_for(loop_time);
    }
}