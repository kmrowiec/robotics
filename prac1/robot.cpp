#include <iostream>
#include <string>
#include <libplayerc++/playerc++.h>
#include <robot.h>
#include <vector>
#include <fstream>
#include <cstdlib>

#include "utils.h"

using namespace PlayerCc;
using namespace std;

Robot::Robot(string cl) {
    client = new PlayerClient(cl);
    #ifdef ROBOT
		sp = new SonarProxy(client, 0);
    #else
		sp = new RangerProxy(client, 0);
    #endif
     
    pp = new Position2dProxy(client, 0);
    pp->SetMotorEnable(true);
    h = NORTH;

    int x = 0, y = 0;
    for (x; x < GRID_SIZE; x++) {
        for (y = 0; y < GRID_SIZE; y++) {
            if(x < 4 || x > GRID_SIZE-5 || y < 4 || y > GRID_SIZE-5)
            grid[x][y] = OCC0;    //map has a border
            else grid[x][y] = 0; //unknown terrain
        }
    }
}

/**
 * Changes the actual heading of a robot by rotating it 
 * (to the new heading, which is relative to the existing one).
 * Assuming that robot rotates only in clockwise direction, 
 * except when rotating to west. (but 180 rotation is clockwise. always)
 **/
void Robot::changeHeading(Heading h) {
	int currentHeading = this->h;
	switch(h){
		case NORTH: //cannot rotate forwards!
			break;
		case EAST:
			rotate(-90);
			currentHeading++;
			break;
		case SOUTH:
			rotate(-180);
			currentHeading+=2;
			break;
		case WEST:
			rotate(90);
			currentHeading--;
			break;
	}
	currentHeading = currentHeading % 4;
	if(currentHeading == 0) currentHeading = 4;
	this->h = (Heading) currentHeading;	
}

void Robot::rotate(int degrees) {

    client->Read();
    float initialAngle = rtod(pp->GetYaw());

    float currentAngle = 0;

    //Calculating expected angle (in degrees)
    //If ig goes above 181, need to subtract -360
    //Or add 360 if below -180
    float expectedAngle = initialAngle + degrees;
    if (expectedAngle > 180) expectedAngle -= 360;
    else if (expectedAngle < -180) expectedAngle += 360;

    int speed = 5;
    float distance = 0;

    for (;;) {

        client->Read(); // reading sensors
        currentAngle = rtod(pp->GetYaw());
        distance = calcAngularDistance(currentAngle, expectedAngle);


//std::cout << "Started at : " << initialAngle << std::endl;
//std::cout << "Current angle: " << currentAngle << std::endl;
//std::cout << "Going to: " << expectedAngle << std::endl;
//std::cout << "Distance: " << distance << std::endl;


        if (distance < ROTATE_ERROR) {
	  cout<< "FINISHING!" <<endl;
            #ifdef ROBOT
            int i = 0;
            speed = 10;
            if(degrees<0)
            pp->SetSpeed(0,dtor(-speed));
            else
            pp->SetSpeed(0,dtor(speed));
	    
            usleep(700000);
	    if(degrees== 180 || degrees == -180) usleep(600000);
			pp->SetSpeed(0, dtor(0));
	    cout<< "SPEED IS ZERO!" <<endl;
			#endif
	    
            return;
        }

        //If distance to the desired angle is high, we can move faster
        //but when we get closer, need to slow down to increase accuracy.
        if (distance > ROTATE_TRESHOLD) speed = 20;
        else speed = ROTATE_SLOW_SPEED;

        if (degrees < 0)
            pp->SetSpeed(0, dtor(-speed));
        else
            pp->SetSpeed(0, dtor(speed));

    }
} //end of rotate function

void Robot::moveToCell(Heading dest) {
    switch (dest) {
        case WEST:
            changeHeading(WEST);
            break;
        case SOUTH:
            changeHeading(SOUTH);
            break;
        case EAST:
            changeHeading(EAST);
            break;
    }
    //if NORTH then we just move forward
    //if not, we changed heading (i. e. rotated) first  
    move(CELL_SIZE);
    //changing xy position on the grid
    if (h == NORTH) gY--;
    else if (h == EAST) gX++;
    else if (h == WEST) gX--;
    else gY++;  
}

