//
// Created by l on 14.09.18.
//

#ifndef NAVI_LUEBECK_DATATYPES_H
#define NAVI_LUEBECK_DATATYPES_H

#include <string>
#include <vector>
#include <map>

struct Edge{
    int to;
    int distance;
};

struct Coordinates {
    double latitude;
    double longitude;
};

struct Node{
    std::string name;
    int id;
    std::vector<Edge> edges;
    Coordinates coordinates;
};

using Graph = std::map<int, Node>;

#endif //NAVI_LUEBECK_DATATYPES_H
