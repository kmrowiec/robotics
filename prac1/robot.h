/* 
 * File:   robot.h
 * Author: kamil
 *
 * Created on 25 February 2013, 03:19
 */

#ifndef ROBOT_H
#define	ROBOT_H

//#ifdef	__cplusplus
//extern "C" {
//#endif
    
#include <iostream>
#include <others.h>
#include <string>
#include <vector>
#include <libplayerc++/playerc++.h>
using namespace PlayerCc;    

const int GRID_SIZE = 40;

#ifdef ROBOT
const float ROTATE_ERROR = 0.0015;
const float ROTATE_SLOW_SPEED = 10; 
const float ROTATE_TRESHOLD = 15; 
#else
const float ROTATE_ERROR = 0.2;
const float ROTATE_SLOW_SPEED = 1; 
const float ROTATE_TRESHOLD = 5; 
#endif

const float RANGE1 = 0.7;
const float RANGE2 = 1.4;
const float RANGE3 = 2.1;
const float RANGE4 = 2.8;



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
	int p[16]; //proximity 
	int gX, gY; //position on the grid
	Heading h;
        
        Robot(std::string cl);
        void changeHeading(Heading h);
        void rotate(int degrees);
        void moveToCell(Heading dest);
        bool moveToNearbyCell(Point p);
        void exploreRoute(std::vector<Point*> route);
        void exploreWorld();
        void move(double distance);
        void checkProximity();
        void applyProximityToGrid();
        float calcAngularDistance(float a1, float a2);
        Point getGridPosition();
        void drawGrid();
        
};

//#ifdef	__cplusplus
//}
//#endif

#endif	/* ROBOT_H */