bool Robot::moveToCellIfEmpty(Heading dest) {
    switch (dest) {
        case WEST:
            changeHeading(WEST);
            break;
        case SOUTH:
            changeHeading(SOUTH);
            break;
        case EAST:
            changeHeading(EAST);
            break;
    }
    //if NORTH then we just move forward
    //if not, we changed heading (i. e. rotated) first
    //First checking if there is no obstacle ahead.
    //if is, returning false;
    client->Read();
    if((*sp)[3] < RANGE1 || (*sp)[4] < RANGE1) return false;
    
    move(CELL_SIZE);
    //changing xy position on the grid
    if (h == NORTH) gY--;
    else if (h == EAST) gX++;
    else if (h == WEST) gX--;
    else gY++; 
    return true;
}



bool Robot::moveToNearbyCell(Point p){
    if(grid[p.x][p.y] > -20) return false;
    //cell to the east
    if(p.x == this->gX+1 && p.y == this->gY){
        if(h==EAST) moveToCell(NORTH);
        else if(h==NORTH) moveToCell(EAST);
        else if(h==SOUTH) moveToCell(WEST);
        else moveToCell(SOUTH);
    }else if(p.x == this->gX && p.y == this->gY+1){
        //cell to the south
        if(h==EAST) moveToCell(EAST);
        else if(h==NORTH) moveToCell(SOUTH);
        else if(h==SOUTH) moveToCell(NORTH);
        else moveToCell(WEST);
    }else if(p.x == this->gX-1 && p.y == this->gY){
        //cell to the west
        if(h==EAST) moveToCell(SOUTH);
        else if(h==NORTH) moveToCell(WEST);
        else if(h==SOUTH) moveToCell(EAST);
        else moveToCell(NORTH);
    }else if(p.x == this->gX && p.y == this->gY-1){
        //cell to the north
        if(h==EAST) moveToCell(WEST);
        else if(h==NORTH) moveToCell(NORTH);
        else if(h==SOUTH) moveToCell(SOUTH);
        else moveToCell(EAST);
    }
    return true;
}

void Robot::exploreRoute(vector<Point*> route){
    checkProximity();
    applyProximityToGrid();
    if(route.empty()){
        cout << "Route is empty" << endl;
        drawGrid();
        return;
    }
    
    int i;
    for (i = 0; i < route.size(); i++) {
        if(moveToNearbyCell(*(route.at(i))) == false) return;
        checkProximity();
        applyProximityToGrid();
    }
}

void Robot::findHiddenRobot(){
    vector<Point> hideouts = findHidingSpots();
    int i = 0;
    for(i; i<hideouts.size(); i++){
        if(checkForRobot(hideouts.at(i))){
            cout << "Found robot at x=" << gX << " and y=" <<gY << endl;
            return;
        }
    }
    cout << "Cannot find a robot :( " << endl;
}


//Goes to the point given as parameter and checks if a robot is hidden there.
//Returns true if robot is found, false otherwise.
bool Robot::checkForRobot(Point p){
    vector<Point*> route = findRoute(Point(this->gX, this->gY), p, this);
    route.pop_back(); //removing last element, so the robot will stop one cell
                      //before the expected hideout
    move(route);
    
    Heading useSensors;
    if(p.x == gX){
        if(p.y > gY){
            //robot is in the cell to the north of the target
            if(h==WEST){
                //use left hand side sensors
                useSensors = WEST;
            }else if(h==SOUTH){
                //use front sensors
                useSensors = NORTH;
            }else{
                useSensors = EAST;
                //use right hand side sensors
            }
        }
        else{ 
            //robot is in the cell to the south of the target
            if(h==WEST){
                //use right hand side sensors
                useSensors = EAST;
            }else if(h==NORTH){
                //use front sensors
                useSensors = NORTH;
            }else{
                //use left hand side sensors
                useSensors = WEST;
            }
        }
    }else{
        if(p.x > gX){
            //robot is in the cell to the west of the target
            if(h==NORTH){
                //use right hand side sensors
                useSensors = EAST;
            }else if(h==EAST){
                //use front sensors
                useSensors = NORTH;
            }else{
                //use left hand side sensors
                useSensors = WEST;
            }
        }
        else{
            //robot is in the cell to the east of the target
            if(h==NORTH){
                //use left hand side sensors
                useSensors = WEST;
            }else if(h==WEST){
                //use front sensors
                useSensors = NORTH;
            }else{
                //use right hand side sensors
                useSensors = EAST;
            }
        }
    }
    
    if(useSensors == NORTH){
        //using sensors in front of the robot
        if((*sp)[3] < RANGE1 || (*sp)[4] < RANGE1) return true;
        
    }else if(useSensors == WEST){
        if((*sp)[0] < RANGE1 || (*sp)[15] < RANGE1) return true;
    }else{
        if((*sp)[7] < RANGE1 || (*sp)[8] < RANGE1) return true;
    }
    return false;
    
}

