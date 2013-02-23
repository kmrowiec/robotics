/*
 * Copyright (c) 2005, Brad Kratochvil, Toby Collett, Brian Gerkey, Andrew Howard, ...
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright notice,
 *           this list of conditions and the following disclaimer.
 *               * Redistributions in binary form must reproduce the above copyright notice,
 *                     this list of conditions and the following disclaimer in the documentation
 *                           and/or other materials provided with the distribution.
 *                               * Neither the name of the Player Project nor the names of its contributors
 *                                     may be used to endorse or promote products derived from this software
 *                                           without specific prior written permission.
 *
 *                                           THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 *                                           ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *                                           WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *                                           DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 *                                           ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *                                           (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *                                           LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 *                                           ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *                                           (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *                                           SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *                                           */

#include <iostream>
#include <libplayerc++/playerc++.h>
#include <math.h>
//#include <"robot.cpp">

using namespace PlayerCc;

//0.001 for robot , 0.1 for simulator
const float ROTATE_ERROR = 0.1;
const float ROTATE_SLOW_SPEED = 1; //10 for robot, 1 for sim

void rotate(Position2dProxy *pp, PlayerClient *robot, int degrees);
void move(Position2dProxy *pp, PlayerClient *robot, double distance);
float calcAngularDistance(float a1, float a2);
void moveTo(Position2dProxy *pp, PlayerClient *robot, char c);

	
int main(int argc, char *argv[])
{
	

	PlayerClient    robot("localhost");
	RangerProxy      sp(&robot,0);
	Position2dProxy pp(&robot,0);
// 	pp = new Position2dProxy(&robot,0);

	pp.SetMotorEnable(true);

	
	move(&pp, &robot, 2);
	rotate(&pp, &robot, -90);
	move(&pp, &robot, 2);
	rotate(&pp, &robot, -90);
	move(&pp, &robot, 2);
	rotate(&pp, &robot, -90);
	move(&pp, &robot, 2); 
	
	//moveTo(&pp, &robot, 'N');
	
	//for(;;)
	//{
		//double turnrate, speed;

		////  read from the proxies
		//robot.Read();

		////         print out sonars for fun
		//std::cout << sp << std::endl;

		////             do simple collision avoidance
		////if((sp[0] + sp[1]) < (sp[6] + sp[7]))
		////	turnrate = dtor(-20); // turn 20 degrees per second
		////else
			//turnrate = dtor(-10);

		////turnrate = 0;
		////if(sp[3] < 0.500)
			////speed = 0;
		////else
			//speed = 0.200;

		////                                                           command the motors
		////pp.SetSpeed(speed, turnrate);
		
		//pp.SetSpeed(speed, 0);
	//}
}

void rotate(Position2dProxy *pp, PlayerClient *robot, int degrees){

    robot->Read();
    float initialAngle = rtod(pp->GetYaw());
    
    float currentAngle = 0;
    
    //Caclulating expected angle (in degrees)
    //If ig goes above 181, need to substract -360
    //Or add 360 if below -180
    float expectedAngle = initialAngle + degrees;
    if(expectedAngle > 180) expectedAngle -= 360;
    else if(expectedAngle < -180) expectedAngle += 360;
    
    int speed = 5;
    float distance = 0;
    
    for(;;){
      
      robot->Read(); // reading sensors
      currentAngle = rtod(pp->GetYaw());
      distance = calcAngularDistance(currentAngle, expectedAngle);
      
      std::cout << "Started at : " << initialAngle << std::endl;
      std::cout << "Current angle: " << currentAngle << std::endl;
      std::cout << "Going to: " << expectedAngle << std::endl;
      std::cout << "Distance: " << distance << std::endl;
      
      
      if(distance < ROTATE_ERROR){
		//int i = 0;
		//speed = 10;
		//if(degrees<0)
			//pp->SetSpeed(0,dtor(-speed));
        //else
			//pp->SetSpeed(0,dtor(speed));
		
		//usleep(600000);
		return;
      } 
      
      //If distance to the desired angle is high, we can move faster
      //but when we get closer, need to slow down to increase accuracy.
      if(distance > 15) speed = 20;
      else speed = ROTATE_SLOW_SPEED;
       
      if(degrees<0)
		pp->SetSpeed(0,dtor(-speed));
      else
		pp->SetSpeed(0,dtor(speed));
      
    }
    
}

void move(Position2dProxy *pp, PlayerClient *robot, double distance){
    robot->Read();
    double iX, iY;
    iX = pp->GetXPos();
    iY = pp->GetYPos();
    
    double cX, cY;
    double cDistance;
    
    for(;;){
      robot->Read();
      cX = pp->GetXPos();
      cY = pp->GetYPos();
      std::cout << "X: " <<cX<<"Y: "<<cY << std::endl;
      
      cDistance = sqrt( pow(cX-iX, 2) + pow(cY-iY,2));   
      if(cDistance - distance >= -0.05) break;
      pp->SetSpeed(0.2, 0);
      
    }
    
}

/**
* moves the robot to the closest cell, c specifies which one (N,S,W,E);
*/
void moveTo(Position2dProxy *pp, PlayerClient *robot, char c){
  switch(c){
    case 'N':
      break;
    case 'S':
      rotate(pp, robot, -180);
      break;
    case 'E':
      rotate(pp, robot, -90);
      break;
    case 'W':
      rotate(pp, robot, 90);
      break;
      
  }
  
  robot->Read();
    double iX, iY;
    iX = pp->GetXPos();
    iY = pp->GetYPos();
    
    double cX, cY;
    double distance = 0.6;
    double cDistance;
    
    for(;;){
      robot->Read();
      cX = pp->GetXPos();
      cY = pp->GetYPos();
      std::cout << "X: " <<cX<<"Y: "<<cY << std::endl;
      
      cDistance = sqrt( pow(cX-iX, 2) + pow(cY-iY,2));   
      if(cDistance - distance >= -0.05) break;
      pp->SetSpeed(0.2, 0);
      
    }
}

/**
 * Calculates the distance between two angles.
 * Input as values in robot's range.
 */
 float calcAngularDistance(float a1, float a2){
	if(a1 < 0) a1+=360;
	if(a2 < 0) a2+=360;
	
	float dist = a2 - a1;
	if(dist < 0) dist = -dist;
	if(dist > 180) dist = 360 - dist;
	return dist;
}

