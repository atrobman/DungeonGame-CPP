#ifndef PATHFINDING_TEST_H
#define PATHFINDING_TEST_H

#include <math.h>
#include <vector>
#include <stack>

class Pathfinder {

public:

    class Node {
        public:
            int x;
            int y;
            int parentX;
            int parentY;
            float gCost;
            float hCost;
            float fCost;
            float weight;
            bool wall;

            Node() {;}
            Node(int x_, int y_) {
                x = x_;
                y = y_;
            }

            friend bool operator< (const Node& lhs, const Node& rhs) {
                return lhs.fCost < rhs.fCost;
            }

            friend bool operator== (const Node& lhs, const Node& rhs) {
                return lhs.x == rhs.x && lhs.y == rhs.y;
            }
    };

    Pathfinder(int w, int h) {
        width = w;
        height = h;
        
        for (int i = 0; i < w; i ++) {
            std::vector<std::pair<Node, bool>> row;
            for (int j = 0; j < h; j ++) {
                Node n = Node();
                n.x = i;
                n.y = j;
                n.fCost = __FLT_MAX__;
                n.gCost = __FLT_MAX__;
                n.hCost = __FLT_MAX__;
                n.parentX = -1;
                n.parentY = -1;
                n.wall = false;

                row.push_back( std::make_pair(n, false) );
            }
            map.push_back(row);
        }
    }

    std::vector<Node> aStar(Node start, Node goal) {

        std::vector<Node> empty;

        if ( !isValid(start) || !isValid(goal) || start == goal) {
            return empty;
        }

        int x = start.x;
        int y = start.y;
        map[x][y].first.fCost = 0.0;
        map[x][y].first.gCost = 0.0;
        map[x][y].first.hCost = 0.0;
        map[x][y].first.parentX = x;
        map[x][y].first.parentY = y;

        std::vector<Node> openList;
        openList.emplace_back(map[x][y].first);

        while (!openList.empty() && openList.size() < (size_t)(width * height) ) {
            Node node;

            do {
                
                float temp = __FLT_MAX__;
                std::vector<Node>::iterator itNode;
                for (std::vector<Node>::iterator it = openList.begin();
                     it != openList.end();
                     it = std::next(it)) {

                    Node n = *it;
                    if (n.fCost < temp) {
                        temp = n.fCost;
                        itNode = it;
                    }
                }
                node = *itNode;
                openList.erase(itNode);

            } while (!isValid(node));

            x = node.x;
            y = node.y;
            map[x][y].second = true;

            for (int newX = -1; newX <= 1; newX ++) {
                for (int newY = -1; newY <= 1; newY ++) {
                    double gNew, hNew, fNew;

                    if (isValid(x + newX, y + newY) && !map[x + newX][y + newY].first.wall) {

                        if (x + newX == goal.x && y + newY == goal.y) {
                            map[x + newX][y + newY].first.parentX = x;
                            map[x + newX][y + newY].first.parentY = y;
                            return makePath(goal);
                        } else if (!map[x + newX][y + newY].second) {
                            gNew = node.gCost + 1.0;
                            hNew = calcH(node, goal);
                            fNew = gNew + hNew;
                        
                            if (map[x + newX][y + newY].first.fCost == -1 ||
                                map[x + newX][y + newY].first.fCost > fNew) {

                                map[x + newX][y + newY].first.fCost = fNew;
                                map[x + newX][y + newY].first.gCost = gNew;
                                map[x + newX][y + newY].first.hCost = hNew;
                                map[x + newX][y + newY].first.parentX = x;
                                map[x + newX][y + newY].first.parentY = y;
                                openList.emplace_back(map[x + newX][y + newY].first);
                            }
                        }
                    }
                }
            }
        }

        return empty;
    }

    void makeWall(int x, int y) {

        if (isValid(x, y)) {
            map[x][y].first.wall = true;
        }

    }
    
    void removeWall(int x, int y) {

        if (isValid(x, y)) {
            map[x][y].first.wall = false;
        }
        
    }

private:

    std::vector<Node> makePath(Node node) {

        int x = node.x;
        int y = node.y;

        std::stack<Node> path;
        std::vector<Node> usablePath;

        while (!(map[x][y].first.parentX == x && map[x][y].first.parentY == y)
              && map[x][y].first.x != -1 && map[x][y].first.y != -1) {

            path.push(map[x][y].first);
            int tempX = map[x][y].first.parentX;
            int tempY = map[x][y].first.parentY;
            x = tempX;
            y = tempY;
        }

        path.push(map[x][y].first);
        
        while (!path.empty()) {
            Node top = path.top();
            path.pop();
            usablePath.emplace_back(top);
        }


        return usablePath;
    }

    bool isValid(Node n) {
        return (n.x >= 0 && n.x < width && n.y >= 0 && n.y < height); 
    }

    bool isValid(int x, int y) {
        return (x >= 0 && x < width && y >= 0 && y < height);
    }

    double calcH(Node n, Node goal) {
        double H = (sqrt(
                        (n.x - goal.x)*(n.x - goal.x)
                      + (n.y - goal.y)*(n.y - goal.y)
                        ));
        return H;
    }

    int width;
    int height;
    std::vector<std::vector<std::pair<Node, bool>>> map;
};

#endif