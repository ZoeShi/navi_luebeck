//
// Created by l on 09.09.18.
//

#include <algorithm>

#include "dijkstra.h"

Dijkstra::Dijkstra (Graph graph, int start_node) : graph (graph), start_node (start_node) {
    init ();
    find_shortest_paths ();
}

void Dijkstra::init () {
    for(auto &node_pair : graph) {
        queue.push_back(node_pair.first);
        // Use maximum integer to represent infinity
        // Warning: this _will_ break for nodes with *very* long distances
        distances[node_pair.first] = std::numeric_limits<int>::max();
        predecessors[node_pair.first] = NO_PREDECESSOR;
    }
    distances[start_node] = 0;
}

// Find node with highest priority (= min distance to start node) that has not been processed
int Dijkstra::next_node () {
    auto it = std::min_element (std::begin (queue), std::end (queue), [this] (const int l, const int r) {
        return distances[l] < distances[r];
    });
    return *it;
}

void Dijkstra::mark_node_visited(int node_id) {
    auto it = std::find (std::begin(queue), std::end (queue), node_id);
    queue.erase (it);
}

void Dijkstra::find_shortest_paths () {
    while (!queue.empty ()) {
        auto node_id = next_node ();
        mark_node_visited (node_id);
        process_node (node_id);
    }
}

void Dijkstra::process_node (int node_id) {
    for (auto &edge : graph[node_id].edges) {
        // If node that edge connects to has already been visited (= is no longer in the queue), ignore it
        if (std::find (std::begin(queue), std::end (queue), edge.to) == end (queue)) {
            continue;
        }
        // Check if edge connected node is a shorter alternative
        auto alternative = distances[node_id] + edge.distance;
        if (alternative < distances[edge.to]) {
            // Update distance and predecessor information
            distances[edge.to] = alternative;
            predecessors[edge.to] = node_id;
        }
    }
}

std::list<int> Dijkstra::get_route (int target_id) {
    std::list<int> route;
    route.push_front(target_id);
    while (predecessors[target_id] != NO_PREDECESSOR) {
        target_id = predecessors[target_id];
        route.push_front(target_id);
    }
    return route;
}
