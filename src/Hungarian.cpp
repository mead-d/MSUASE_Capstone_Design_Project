#include <iostream>
#include <vector>
//#include <algorithm>
#include <limits>
#include <iomanip>

using namespace std;

class HungarianAlgorithm {
private:
    int n;
    vector<vector<int>> cost_matrix;
    vector<vector<int>> mask_matrix; // 0: None, 1: Star, 2: Prime
    vector<int> row_cover;
    vector<int> col_cover;
    
    // Path storage for the augmenting path step
    vector<pair<int, int>> path;
    pair<int, int> path_start;

public:
    HungarianAlgorithm(const vector<vector<int>>& input_matrix) {
        n = input_matrix.size();
        cost_matrix = input_matrix;
        mask_matrix.resize(n, vector<int>(n, 0));
        row_cover.resize(n, 0);
        col_cover.resize(n, 0);
        path.resize(n * 2 + 1); 
    }

    // --- MAIN SOLVER ---
    vector<pair<int, int>> solve() {
        // Step 1 & 2: Reduce Matrix
        step_one_and_two_reduction();

        // Step 3: Initial starring of zeros
        step_three_star_initial_zeros();

        int step = 3; // Start by checking covers
        bool done = false;
        int loops = 0;

        while (!done) {
            switch (step) {
                case 3:
                    // Cover columns containing stars. Check if we are done.
                    if (loops%1000000 == 0) {
                        cout<<"Searching... "<<loops<<endl;
                    }
                    step = step_three_cover_columns();
                    loops++;
                    break;
                case 4:
                    // Find an uncovered zero (Prime it) or decide to adjust matrix
                    step = step_four_find_prime();
                    break;
                case 5:
                    // Construct augmenting path and flip stars/primes
                    step = step_five_augment_path();
                    break;
                case 6:
                    // Adjust matrix values to create new zeros
                    step = step_six_adjust_matrix();
                    break;
                case 7:
                    done = true;
                    break;
            }
        }

        // Extract the results
        vector<pair<int, int>> assignment;
        for (int r = 0; r < n; ++r) {
            for (int c = 0; c < n; ++c) {
                if (mask_matrix[r][c] == 1) {
                    assignment.push_back({r, c});
                }
            }
        }
        return assignment;
    }

private:
    // --- STEPS IMPLEMENTATION ---

    // Steps 1 & 2: Subtract min from rows and then columns
    void step_one_and_two_reduction() {
        // Row Reduction
        for (int r = 0; r < n; ++r) {
            int min_val = numeric_limits<int>::max();
            for (int c = 0; c < n; ++c) min_val = min(min_val, cost_matrix[r][c]);
            for (int c = 0; c < n; ++c) cost_matrix[r][c] -= min_val;
        }

        // Column Reduction
        for (int c = 0; c < n; ++c) {
            int min_val = numeric_limits<int>::max();
            for (int r = 0; r < n; ++r) min_val = min(min_val, cost_matrix[r][c]);
            for (int r = 0; r < n; ++r) cost_matrix[r][c] -= min_val;
        }
    }

    // Step 3 (Part A): Star zeros where possible (Initial greedy matching)
    void step_three_star_initial_zeros() {
        for (int r = 0; r < n; ++r) {
            for (int c = 0; c < n; ++c) {
                if (cost_matrix[r][c] == 0 && row_cover[r] == 0 && col_cover[c] == 0) {
                    mask_matrix[r][c] = 1; // Star it
                    row_cover[r] = 1;
                    col_cover[c] = 1;
                }
            }
        }
        // Reset covers for the actual algorithm loop
        fill(row_cover.begin(), row_cover.end(), 0);
        fill(col_cover.begin(), col_cover.end(), 0);
    }

    // Step 3 (Part B): Cover columns containing stars.
    int step_three_cover_columns() {
        int col_count = 0;
        for (int c = 0; c < n; ++c) {
            for (int r = 0; r < n; ++r) {
                if (mask_matrix[r][c] == 1) {
                    col_cover[c] = 1;
                    col_count++;
                    break; 
                }
            }
        }
        
        if (col_count >= n) {
            return 7; // Done!
        } else {
            return 4; // Move to finding primes
        }
    }

