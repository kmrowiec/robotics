/* 
 * File:   robot.h
 * Author: kamil
 *
 * Created on 25 February 2013, 03:19
 */

#ifndef ROBOT_H
#define	ROBOT_H

#ifdef	__cplusplus
extern "C" {
#endif
    
#include <iostream>
#include <string>
#include <libplayerc++/playerc++.h>
using namespace PlayerCc;    

const int GRID_SIZE = 30;

#ifdef ROBOT
const float ROTATE_ERROR = 0.0015;
const float ROTATE_SLOW_SPEED = 10; 
#else
const float ROTATE_ERROR = 0.1;
const float ROTATE_SLOW_SPEED = 2; 
#endif

enum Heading{
    NORTH = 1,
	EAST = 2,
	SOUTH = 3,
	WEST = 4
};

class Robot{
	
	public:
	
	PlayerClient* client;
		
	#ifdef ROBOT
		SonarProxy* sp;
    #else
		RangerProxy* sp;
    #endif

	Position2dProxy* pp;
	int grid[GRID_SIZE][GRID_SIZE];
	int p[5][5]; //proximity 
	int gX, gY; //position on the grid
	Heading h;
        
        Robot(std::string cl);
        void changeHeading(Heading h);
        void rotate(int degrees);
        void moveToCell(Heading dest);
        void move(double distance);
        void checkProximity();
        void applyProximityToGrid();
        float calcAngularDistance(float a1, float a2);
        void drawGrid();
        
};

#ifdef	__cplusplus
}
#endif

#endif	/* ROBOT_H */

