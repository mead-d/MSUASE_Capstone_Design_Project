#ifndef TARGET_SELECTOR_H
#define TARGET_SELECTOR_H
#include <vector>

struct Point { double x, y; };

int getMyTarget_greedy(int node_id, std::vector<Point>& all_nodes, std::vector<Point>& all_targets);

#endif