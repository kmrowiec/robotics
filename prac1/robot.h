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
const float ROTATE_ERROR = 0.1;
const float ROTATE_SLOW_SPEED = 0.3; 
const float ROTATE_TRESHOLD = 3; 
#endif

const float CELL_SIZE = 0.65;
const float CELL_PADDING = 0.16;
const float SIDE_DIFF = 0.07;
const float RANGE1 = CELL_SIZE + CELL_PADDING;
const float RANGE2 = 2*CELL_SIZE + CELL_PADDING;
const float RANGE3 = 3*CELL_SIZE + CELL_PADDING;
const float RANGE4 = 4*CELL_SIZE + CELL_PADDING;



class Robot{
	
	public:
	
	PlayerClient* client;
		
	#ifdef ROBOT
		SonarProxy* sp;
        #else
		RangerProxy* sp;
        #endif

	Position2dProxy* pp;
        
        /* Represents occupancy grid. Each element has a number that
         * says how sure the robot is about the cell.
         * 0 if completely unknown, negative numbers if cell is empty,
         * and positive if occupied.
         * After each measurement : 
         * 0 if unknown, 5 or -5 if not so sure, 8 or -8 if pretty sure.
         * We assume that cell is known if number is <-10 or >10.
         */
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

