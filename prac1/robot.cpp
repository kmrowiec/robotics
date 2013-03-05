#include <iostream>
#include <string>
#include <libplayerc++/playerc++.h>
#include <robot.h>

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
            grid[x][y] = -1;
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

    //Caclulating expected angle (in degrees)
    //If ig goes above 181, need to substract -360
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
		
		/*
        std::cout << "Started at : " << initialAngle << std::endl;
        std::cout << "Current angle: " << currentAngle << std::endl;
        std::cout << "Going to: " << expectedAngle << std::endl;
        std::cout << "Distance: " << distance << std::endl;
		*/

        if (distance < ROTATE_ERROR) {
            #ifdef ROBOT
            int i = 0;
            speed = 10;
            if(degrees<0)
            pp->SetSpeed(0,dtor(-speed));
            else
            pp->SetSpeed(0,dtor(speed));
            usleep(600000);
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
			move(0.6);
			break;
        case NORTH:
            move(0.6);
            //changing xy position on the grid
            if (h == NORTH) gY--;
            else if (h == EAST) gX++;
            else if (h == WEST) gX--;
            else gY++;
            break; 
    }
    
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
        if (cDistance - distance >= -0.005) break;
        if (cDistance - distance >= -0.2) pp->SetSpeed(0.1, 0);
        else pp->SetSpeed(0.2, 0);

    }

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
    p[0][1] = -1;
    p[4][1] = -1;
    p[0][3] = -1;
    p[4][3] = -1;

    //Front cells
    if ((*sp)[3] < 0.6 && (*sp)[4] < 0.6) {
        p[2][1] = 1;
        p[2][0] = -1;
        p[3][0] = -1;
        p[1][0] = -1;
        p[1][1] = -1;
        p[3][1] = -1;
        p[0][0] = -1;
        p[4][0] = -1;
    } else { //if front cell is empty then all other in front can be checked
        p[2][1] = 0;
        if ((*sp)[3] < 1.2 && (*sp)[4] < 1.2) {
            p[2][0] = 1;
        } else {
            p[2][0] = 0;
        }
        //checking diagonally to the left
        if ((*sp)[1] < 1) {
            p[1][1] = 1;
            p[0][0] = -1;
        } else { //if 11 is empty
            p[1][1] = 0;
            if ((*sp)[1] < 1.8) {
                p[0][0] = 1;
            } else p[0][0] = 0;
        }
        if ((*sp)[2] < 1.5) {
            p[1][0] = 1;
        } else p[1][0] = 0;

        //checking diagonally to the right
        if ((*sp)[6] < 1) {
            p[3][1] = 1;
            p[4][0] = -1;
        } else {
            p[3][1] = 0;
            if ((*sp)[6] < 1.8) {
                p[4][0] = 1;
            } else p[4][0] = 0;
        }
        if ((*sp)[5] < 1.5) {
            p[3][0] = 1;
        } else p[3][0] = 0;

    }//end of checking cells to the front

    //And mirroring it for back cells
    if ((*sp)[11] < 0.6 && (*sp)[12] < 0.6) {
        p[2][3] = 1;
        p[4][4] = -1;
        p[1][3] = -1;
        p[3][3] = -1;
        p[4][3] = -1;
        p[2][4] = -1;
        p[0][4] = -1;
        p[3][4] = -1;
        p[1][4] = -1;
    } else { //if cell behing is empty then all other can be checked
        p[2][3] = 0;
        if ((*sp)[11] < 1.2 && (*sp)[12] < 1.2) {
            p[2][4] = 1;
        } else {
            p[2][4] = 0;
        }
        //checking diagonally to the left
        if ((*sp)[14] < 1) {
            p[1][3] = 1;
            p[0][4] = -1;
        } else { //if 11 is empty
            p[1][3] = 0;
            if ((*sp)[14] < 1.8) {
                p[0][4] = 1;
            } else p[0][4] = 0;
        }
        if ((*sp)[13] < 1.5) {
            p[1][4] = 1;
        } else p[1][4] = 0;

        //checking diagonally to the right
        if ((*sp)[9] < 1) {
            p[3][3] = 1;
            p[4][4] = -1;
        } else {
            p[3][3] = 0;
            if ((*sp)[9] < 1.8) {
                p[4][4] = 1;
            } else p[4][4] = 0;
        }
        if ((*sp)[10] < 1.5) {
            p[3][4] = 1;
        } else p[3][4] = 0;
    }
    //And finally checking the sides
    if ((*sp)[0] < 0.6 && (*sp)[15] < 0.6) {
        p[1][2] = 1;
        p[0][2] = -1;
    } else {
        p[1][2] = 0;
        if ((*sp)[0] < 1.2 && (*sp)[15] < 1.2) {
            p[0][2] = 1;
        } else p[0][2] = 0;
    }

    if ((*sp)[6] < 0.6 && (*sp)[7] < 0.6) {
        p[3][2] = 1;
        p[4][2] = -1;
    } else {
        p[3][2] = 0;
        if ((*sp)[6] < 1.2 && (*sp)[7] < 1.2) {
            p[4][2] = 1;
        } else p[4][2] = 0;
    }

    //Printing out sensors reading and proximity.
    cout << *sp <<endl;
    cout << p[0][0] << p[1][0] << p[2][0] << p[3][0] << p[4][0] <<endl;
    cout << p[0][1] << p[1][1] << p[2][1] << p[3][1] << p[4][1] <<endl;
    cout << p[0][2] << p[1][2] << "R" << p[3][2] << p[4][2] <<endl;
    cout << p[0][3] << p[1][3] << p[2][3] << p[3][3] << p[4][3] <<endl;
    cout << p[0][4] << p[1][4] << p[2][4] << p[3][4] << p[4][4] <<endl;		
}

/**
 * Applies the information about cells nearby to the occupancy grid. 
 */
void Robot::applyProximityToGrid() {
    int sX, sY;
    int x, y;
    switch (h) {
        case NORTH:
            sX = gX - 2;
            sY = gY - 2;
            for (y = 0; y < 5; y++) {
                for (x = 0; x < 5; x++) {
                    if (p[x][y] != -1)
                        grid[sX + x][sY + y] = p[x][y];
                }
            }
            break;
        case EAST:
            sX = gX + 2;
            sY = gY - 2;
            for (y = 0; y < 5; y++) {
                for (x = 0; x < 5; x++) {
                    grid[sX - y][sY + x] = p[x][y];
                }
            }
            break;
        case WEST:
            sX = gX - 2;
            sY = gY + 2;
            for (y = 0; y < 5; y++) {
                for (x = 0; x < 5; x++) {
                    grid[sX + y][sY - x] = p[x][y];
                }
            }
            break;
        case SOUTH:
            sX = gX + 2;
            sY = gY + 2;
            for (y = 0; y < 5; y++) {
                for (x = 0; x < 5; x++) {
                    grid[sX - x][sY - y] = p[x][y];
                }
            }
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
            switch (grid[x][y]) {
                case -1:
                    cout << "X";
                    break;
                case 0:
                    cout << " ";
                    break;
                case 1:
                    cout << "#";
                    break;

            }
            //cout << grid[x][y] ;
        }
        cout << endl;
    }
}