    // Step 4: Find a zero. Prime it. Check for Star in row.
    int step_four_find_prime() {
        int row = -1, col = -1;
        bool found = find_uncovered_zero(row, col);

        if (!found) {
            return 6; // No zero found -> Go to Adjust Matrix
        }

        // Prime the zero
        mask_matrix[row][col] = 2; 

        // Check if there is a star in this row
        int star_col = find_star_in_row(row);

        if (star_col != -1) {
            // Cover row, Uncover column, continue Step 4
            row_cover[row] = 1;
            col_cover[star_col] = 0;
            return 4; 
        } else {
            // No star in row -> We found an augmenting path
            path_start = {row, col};
            return 5; 
        }
    }

    // Step 5: Construct series of alternating primes and stars
    int step_five_augment_path() {
        int count = 0;
        path[count] = path_start;
        
        while (true) {
            int star_row = find_prime_in_col(path[count].second);
            if (star_row == -1) break;
            
            count++;
            path[count] = {star_row, path[count-1].second};

            int prime_col = find_star_in_row(path[count].first);
            count++;
            path[count] = {path[count-1].first, prime_col};
        }

        // Flip assignment
        for (int i = 0; i <= count; ++i) {
            int r = path[i].first;
            int c = path[i].second;
            if (mask_matrix[r][c] == 1) mask_matrix[r][c] = 0;
            else mask_matrix[r][c] = 1;
        }

        // Clear covers and primes
        fill(row_cover.begin(), row_cover.end(), 0);
        fill(col_cover.begin(), col_cover.end(), 0);
        for (int r = 0; r < n; ++r) {
            for (int c = 0; c < n; ++c) {
                if (mask_matrix[r][c] == 2) mask_matrix[r][c] = 0;
            }
        }

        return 3; // Return to covering columns
    }

    // Step 6: Adjust the matrix constraints to reveal new zeros
    int step_six_adjust_matrix() {
        int min_val = numeric_limits<int>::max();

        // Find smallest uncovered value
        for (int r = 0; r < n; ++r) {
            for (int c = 0; c < n; ++c) {
                if (row_cover[r] == 0 && col_cover[c] == 0) {
                    min_val = min(min_val, cost_matrix[r][c]);
                }
            }
        }

        // Add/Subtract min_val
        for (int r = 0; r < n; ++r) {
            for (int c = 0; c < n; ++c) {
                if (row_cover[r] == 1) cost_matrix[r][c] += min_val;
                if (col_cover[c] == 0) cost_matrix[r][c] -= min_val;
            }
        }

        return 4; // Go back to finding primes
    }

    // --- HELPERS ---
    bool find_uncovered_zero(int& row, int& col) {
        for (int r = 0; r < n; ++r) {
            for (int c = 0; c < n; ++c) {
                if (cost_matrix[r][c] == 0 && row_cover[r] == 0 && col_cover[c] == 0) {
                    row = r;
                    col = c;
                    return true;
                }
            }
        }
        return false;
    }

    int find_star_in_row(int row) {
        for (int c = 0; c < n; ++c) {
            if (mask_matrix[row][c] == 1) return c;
        }
        return -1;
    }

    int find_prime_in_col(int col) {
        for (int r = 0; r < n; ++r) {
            if (mask_matrix[r][col] == 2) return r;
        }
        return -1;
    }
};

int main() {
    // Example: 4 Workers (Rows) and 4 Jobs (Cols)
    // Values represent the cost (time/distance) for a worker to do a job.
    vector<vector<int>> cost_matrix = {
        {23, 84, 32, 9},
        {4, 27, 28, 69},
        {48, 38, 5, 42},
        {12, 37, 49, 8}
        // {82, 83, 69, 92},
        // {77, 37, 49, 92},
        // {11, 69,  5, 86},
        // { 8,  9, 98, 23}
    };

    cout << "Original Cost Matrix:" << endl;
    for (const auto& row : cost_matrix) {
        for (int val : row) cout << setw(4) << val;
        cout << endl;
    }

    HungarianAlgorithm solver(cost_matrix);
    vector<pair<int, int>> result = solver.solve();

    cout << "\nOptimal Assignment (Row, Col):" << endl;
    int total_cost = 0;
    for (const auto& p : result) {
        cout << "Worker " << p.first << " -> Job " << p.second 
                  << " (Cost: " << cost_matrix[p.first][p.second] << ")" << endl;
        total_cost += cost_matrix[p.first][p.second];
    }
    cout << "Total Minimum Cost: " << total_cost << endl;

    return 0;
}