void Robot::move(vector<Point*> route){
	if(route.empty()){
        cout << "Cannot move - route is empty" << endl;       
    }else{
		int i;
		for (i = 0; i < route.size(); i++) {
			if(moveToNearbyCell(*(route.at(i))) == false) return;
		}
	}
}

void Robot::exploreWorld(){
    while(1){
        Point p = findNearestUnexplored(this);
        //cout << "Nearest unexplored point: " << p.x << " " << p.y <<endl;
        //cout << "Grid repres. : " << grid[p.x][p.y] << endl;
        if(p.x == -1 && p.y == -1) break;
        vector<Point*> route = findRoute(this->getGridPosition(), p, this);
        this->exploreRoute(route);
        this->drawGrid();
    }
    cout << "Map ready!" << endl;
}

vector<Point> Robot::findHidingSpots(){
	vector<Point> best_hideouts;
	vector<Point> good_hideouts;
	
	int x = 0, y = 0;
	int n;
        for (y = 4; y < GRID_SIZE - 5; y++) {
            for (x = 4; x < GRID_SIZE - 5; x++) {
                if(grid[x][y] < -20){ // if the cell is not occupied
                                    n = 0;

                                    //checking how many walls are adjacent to the cell
                                    if(grid[x][y-1] > 20) n++;
                                    if(grid[x][y+1] > 20) n++;
                                    if(grid[x-1][y] > 20) n++;
                                    if(grid[x+1][y] > 20) n++;

                                    if(n==3){ //if three walls around
                                            best_hideouts.push_back(Point(x,y));
                                    }else if(n==2){
                                            good_hideouts.push_back(Point(x,y));
                                    }
                            }             
            }
        }
    
        int i = 0;
        for(i; i<good_hideouts.size(); i++){
            best_hideouts.push_back(good_hideouts.at(i));
        }
        
        
    return best_hideouts;
}
	

void Robot::move(double distance) {
    client->Read();
    double iX, iY;
    iX = pp->GetXPos();
    iY = pp->GetYPos();

    double cX, cY;
    double cDistance;

    for (;;) {
        client->Read();
        cX = pp->GetXPos();
        cY = pp->GetYPos();
        //std::cout << "X: " <<cX<<"Y: "<<cY << std::endl;

        cDistance = sqrt(pow(cX - iX, 2) + pow(cY - iY, 2));
	//cout << cDistance << endl;
	#ifdef ROBOT
            if (cDistance - distance >= -0.01) break;
            if (cDistance - distance >= -0.1) pp->SetSpeed(0.1, 0);
            else pp->SetSpeed(0.2, 0);
	    #else
	    if (cDistance - distance >= -0.005) break;
            if (cDistance - distance >= -0.1) pp->SetSpeed(0.05, 0);
            else pp->SetSpeed(0.2, 0);
	    #endif
        

    }
    pp->SetSpeed(0, 0);
}

/**
 * Checks the neighbouring cells if they are occupied.
 * Result of the check is saved in array 'p' :
 * 0 when empty
 * 1 when occupied
 * -1 when unknown
 * First checks the closest cell, then the 2-places away ones.
 * (if those closer are occupied, the further are unknown).
 * Warning! Awful function!
 */
