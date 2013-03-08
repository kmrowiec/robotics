/* 
 * File:   others.h
 * Author: kamil
 *
 * Created on 08 March 2013, 04:09
 */

#ifndef OTHERS_H
#define	OTHERS_H

class Point{
public:
    int x;
    int y;
    
    Point(int x, int y);
    Point();
};


enum Heading{
        NORTH = 1,
	EAST = 2,
	SOUTH = 3,
	WEST = 4
};

#endif	/* OTHERS_H */

