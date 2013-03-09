#include <utils.h>
#include <iostream>
#include <string>
#include <queue>
#include <stack>

using namespace std;



Point::Point(int x, int y){
   this->x = x;
   this->y = y;
}

Point::Point(){
   this->x = 0;
   this->y = 0;
}

TreeNode::TreeNode(TreeNode * root, Point content, Heading h){
    this->root = root;
    this->content.x = content.x;
    this->content.y = content.y;
    this->robotHeadingAtNode = h;
}

bool checkParents(TreeNode * start, Point p){
    TreeNode * current = start;
    while(current->root != NULL){
        if(current->root->content.x == p.x && current->root->content.y == p.y)
            return true;
        current = current->root;
    }
    return false;
}

vector<Point> getNeighbours(Point p){
    vector<Point> nbs; 
    if(3 < p.x < GRID_SIZE-4 && 3 < p.y-1 < GRID_SIZE-4)
    nbs.push_back(Point(p.x, p.y-1));
    if(3 < p.x+1 < GRID_SIZE-4 && 3 < p.y < GRID_SIZE-4)
    nbs.push_back(Point(p.x+1, p.y));
    if(3 < p.x < GRID_SIZE-4 && 3 < p.y+1 < GRID_SIZE-4)
    nbs.push_back(Point(p.x, p.y+1));
    if(3 < p.x-1 < GRID_SIZE-4 && 3 < p.y < GRID_SIZE-4)
    nbs.push_back(Point(p.x-1, p.y));         
    return nbs;
}

Heading getCellsHeading(Point p, Point q){
    if(p.x == q.x && q.y == p.y-1) return NORTH;
    if(p.x == q.x && q.y == p.y+1) return SOUTH;
    if(q.x == p.x-1 && q.y == p.y) return WEST;
    return EAST;
}

vector<Point*> findRoute(Point start, Point end, Robot * robot){
    
    TreeNode * treeRoot = new TreeNode(NULL, start, robot->h);
    stack<TreeNode*> items;
    vector<Point> explored;
    
    vector<Point*> correct_path;
    
    items.push(treeRoot);
    explored.push_back(start);
    
    //vector<Point*> nbs; //neighbours
    //vector<TreeNode*> nbs_nodes;
    TreeNode * current;
    TreeNode * finish = NULL;
    
     //cout << "END point: " << end.x << " " << end.y <<endl;
    
    while(!items.empty()){
        //sleep(1);
        current = items.top();
        
        //Checking if front of the queue is the target
        if(current->content.x == end.x && current->content.y == end.y){
            finish = current;
            break;
        }
        
        vector<Point> nbs; //neighbours
        vector<TreeNode*> nbs_nodes;
        
        nbs = getNeighbours(current->content);
        
        int i;
        for(i = 0; i< nbs.size(); i++){
            int j; 
            bool contains = false;
            for(j = 0; j< explored.size(); j++){
                if(explored.at(j).x == nbs.at(i).x && explored.at(j).y == nbs.at(i).y){
                    contains = true; break;
                }
            }
            if(!contains){
               //HERE NEEDS TO CHECK IF NOT OCCUPIED
               if(robot->grid[nbs.at(i).x][nbs.at(i).y] != 1)
               nbs_nodes.push_back(new TreeNode(current, nbs.at(i), getCellsHeading(current->content,nbs.at(i))));
            } 
        }
        
        if(nbs_nodes.empty()) items.pop();
        
        vector<TreeNode*> children;
        for(i = 0; i< nbs_nodes.size(); i++){
            if(nbs_nodes.at(i)->robotHeadingAtNode == current->robotHeadingAtNode)
                children.push_back(nbs_nodes.at(i));
        }
        for(i = 0; i< nbs_nodes.size(); i++){
            if(nbs_nodes.at(i)->robotHeadingAtNode != current->robotHeadingAtNode)
                children.push_back(nbs_nodes.at(i));
        }
        TreeNode * closest;
        int howClose = 9999;
        for(i = 0; i< children.size(); i++){
            int a = children.at(i)->content.x - end.x;
            int b = children.at(i)->content.y - end.y;
            int sum = (a<0 ? -a : a) + (b<0 ? -b : b);
            if(sum < howClose){
                howClose = sum;
                closest = children.at(i);
            }
        }
        
        items.push(closest);
        cout << "Closest point: " << closest->content.x << " " << closest->content.y <<endl;
        explored.push_back(Point(closest->content.x, closest->content.y));
        
        
    } 
    
    cout << "DONE!" << endl;
    
    if(finish == NULL){
        cout <<"Cannot find path" <<endl;
        return correct_path;
    }
    
    TreeNode * node = finish;
//    while(node->root != NULL){
//        cout << node->content->x << ", " << node->content->y << " -> " <<endl;
//        node = node->root;
//    }
    
    vector<Point*> path;
    node = finish;
    while(node->root != NULL){
        path.push_back(new Point(node->content.x, node->content.y));
        node = node->root;
    }
    
    
    int i;
    for(i = path.size()-1; i >=0; i--){
        correct_path.push_back(path.at(i));
        cout << path.at(i)->x << ", " << path.at(i)->y << " -> " ;
    }

    return correct_path;
    
    
}

Point findNearestUnexplored(Robot * r){
    Point start(r->gX, r->gY);
    queue<Point> items;
    vector<Point> explored;
    
    Point * current;
    
    items.push(start);
    while(!items.empty()){
        current = &items.front();
        
        if(r->grid[current->x][current->y-1] == -1 ||
           r->grid[current->x][current->y+1] == -1 ||
           r->grid[current->x-1][current->y] == -1 ||
           r->grid[current->x+1][current->y] == -1 ) return *current;
        
        vector<Point> children = getNeighbours(*current);
        if(children.empty()) continue;
        int i;
        int j;
        try{
        for(i = 0; i<children.size(); i++){
            
            bool contains = false;
            
            for(j = 0; i<explored.size(); j++){
                if(explored.at(j).x == children.at(i).x && 
                        explored.at(j).y == children.at(i).y){
                    contains = true; 
                    break;
                }     
            }
            
            if (!contains) {
                if (r->grid[children.at(i).x][children.at(i).y] != 1) {
                    items.push(children.at(i));
                    explored.push_back(children.at(i));
                }

            }
        }
        }
        catch(std::out_of_range){
            cout<< " caught vector out of range " << endl;
            cout<< " i : " << i << endl;
            cout<< " j : " << j << endl;
            cout<< children.at(i).x << endl;
            cout<< explored.at(j).x << endl;
        }
        items.pop();
    }
    return Point(-1,-1); //When no such cell is found.
}