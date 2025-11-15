#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <cmath>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <random>
#include <climits>
#include <limits>
#include <cstdint>
#include <chrono>
#include <numeric>

// ==================== DATA & HELPER FUNCTIONS ====================

bool getDataFromFile(const std::string &filename, std::vector<std::vector<int>> &data)
{
    std::ifstream file(filename);
    if (!file.is_open())
    {
        std::cerr << "Error: Could not open the file: " << filename << std::endl;
        return false;
    }

    std::string line;
    while (std::getline(file, line))
    {
        std::stringstream ss(line);
        std::string value;
        std::vector<int> row;
        while (std::getline(ss, value, ';'))
        {
            try { row.push_back(std::stoi(value)); }
            catch (...) { return false; }
        }
        data.push_back(row);
    }
    file.close();
    return true;
}

int getEuclidanDistance(int x1, int y1, int x2, int y2)
{
    return static_cast<int>(std::sqrt(std::pow(x2 - x1, 2) + std::pow(y2 - y1, 2)));
}

int **getDistanceMatrix(std::vector<std::vector<int>> &data, int &size)
{
    int **distanceMatrix = new int *[size];
    for (int i = 0; i < size; i++)
    {
        distanceMatrix[i] = new int[size];
        for (int j = 0; j < size; j++)
        {
            if (i == j) distanceMatrix[i][j] = 0;
            else distanceMatrix[i][j] = getEuclidanDistance(data[i][0], data[i][1], data[j][0], data[j][1]);
        }
    }
    return distanceMatrix;
}

std::vector<int> getCostVector(std::vector<std::vector<int>> &data)
{
    std::vector<int> nodeCosts;
    for (const auto &row : data) nodeCosts.push_back(row[2]);
    data.clear();
    return nodeCosts;
}

int evaluateSolution(const std::vector<int> &solution, int **distanceMatrix, const std::vector<int> &costVector)
{
    int totalCost = 0;
    if (solution.empty()) return 0;
    for (size_t i = 0; i < solution.size(); ++i)
    {
        totalCost += costVector[solution[i]];
        totalCost += distanceMatrix[solution[i]][solution[(i + 1) % solution.size()]];
    }
    return totalCost;
}

std::vector<int> randomPermutation(int size, std::mt19937 &g)
{
    int nodesToVisit = (size % 2 == 0) ? (size / 2) : ((size + 1) / 2);
    std::vector<int> solution(size);
    std::iota(solution.begin(), solution.end(), 0);
    std::shuffle(solution.begin(), solution.end(), g);
    solution.resize(nodesToVisit);
    return solution;
}

void reverseCircularSegment(std::vector<int> &solution, int pos1, int pos2)
{
    int n = static_cast<int>(solution.size());
    if (n == 0) return;
    
    pos1 = (pos1 % n + n) % n;
    pos2 = (pos2 % n + n) % n;

    int halfLen;
    if (pos1 <= pos2) halfLen = (pos2 - pos1 + 1) / 2;
    else halfLen = (n - pos1 + pos2 + 1) / 2;

    for (int i = 0; i < halfLen; ++i)
    {
        std::swap(solution[pos1], solution[pos2]);
        pos1 = (pos1 + 1) % n;
        pos2 = (pos2 - 1 + n) % n;
    }
}

// ==================== ASSIGNMENT 5: LM & LAZY EVAL LOGIC ====================

struct Move {
    int type; // 1 = Intra (2-opt), 2 = Inter (Swap In-Node with Out-Node)
    int delta;
    
    // For Intra (2-opt): edges (u, u_next) and (v, v_next) are broken.
    // For Inter (Swap): node u (IN) is replaced by node v (OUT).
    int u, u_next; // Used for Type 1 & Type 2 (u is the node being removed)
    int v, v_next; // Used for Type 1. For Type 2, v is the *replacement* node.
};

bool compareMoves(const Move &a, const Move &b) {
    return a.delta < b.delta;
}

