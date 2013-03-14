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

TreeNode::TreeNode(Point content, Heading h){
    this->content.x = content.x;
    this->content.y = content.y;
    this->robotHeadingAtNode = h;
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
    
    TreeNode * treeRoot = new TreeNode(start, robot->h);
    stack<TreeNode*> items;
    vector<Point> explored;
    
    //Checking if the end cell is not occupied
    //if is, obviously path would not exist
    if (robot->grid[end.x][end.y] < -20) {
        items.push(treeRoot);
        explored.push_back(start);
    }
    
    TreeNode * current;
    TreeNode * finish = NULL;
    
    while(!items.empty()){
        //sleep(1);
        current = items.top();
        
        //Checking if front of the queue is the target
        if(current->content.x == end.x && current->content.y == end.y){
            finish = current;
            break;
        }
        
        vector<Point> nbs; //will hold adjacent points
        vector<TreeNode*> nbs_nodes; // will hold adjacent cells as nodes in the search tree
        
        nbs = getNeighbours(current->content);
        
        int i;
        for(i = 0; i< nbs.size(); i++){
            int j; 
            bool contains = false;
            //Checking if the cell has been checked before
            for(j = 0; j< explored.size(); j++){
                if(explored.at(j).x == nbs.at(i).x && explored.at(j).y == nbs.at(i).y){
                    contains = true; break;
                }
            }
            if(!contains){
               //Checking if the cell is not occupied
               if(robot->grid[nbs.at(i).x][nbs.at(i).y] < -20)
               nbs_nodes.push_back(new TreeNode(nbs.at(i), getCellsHeading(current->content,nbs.at(i))));
            } 
        }
        
        if(nbs_nodes.empty()) items.pop();
        
        vector<TreeNode*> children;
        //Cell that lays in straight line will be added first,
        //what makes sure that it will be used at the first place, if adequate
        for(i = 0; i< nbs_nodes.size(); i++){
            if(nbs_nodes.at(i)->robotHeadingAtNode == current->robotHeadingAtNode)
                children.push_back(nbs_nodes.at(i));
        }
        //Now adding remaining cells
        for(i = 0; i< nbs_nodes.size(); i++){
            if(nbs_nodes.at(i)->robotHeadingAtNode != current->robotHeadingAtNode)
                children.push_back(nbs_nodes.at(i));
        }
        TreeNode * closest;
        int howClose = 9999;
        //Picking cell closest to destination
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
        explored.push_back(Point(closest->content.x, closest->content.y));      
    } 
    
    //If stack is empty, path is not found
    if(items.empty()){
        cout <<"Cannot find path" <<endl;
        return vector<Point*>();
    }
    
    //Otherwise, forming the path and returning it.
    stack<TreeNode*> temp;
    cout << "Items size : " << items.size() << endl;
    while(!items.empty()){
        temp.push(items.top());
        items.pop();
    }
    cout << "Temp size : " << temp.size() << endl;
    vector<Point*> correct_path;
    while(!temp.empty()){
        correct_path.push_back(new Point(temp.top()->content.x, 
                temp.top()->content.y));
        temp.pop();
    }
    cout << "path size : " << correct_path.size() << endl;
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
        
        //Checking if the cell is near unexplored cell
        if((-20 <= r->grid[current->x][current->y-1] && r->grid[current->x][current->y-1]<= 20 )||
           (-20 <= r->grid[current->x][current->y+1] && r->grid[current->x][current->y+1]<= 20 )||
           (-20 <= r->grid[current->x-1][current->y] && r->grid[current->x-1][current->y]<= 20 )||
           (-20 <= r->grid[current->x+1][current->y] && r->grid[current->x+1][current->y]<= 20 )) return *current;
        
        vector<Point> children = getNeighbours(*current);
        if(children.empty()) continue;
        int i;
        int j;    
        for(i = 0; i<children.size(); i++){
            
            bool contains = false;
            //Checking if the cell has already been checked
            for(j = 0; j<explored.size(); j++){
                if(explored.at(j).x == children.at(i).x && 
                        explored.at(j).y == children.at(i).y){
                    contains = true; 
                    break;
                }     
            }
            
            if (!contains) {
                if (r->grid[children.at(i).x][children.at(i).y] < -20) {
                    items.push(children.at(i));
                    explored.push_back(children.at(i));
                }

            }
        }
       
        items.pop();
    }
    return Point(-1,-1); //When no such cell is found.
}



vector<string> split(const string &s, char delim) {
    vector<std::string> elems;
    stringstream ss(s);
    string item;
    while(getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}