void Robot::checkProximity() {
    client->Read();
    int i; 
    
    for(i = 0; i< 16;i++){
        p[i] = 0;
    }
    
    cout << "Sensors:" << endl;
    cout << "Front: " << (*sp)[3] <<" "<<(*sp)[4]<<endl;
    cout << "Right: " << (*sp)[7] <<" "<<(*sp)[8]<<endl;
    cout << "Back: " << (*sp)[11] <<" "<<(*sp)[12]<<endl;
    cout << "Left: " << (*sp)[0] <<" "<<(*sp)[15]<<endl;
    
    bool wallNearby = false;
    if((*sp)[7] < RANGE1+SIDE_DIFF || (*sp)[8] < RANGE1+SIDE_DIFF || 
            (*sp)[0] < RANGE1+SIDE_DIFF || (*sp)[15] < RANGE1+SIDE_DIFF) wallNearby = true;
    //Checking front
    if(wallNearby){
        //If robot is close to the wall, only first cell is checked
        if((*sp)[3] < RANGE1 || (*sp)[4] < RANGE1){ p[0] += OCC1; goto next1;}
        else p[0] += FREE1;
    }else{
        if((*sp)[3] < RANGE1 || (*sp)[4] < RANGE1){ p[0] += OCC1; goto next1;}
        else p[0] += FREE1;
        if((*sp)[3] < RANGE2 || (*sp)[4] < RANGE2){ p[1] += OCC2; goto next1;}
        else p[1] += FREE2;
        if((*sp)[3] < RANGE3 || (*sp)[4] < RANGE3){ p[2] += OCC3;  goto next1;}
        else p[2] += FREE3;
        if((*sp)[3] < RANGE4 || (*sp)[4] < RANGE4){ p[3] += OCC4;  goto next1;}
        else p[3] += FREE4;
    }
    
    
    next1:
    //Checking right side
    if((*sp)[7] < RANGE1+SIDE_DIFF || (*sp)[8] < RANGE1+SIDE_DIFF){ p[4] += OCC1; goto next2;}
    else p[4] += FREE1;
    if((*sp)[7] < RANGE2+SIDE_DIFF || (*sp)[8] < RANGE2+SIDE_DIFF){ p[5] += OCC2; goto next2;}
    else p[5] += FREE2;
    if((*sp)[7] < RANGE3+SIDE_DIFF || (*sp)[8] < RANGE3+SIDE_DIFF){ p[6] += OCC3; goto next2;}
    else p[6] += FREE3;
    if((*sp)[7] < RANGE4+SIDE_DIFF || (*sp)[8] < RANGE4+SIDE_DIFF){ p[7] += OCC4; goto next2;}
    else p[7] += FREE4;
    
    next2:
    //Checking back side
    
    if(wallNearby){
        if((*sp)[11] < RANGE1 || (*sp)[12] < RANGE1){ p[8] += OCC1; goto next3;}
        else p[8] += FREE1;
    }else{
        if((*sp)[11] < RANGE1 || (*sp)[12] < RANGE1){ p[8] += OCC1; goto next3;}
        else p[8] += FREE1;
        if((*sp)[11] < RANGE2 || (*sp)[12] < RANGE2){ p[9] += OCC2; goto next3;}
        else p[9] += FREE2;
        if((*sp)[11] < 1.8 || (*sp)[12] < 1.8){ p[10] += OCC3; goto next3;}
        else p[10] += FREE3;
        if((*sp)[11] < 2.4 || (*sp)[12] < 2.4){ p[11] += OCC4; goto next3;}
        else p[11] += FREE4;
    }
    
    
    next3:
    //Checking left side
    if((*sp)[0] < RANGE1+SIDE_DIFF || (*sp)[15] < RANGE1+SIDE_DIFF){ p[12] += OCC1; return;}
    else p[12] += FREE1;
    if((*sp)[0] < RANGE2+SIDE_DIFF || (*sp)[15] < RANGE2+SIDE_DIFF){ p[13] += OCC2; return;}
    else p[13] += FREE2;
    if((*sp)[0] < RANGE3+SIDE_DIFF || (*sp)[15] < RANGE3+SIDE_DIFF){ p[14] += OCC3; return;}
    else p[14] += FREE3;
    if((*sp)[0] < RANGE4+SIDE_DIFF || (*sp)[15] < RANGE4+SIDE_DIFF){ p[15] += OCC4; return;}
    else p[15] += FREE4;
    
}

/**
 * Applies the information about cells nearby to the occupancy grid. 
 */
