//
// Created by l on 12.09.18.
//

#ifndef NAVI_LUEBECK_DIJKSTRA_H
#define NAVI_LUEBECK_DIJKSTRA_H

#include <list>
#include <map>
#include <vector>

#include "DataTypes.h"

class Dijkstra {
private:
    static constexpr int NO_PREDECESSOR = -1;
    Graph graph;
    const int start_node;
    std::vector<int> queue;
    std::map<int, int> distances;
    std::map<int, int> predecessors;
    void init ();
    int next_node ();
    void mark_node_visited(int node_id);
    void process_node(int node_id);
    void find_shortest_paths ();
public:
    Dijkstra (Graph graph, int start_node);
    std::list<int> get_route (int target_id);
};

#endif //NAVI_LUEBECK_DIJKSTRA_H
