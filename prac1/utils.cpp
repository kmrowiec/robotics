#include <utils.h>
#include <iostream>
#include <string>
#include <queue>

using namespace std;



Point::Point(int x, int y){
   this->x = x;
   this->y = y;
}

TreeNode::TreeNode(TreeNode * root, Point * content, Heading h){
    this->root = root;
    this->content = content;
    this->robotHeadingAtNode = h;
}

bool checkParents(TreeNode * start, Point p){
    TreeNode * current = start;
    while(current->root != NULL){
        if(current->root->content->x == p.x && current->root->content->y == p.y)
            return true;
        current = current->root;
    }
    return false;
}

vector<Point*> getNeighbours(Point p){
    vector<Point*> nbs;
    nbs.push_back(new Point(p.x, p.y-1));
    nbs.push_back(new Point(p.x+1, p.y));
    nbs.push_back(new Point(p.x, p.y+1));
    nbs.push_back(new Point(p.x-1, p.y));
    return nbs;
}

Heading getCellsHeading(Point p, Point q){
    if(p.x == q.x && q.y == p.y-1) return NORTH;
    if(p.x == q.x && q.y == p.y+1) return SOUTH;
    if(q.x == p.x-1 && q.y == p.y) return WEST;
    return EAST;
}

string findRoute(Point start, Point end, Robot * robot){
    
    TreeNode * treeRoot = new TreeNode(NULL, &start, robot->h);
    queue<TreeNode*> items;
    
    items.push(treeRoot);
    
    vector<Point*> nbs; //neighbours
    vector<TreeNode*> nbs_nodes;
    TreeNode * current;
    TreeNode * finish;
    while(1){
        if(items.front()->content->x == end.x && items.front()->content->y == end.y){
            finish = items.front();
            break;
        }
        nbs.clear();
        nbs_nodes.clear();
        current = items.front();
        items.pop();
        nbs = getNeighbours(*(current->content));
        int i;
        for(i = 0; i< nbs.size(); i++){
            nbs_nodes.push_back(new TreeNode(current, nbs.at(i), getCellsHeading(*(current->content),*(nbs.at(i)))));
        }
        //Adding all neighbours to items and to tree,
        //in first order those that do not require turning
        current->children = nbs_nodes;
        for(i = 0; i< nbs.size(); i++){
            if(nbs_nodes.at(i)->robotHeadingAtNode == current->robotHeadingAtNode)
                items.push(nbs_nodes.at(i));
        }
        for(i = 0; i< nbs.size(); i++){
            if(nbs_nodes.at(i)->robotHeadingAtNode != current->robotHeadingAtNode)
                items.push(nbs_nodes.at(i));
        }
        
    }
    
    cout << "DONE!" << endl;
    
    
    
}