// Returns: 1 (Forward), -1 (Reversed), 0 (Broken/Non-existent)
int checkEdge(int u, int v, const std::vector<int>& sol, const std::vector<int>& pos) {
    int n = sol.size();
    int u_idx = pos[u];
    int v_idx = pos[v];

    if (u_idx == -1 || v_idx == -1) return 0; 

    int u_next_idx = (u_idx + 1) % n;
    int u_prev_idx = (u_idx - 1 + n) % n;

    if (u_next_idx == v_idx) return 1;  
    if (u_prev_idx == v_idx) return -1; 
    return 0;
}

#define dist(a, b) distanceMatrix[a][b]
#define cost(a) costVector[a]

void generateMoves(
    int **distanceMatrix,
    const std::vector<int> &costVector,
    const std::vector<int> &solution,
    const std::vector<int> &pos, // pos[node] = index in solution, or -1 if not in solution
    std::vector<Move> &LM,
    bool fullScan,
    int totalNodes, // Total nodes in problem instance (not just solution size)
    const std::vector<int> &nodesToCheck = {}) // Only used if !fullScan
{
    int n = solution.size();

    // We need two scanning logics:
    // 1. Intra-Route (2-opt): Scan edges inside solution
    // 2. Inter-Route (Node Exchange): Scan nodes IN solution vs nodes OUT of solution

    auto addIntraMoves = [&](int u_idx) {
        int u = solution[u_idx];
        int u_next_idx = (u_idx + 1) % n;
        int u_next = solution[u_next_idx];

        for (int v_idx = 0; v_idx < n; ++v_idx) {
             if (u_idx == v_idx || u_next_idx == v_idx) continue;
             
             int v = solution[v_idx];
             int v_next_idx = (v_idx + 1) % n;
             if (v_next_idx == u_idx) continue; // Adjacent edge

             int v_next = solution[v_next_idx];

             int delta = (dist(u, v) + dist(u_next, v_next)) - (dist(u, u_next) + dist(v, v_next));
             if (delta < 0) {
                 LM.push_back({1, delta, u, u_next, v, v_next});
             }
        }
    };

    auto addInterMoves = [&](int u_idx) {
        int u = solution[u_idx];
        int u_prev = solution[(u_idx - 1 + n) % n];
        int u_next = solution[(u_idx + 1) % n];
        
        // Check against all nodes NOT in solution
        for (int v = 0; v < totalNodes; ++v) {
            if (pos[v] != -1) continue; // v is already in solution

            // Delta = New Cost - Old Cost
            // Remove u: -(dist(u_prev, u) + dist(u, u_next) + cost(u))
            // Add v:    +(dist(u_prev, v) + dist(v, u_next) + cost(v))
            
            int current_cost = dist(u_prev, u) + dist(u, u_next) + cost(u);
            int new_cost = dist(u_prev, v) + dist(v, u_next) + cost(v);
            
            int delta = new_cost - current_cost;
            
            if (delta < 0) {
                // We store u (node to remove) and v (node to add)
                // We don't need v_next for Type 2
                LM.push_back({2, delta, u, -1, v, -1}); 
            }
        }
    };

    if (fullScan) {
        // Scan all current edges for 2-opt
        for (int i = 0; i < n; ++i) {
            // Optimization: 2-opt is symmetric, only check j > i essentially
            // But for simplicity of loop structure above, we can loop all.
            // Better:
            for (int j = i + 2; j < n + (i > 0 ? 0 : -1); ++j) {
                 int u = solution[i];
                 int u_next = solution[(i+1)%n];
                 int v = solution[j];
                 int v_next = solution[(j+1)%n];
                 int delta = (dist(u, v) + dist(u_next, v_next)) - (dist(u, u_next) + dist(v, v_next));
                 if(delta < 0) LM.push_back({1, delta, u, u_next, v, v_next});
            }
        }
        // Scan all nodes for Inter-Exchange
        for (int i = 0; i < n; ++i) {
            addInterMoves(i);
        }
    } 
    else {
        // Incremental update
        // We only check moves involving 'nodesToCheck'
        for (int node : nodesToCheck) {
            int idx = pos[node];
            if (idx != -1) {
                // Node is in solution: Check 2-opt starting here, and Swaps removing this
                addInterMoves(idx); 
                // For 2-opt, we should strictly check all edges, but checking edges connected to 'node' is sufficient approximation
                // We check edges (node, node_next) and (node_prev, node) against all others
                // This is expensive to do perfectly incrementally, so we scan the whole tour against this node's edges
                int u = node;
                int u_next = solution[(idx + 1) % n];
                // ... (Simplified: re-scanning full 2-opt is too slow, scanning just this node against all others is O(N))
                for(int j=0; j<n; ++j) {
                     if(std::abs(idx - j) <= 1 || (idx==0 && j==n-1) || (idx==n-1 && j==0)) continue;
                     int v = solution[j];
                     int v_next = solution[(j+1)%n];
                     int delta = (dist(u, v) + dist(u_next, v_next)) - (dist(u, u_next) + dist(v, v_next));
                     if(delta < 0) LM.push_back({1, delta, u, u_next, v, v_next});
                }
            }
            else {
                // Node is NOT in solution: It can only be a candidate for Swap (v)
                // Check this 'node' against all 'u' in solution
                for (int i = 0; i < n; ++i) {
                     int u = solution[i];
                     int u_prev = solution[(i - 1 + n) % n];
                     int u_next_node = solution[(i + 1) % n];
                     int delta = (dist(u_prev, node) + dist(node, u_next_node) + cost(node)) 
                               - (dist(u_prev, u) + dist(u, u_next_node) + cost(u));
                     if(delta < 0) LM.push_back({2, delta, u, -1, node, -1});
                }
            }
        }
    }
}

