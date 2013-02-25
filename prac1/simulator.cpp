#include <iostream>
#include <string>
#include <libplayerc++/playerc++.h>
#include <robot.h>

using namespace std;
using namespace PlayerCc;

int main(int argc, char *argv[]){
	Robot r("localhost");
	r.client->Read();
	sleep(1);
	
	//starts at the left bottom of the map
	r.gX = 5;
	r.gY = 27;
	
	//r.rotate(-10);
	//r.move(0.3);
	//cout << r.sp[3] << r.sp[4] <<endl;
	//cout << r.sp <<endl;
	
	r.h = NORTH;
	r.checkProximity();
	r.applyProximityToGrid();
	int i;
	for(i =0; i<15; i++){
		r.moveToCell(NORTH);
		r.checkProximity();
		r.applyProximityToGrid();		
	}
	r.drawGrid();
}
