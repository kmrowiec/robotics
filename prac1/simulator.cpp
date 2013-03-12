#include <iostream>

#include <iostream>
#include <string>
#include <libplayerc++/playerc++.h>
#include <robot.h>
#include <utils.h>

using namespace std;
using namespace PlayerCc;


int main(int argc, char *argv[]) {

#ifdef ROBOT
    Robot r("marge.islnet");
#else
    Robot r("localhost");
#endif	


    r.client->Read();
    sleep(1);

    //starts at the left bottom of the map
    r.gX = 5;
    r.gY = 35;
    r.h = EAST;


    r.exploreWorld();
    //vector<Point> hideouts = r.findHidingSpots();
    //Point hideout = hideouts.at(0);
    //r.move(findRoute(Point(r.gX, r.gY), hideout, &r));
    //r.saveGridToFile("dupa");
    
      
        
}
