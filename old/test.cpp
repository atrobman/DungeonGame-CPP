#include "pathfinding_test.hpp"
#include <stdio.h>
#include <time.h>

int main() {

    printf("Testing pathfinding from (0, 0) to (19, 19)\n");

    Pathfinder p = Pathfinder(20, 20);
    Pathfinder::Node start = Pathfinder::Node(0, 0);
    Pathfinder::Node end = Pathfinder::Node(19, 19);

    p.makeWall(0, 1);
    p.makeWall(1, 1);

    std::vector<Pathfinder::Node> path = p.aStar(start, end);

    // p.setWeight(0, 1, 1e10);
    // p.setWeight(1, 1, 1e10);

    printf("Pathfinding finish\n");
    for (Pathfinder::Node node : path) {
        printf("(%d, %d)\n", node.x, node.y);
    }
}