void M_Steepest_LM_RandomStart(
    int **distanceMatrix,
    std::vector<int> &costVector,
    int size,
    int totalRuns = 200)
{
    if (size <= 0) return;
    std::random_device rd;
    std::mt19937 g(rd());

    long long totalSum = 0;
    int bestObjective = std::numeric_limits<int>::max();
    int worstObjective = std::numeric_limits<int>::min();
    std::vector<int> bestSolution;
    auto startTime = std::chrono::high_resolution_clock::now();

    for (int run = 0; run < totalRuns; ++run)
    {
        std::vector<int> solution = randomPermutation(size, g);
        int n = static_cast<int>(solution.size());
        
        std::vector<int> pos(size, -1);
        for(int i=0; i<n; ++i) pos[solution[i]] = i;

        std::vector<Move> LM;
        LM.reserve(n * n); 
        generateMoves(distanceMatrix, costVector, solution, pos, LM, true, size);
        std::sort(LM.begin(), LM.end(), compareMoves);

        bool localOptimum = false;

        while (!localOptimum && !LM.empty())
        {
            bool moveApplied = false;
            
            auto it = LM.begin();
            while (it != LM.end())
            {
                Move m = *it;
                
                if (m.type == 1) { // Intra-Route (2-opt)
                    int e1 = checkEdge(m.u, m.u_next, solution, pos);
                    int e2 = checkEdge(m.v, m.v_next, solution, pos);

                    if (e1 == 0 || e2 == 0) { it = LM.erase(it); continue; } // Edge broken
                    if (e1 != e2) { ++it; continue; } // Direction mismatch (skip)

                    // Apply 2-opt
                    moveApplied = true;
                    int p1, p2;
                    if (e1 == 1) { p1 = pos[m.u_next]; p2 = pos[m.v]; }
                    else { p1 = pos[m.u]; p2 = pos[m.v_next]; } // Inverted case

                    reverseCircularSegment(solution, p1, p2);
                    
                    // Update Pos
                    for(int k=0; k<n; ++k) pos[solution[k]] = k;
                    
                    it = LM.erase(it);
                    
                    // Generate new moves
                    std::vector<int> changed = {m.u, m.u_next, m.v, m.v_next};
                    std::vector<Move> newMoves;
                    generateMoves(distanceMatrix, costVector, solution, pos, newMoves, false, size, changed);
                    std::sort(newMoves.begin(), newMoves.end(), compareMoves);
                    size_t oldSize = LM.size();
                    LM.insert(LM.end(), newMoves.begin(), newMoves.end());
                    std::inplace_merge(LM.begin(), LM.begin() + oldSize, LM.end(), compareMoves);
                    break; 
                }
                else if (m.type == 2) { // Inter-Route (Node Replacement)
                    // m.u is node to remove (must be IN solution)
                    // m.v is node to add (must be OUT of solution)
                    
                    int u_idx = pos[m.u];
                    int v_idx = pos[m.v];

                    // Validity Check
                    if (u_idx == -1) { it = LM.erase(it); continue; } // u no longer in solution
                    if (v_idx != -1) { it = LM.erase(it); continue; } // v already in solution

                    // Lazy Delta Check (neighbors might have changed)
                    int u_prev = solution[(u_idx - 1 + n) % n];
                    int u_next = solution[(u_idx + 1) % n];
                    
                    int current_delta = (dist(u_prev, m.v) + dist(m.v, u_next) + cost(m.v)) 
                                      - (dist(u_prev, m.u) + dist(m.u, u_next) + cost(m.u));
                    
                    if (current_delta >= 0) {
                        it = LM.erase(it); // No longer improving
                        continue;
                    }

                    // Apply Move
                    moveApplied = true;
                    solution[u_idx] = m.v; // Replace u with v
                    pos[m.u] = -1;         // u is now out
                    pos[m.v] = u_idx;      // v is now in

                    it = LM.erase(it);

                    // Generate new moves
                    // Nodes changed: The new node v, and its neighbors (previously u's neighbors)
                    // And the removed node u (now available for insertion elsewhere)
                    std::vector<int> changed = {m.v, u_prev, u_next, m.u};
                    std::vector<Move> newMoves;
                    generateMoves(distanceMatrix, costVector, solution, pos, newMoves, false, size, changed);
                    std::sort(newMoves.begin(), newMoves.end(), compareMoves);
                    size_t oldSize = LM.size();
                    LM.insert(LM.end(), newMoves.begin(), newMoves.end());
                    std::inplace_merge(LM.begin(), LM.begin() + oldSize, LM.end(), compareMoves);
                    break;
                }
            }
            if (!moveApplied) localOptimum = true;
        }
        bestSolution = solution;
        int finalCost = evaluateSolution(solution, distanceMatrix, costVector);
        totalSum += finalCost;
        if (finalCost < bestObjective) bestObjective = finalCost;
        if (finalCost > worstObjective) worstObjective = finalCost;
    }

    auto endTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = endTime - startTime;
    
    std::cout << "====== M_Steepest_LM (Lazy Eval + In/Out Swap) ======\n";
    std::cout << "  runs = " << totalRuns << "\n";
    std::cout << "  min = " << bestObjective << "\n";
    std::cout << "  max = " << worstObjective << "\n";
    std::cout << "  avg = " << (double)totalSum / totalRuns << "\n";
    std::cout << "Execution time: " << elapsed.count() << " s\n\n";
    for (auto node : bestSolution) {
        std::cout << node << " ";
    }
    std::cout << "\n";
}

int main()
{
    std::vector<std::string> fileNames = {"../TSPA.csv", "../TSPB.csv"};
    for (const auto &FILE_NAME : fileNames)
    {
        std::vector<std::vector<int>> data;
        if (!getDataFromFile(FILE_NAME, data)) continue;

        int size = data.size();
        int **distanceMatrix = getDistanceMatrix(data, size);
        std::vector<int> costVector = getCostVector(data);

        M_Steepest_LM_RandomStart(distanceMatrix, costVector, size);

        for (int i = 0; i < size; i++) delete[] distanceMatrix[i];
        delete[] distanceMatrix;
    }
    return 0;
}