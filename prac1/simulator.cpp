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
    Robot r("lisa.islnet");
#else
    Robot r("localhost");
#endif	


    r.client->Read();
    sleep(1);

    //starts at the left bottom of the map
    r.gX = 25;
    r.gY = 34;
    r.h = NORTH;

    /*
    r.rotate(-180);
    r.move(0.8);
    r.rotate(-90);
    r.move(0.8);
    r.rotate(-90);
    r.move(0.8);
    r.rotate(-90);
    r.move(0.8);
    r.rotate(-90);
    */
    
    
    
     r.loadGridFromFile("dpa");
     r.recognisePosition();
     r.findHiddenRobot();
     
     
    /*
    r.exploreWorld();
    r.saveGridToFile("dupa");
    vector<Point> hideouts = r.findHidingSpots();
    Point hideout = hideouts.at(0);
    r.move(findRoute(Point(r.gX, r.gY), hideout, &r));
    cout << "I am hidden!" << endl;
    */
    
      
        
}
