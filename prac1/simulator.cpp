#include <iostream>
#include <string>
#include <libplayerc++/playerc++.h>
#include <robot.h>
#include <utils.h>

using namespace std;
using namespace PlayerCc;

int main(int argc, char *argv[]){

	#ifdef ROBOT
		Robot r("bart.islnet");
	#else
		Robot r("localhost");
	#endif	
		
		
	r.client->Read();
	sleep(1);
	
	//starts at the left bottom of the map
	r.gX = 5;
	r.gY = 25;
	
	//r.moveToCell(NORTH);
	//r.moveToCell(NORTH);
	
	//r.rotate(-10);
	//r.move(0.3);
	//cout << r.sp[3] << r.sp[4] <<endl;
	//cout << r.sp <<endl;
        /*
	sleep(1);
	cout << "Current heading: "<< r.h << endl;
	sleep(2);
	r.changeHeading(WEST);
	cout << "Current heading: "<< r.h << endl;
	sleep(2);
	r.changeHeading(SOUTH);
	cout << "Current heading: "<< r.h << endl;	
	sleep(2);
	r.changeHeading(EAST);
	cout << "Current heading: "<< r.h << endl;
	*/
	//return 1;
        
        r.h = EAST;
        
        Point s(5,25);
        Point e(10, 25);
        vector<Point*> route = findRoute(s, e, &r);
	int i = 0;
        for(i; i< route.size(); i++){
            r.moveToNearbyCell(*(route.at(i)));
        }
        
        return 0;
	
	//r.h = NORTH;
        r.changeHeading(EAST);
	r.checkProximity();
	r.applyProximityToGrid();
	
	
	for(i =0; i<10; i++){
		r.moveToCell(NORTH);
		r.checkProximity();
		r.applyProximityToGrid();		
	}
	r.drawGrid();
}
