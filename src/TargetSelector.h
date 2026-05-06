#ifndef TARGET_SELECTOR_H
#define TARGET_SELECTOR_H
#include <vector>
#include <cmath>

struct Point { double x, y; };

struct Edge {
    int node_id;
    int target_id;
    double dist_sq; // Use squared distance to avoid sqrt() floating-point drift

    // STRICT DETERMINISM: Crucial for distributed system
    bool operator<(const Edge& other) const {
        // 1. Sort by distance
        if (std::abs(dist_sq - other.dist_sq) > 1e-9) {
            return dist_sq < other.dist_sq;
        }
        // 2. Tie-breaker: Node ID
        if (node_id != other.node_id) {
            return node_id < other.node_id;
        }
        // 3. Tie-breaker: Target ID
        return target_id < other.target_id;
    }
};

struct Data { int target; vector<Edge> cost_matrix; };

Data getMyTarget_greedy(int node_id, std::vector<Point>& all_nodes, std::vector<Point>& all_targets);

#endif