void Robot::applyProximityToGrid() {
       
//     int i = 0;
//     for(i = 0; i< 16;i++){
//        cout << p[i]<< " ";
//     }
     
     grid[gX][gY] += FREE0; //where robot actually is, obviously the cell is free for sure
     
    switch (h) {
        case NORTH:
            grid[gX][gY-1] += p[0]; grid[gX][gY-2] += p[1];
            grid[gX][gY-3] += p[2]; grid[gX][gY-4] += p[3];
            grid[gX+1][gY] += p[4]; grid[gX+2][gY] += p[5];
            grid[gX+3][gY] += p[6]; grid[gX+4][gY] += p[7];
            grid[gX][gY+1] += p[8]; grid[gX][gY+2] += p[9];
            grid[gX][gY+3] += p[10]; grid[gX][gY+4] += p[11];
            grid[gX-1][gY] += p[12]; grid[gX-2][gY] += p[13];
            grid[gX-3][gY] += p[14]; grid[gX-4][gY] += p[15];
            break;
        case EAST:
            grid[gX+1][gY] += p[0]; grid[gX+2][gY] += p[1];
            grid[gX+3][gY] += p[2]; grid[gX+4][gY] += p[3];
            grid[gX][gY+1] += p[4]; grid[gX][gY+2] += p[5];
            grid[gX][gY+3] += p[6]; grid[gX][gY+4] += p[7];           
            grid[gX-1][gY] += p[8]; grid[gX-2][gY] += p[9];
            grid[gX-3][gY] += p[10]; grid[gX-4][gY] += p[11];         
            grid[gX][gY-1] += p[12]; grid[gX][gY-2] += p[13];
            grid[gX][gY-3] += p[14]; grid[gX][gY-4] += p[15];
            break;
        case SOUTH:
            grid[gX][gY+1] += p[0]; grid[gX][gY+2] += p[1];
            grid[gX][gY+3] += p[2]; grid[gX][gY+4] += p[3];           
            grid[gX-1][gY] += p[4]; grid[gX-2][gY] += p[5];
            grid[gX-3][gY] += p[6]; grid[gX-4][gY] += p[7];       
            grid[gX][gY-1] += p[8]; grid[gX][gY-2] += p[9];
            grid[gX][gY-3] += p[10]; grid[gX][gY-4] += p[11];       
            grid[gX+1][gY] += p[12]; grid[gX+2][gY] += p[13];
            grid[gX+3][gY] += p[14]; grid[gX+4][gY] += p[15];
            break;
            
        case WEST:
            grid[gX-1][gY] += p[0]; grid[gX-2][gY] += p[1];
            grid[gX-3][gY] += p[2]; grid[gX-4][gY] += p[3];         
            grid[gX][gY-1] += p[4]; grid[gX][gY-2] += p[5];
            grid[gX][gY-3] += p[6]; grid[gX][gY-4] += p[7];           
            grid[gX+1][gY] += p[8]; grid[gX+4][gY] += p[9];
            grid[gX+3][gY] += p[10]; grid[gX+4][gY] += p[11];          
            grid[gX][gY+1] += p[12]; grid[gX][gY+2] += p[13];
            grid[gX][gY+3] += p[14]; grid[gX][gY+4] += p[15];
            break;
    }
    
}

/**
 * Calculates the distance between two angles.
 * Input as values in robot's range.
 */
float Robot::calcAngularDistance(float a1, float a2) {
    if (a1 < 0) a1 += 360;
    if (a2 < 0) a2 += 360;

    float dist = a2 - a1;
    if (dist < 0) dist = -dist;
    if (dist > 180) dist = 360 - dist;
    return dist;
}

void Robot::drawGrid() {
    int x = 0, y = 0;
    for (y; y < GRID_SIZE; y++) {
        for (x = 0; x < GRID_SIZE; x++) {
            if(x == gX && y == gY){
                switch(h){
                    case NORTH:
                        cout << "^";
                        break;
                    case EAST:
                        cout << ">";
                        break;
                    case SOUTH:
                        cout << "V";
                        break;
                    case WEST:
                        cout << "<";
                }      
            }
            else{
                if(grid[x][y] > 20) cout << "#";
                else if(grid[x][y] < -20) cout <<" ";
                else cout << "?";
            }              
        }
        cout << endl;
    }
}

Point Robot::getGridPosition(){
    return Point(gX, gY);
}

void Robot::saveGridToFile(string filename){
	ofstream file;
	file.open("grid.txt");
	
	int x, y;
	for (y = 0; y < GRID_SIZE; y++) {
        for (x = 0; x < GRID_SIZE; x++) {
			file << grid[x][y] << ";" ;
        }
        file << endl;
    }
	file.close();
}

