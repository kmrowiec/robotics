#include <iostream>
#include <string>
#include <libplayerc++/playerc++.h>

using namespace PlayerCc;
using namespace std;

//0.001 for robot , 0.1 for simulator
const float ROTATE_ERROR = 0.1;
const float ROTATE_SLOW_SPEED = 2; //10 for robot, 5 for sim


class Robot{
	
	public:
	PlayerClient* client;
	RangerProxy* sp;
	Position2dProxy* pp;
	
	/*
	void rotate(int degrees);
	void move(double distance);
	float calcAngularDistance(float a1, float a2);
	void moveTo(char c);
	*/
	
	public:
	Robot(string cl){
		client = new PlayerClient(cl);
		sp = new RangerProxy(client, 0);
		pp = new Position2dProxy(client,0);
		pp->SetMotorEnable(true);
	}
	
	void rotate(int degrees){

    client->Read();
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
      
      client->Read(); // reading sensors
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
	} //end of rotate function

private :
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

};//end of robot class

int main(int argc, char *argv[]){
	Robot r("localhost");
	sleep(3);
	r.rotate(60);
}
