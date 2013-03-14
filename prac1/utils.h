/* 
 * File:   utils.h
 * Author: kamil
 *
 * Created on 08 March 2013, 00:42
 */


#ifndef UTILS_H
#define	UTILS_H


#include <vector>
#include <robot.h>
#include <others.h>

using namespace std;

class TreeNode{
public:
    Point content;
    Heading robotHeadingAtNode;
    
    TreeNode(Point content, Heading h);
    bool checkParents(TreeNode * start, Point p);
};

vector<Point> getNeighbours(Point p);
vector<Point*> findRoute(Point start, Point end, Robot * robot);
Point findNearestUnexplored(Robot * r);
vector<string> split(const string &s, char delim);

#endif	/* UTILS_H */

