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
    TreeNode * root;
    Point content;
    Heading robotHeadingAtNode;
    
    TreeNode(TreeNode * root, Point content, Heading h);
    bool checkParents(TreeNode * start, Point p);
};

vector<Point> getNeighbours(Point p);
vector<Point*> findRoute(Point start, Point end, Robot * robot);
Point findNearestUnexplored(Robot * r);

#endif	/* UTILS_H */

