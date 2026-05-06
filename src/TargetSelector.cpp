#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <unordered_set>
#include "TargetSelector.h"

// Function runs on EVERY node
Data getMyTarget_greedy(int node_id, 
                std::vector<Point>& all_nodes, 
                std::vector<Point>& all_targets) {
    
    int N = all_nodes.size();
    std::vector<Edge> edges;
    edges.reserve(N * N);

    // Build the full N x N cost graph
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            double dx = all_nodes[i].x - all_targets[j].x;
            double dy = all_nodes[i].y - all_targets[j].y;
            edges.push_back({i, j, (dx * dx) + (dy * dy)});
        }
    }

    // Deterministic Sort
    std::sort(edges.begin(), edges.end());

    std::vector<bool> node_assigned(N, false);
    std::vector<bool> target_assigned(N, false);
    int my_assigned_target = -1;

    for (const auto& edge : edges) {
        if (!node_assigned[edge.node_id] && !target_assigned[edge.target_id]) {
            node_assigned[edge.node_id] = true;
            target_assigned[edge.target_id] = true;

            // If this assignment belongs to the current node, save it
            if (edge.node_id == node_id) {
                my_assigned_target = edge.target_id;
                // We don't break early; we must simulate the whole process 
                // exactly as other nodes do to maintain state consistency, 
                // though technically for Greedy we could stop once 'my' node is found.
                break; 
            }
        }
    }

    return Data{my_assigned_target, edges};
}