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

using namespace std;


class Point{
public:
    int x;
    int y;
    
    Point(int x, int y);
};


class TreeNode{
public:
    TreeNode * root;
    Point * content;
    Heading robotHeadingAtNode;
    vector<TreeNode*> children;
    
    TreeNode(TreeNode * root, Point * content, Heading h);
    bool checkParents(TreeNode * start, Point p);
};

vector<Point*> getNeighbours(Point p);
string findRoute(Point start, Point end, Robot * robot);

#endif	/* UTILS_H */