void Robot::loadGridFromFile(string filename){
	ifstream file;
	file.open("grid.txt");
	
	//int g[GRID_SIZE][GRID_SIZE];
	string line, number;
	
	int x, y;
	for (y = 0; y < GRID_SIZE; y++) {
		getline(file, line);
		vector<string> numbers = split(line, ';');
                for (x = 0; x < GRID_SIZE; x++) {
			number = numbers.at(x); //loading number to string
                        knownMap[x][y] = strtol(number.c_str(), NULL, 10);
                }
        }
        file.close();
        cout << "Grid loaded from file. " << endl;	
} 

void Robot::recognisePosition(){
    int startX = gX, startY = gY;
    int diffX = 0, diffY = 0;
    vector<Point> matchingPoints;
    
    checkProximity();
    applyProximityToGrid();
    
    int x, y;
    for(y = 4; y < GRID_SIZE - 4; y++){
        for(x = 4; x < GRID_SIZE - 4; x++){
            if(grid[gX][gY]!=0 ? (knownMap[x][y] * grid[gX][gY]> 0) : true)
            if(knownMap[x+1][y] * grid[gX+1][gY] > 0)
            if(grid[gX+2][gY]!=0 ? (knownMap[x+2][y] * grid[gX+2][gY] > 0) : true)
            if(knownMap[x][y+1] * grid[gX][gY+1] > 0)
            if(grid[gX][gY+2]!=0 ? (knownMap[x][y+2] * grid[gX][gY+2] > 0) : true)
            if(knownMap[x-1][y] * grid[gX-1][gY] > 0)
            if(grid[gX-2][gY]!=0 ? (knownMap[x-2][y] * grid[gX-2][gY] > 0) : true)
            if(knownMap[x][y-1] * grid[gX][gY-1] > 0)
            if(grid[gX][gY-2]!=0 ? (knownMap[x][y-2] * grid[gX][gY-2] > 0) : true)
                matchingPoints.push_back(Point(x,y));
        }   
    }
    
    
    while(matchingPoints.size() > 1){
        Point p = findNearestUnexplored(this);
        if(p.x == -1 && p.y == -1) break;
        vector<Point*> route = findRoute(this->getGridPosition(), p, this);
        this->exploreRoute(route);
        
        vector<Point> tempPoints;
        diffX = gX - startX;
        diffY = gY - startY;
        
        int i;
        for(i = 0; i < matchingPoints.size(); i++){
            Point * p = &matchingPoints.at(i);
            if(grid[gX][gY]!=0 ? (knownMap[p->x+diffX][p->y+diffY] * grid[gX][gY]> 0) : true)
            if(knownMap[p->x+1+diffX][p->y+diffY] * grid[gX+1][gY] > 0)
            if(grid[gX+2][gY]!=0 ? (knownMap[p->x+2+diffX][p->y+diffY] * grid[gX+2][gY] > 0) : true)
            if(knownMap[p->x+diffX][p->y+1+diffY] * grid[gX][gY+1] > 0)
            if(grid[gX][gY+2]!=0 ? (knownMap[p->x+diffX][p->y+2+diffY] * grid[gX][gY+2] > 0) : true)
            if(knownMap[p->x-1+diffX][p->y+diffY] * grid[gX-1][gY] > 0)
            if(grid[gX-2][gY]!=0 ? (knownMap[p->x-2+diffX][p->y+diffY] * grid[gX-2][gY] > 0) : true)
            if(knownMap[p->x+diffX][p->y-1+diffY] * grid[gX][gY-1] > 0)
            if(grid[gX][gY-2]!=0 ? (knownMap[p->x+diffX][p->y-2+diffY] * grid[gX][gY-2] > 0) : true)
                tempPoints.push_back(matchingPoints.at(i));
        }
        matchingPoints = tempPoints;
        cout << "Number of matching points : " << matchingPoints.size() <<endl;
    }
    
    if(matchingPoints.size() == 0){
        cout << "Error: position does not match known map at all!" << endl;
        return;
    }
    
    Point location(matchingPoints.front().x + diffX,matchingPoints.front().y + diffY);
    
    cout << "Found location on the map! " << endl;
    cout << "Location is : " << location.x << " " << location.y <<endl;
    
    for(y = 0; y < GRID_SIZE; y++){
        for(x = 0; x < GRID_SIZE; x++){
            grid[x][y] = knownMap[x][y];
        }
    }
    
    
    
    this->gX = location.x;
    this->gY = location.y;
    drawGrid();
}



