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
                 LM.push_back({1, delta, v, v_next, u, u_next}); // Add reverse direction
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
                LM.push_back({2, delta, v, -1, u, -1}); // Reverse move (for completeness)
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

// Perturbation function: Destroys and rebuilds part of the solution
std::vector<int> perturbSolution(
    const std::vector<int> &solution,
    int **distanceMatrix,
    const std::vector<int> &costVector,
    int totalNodes,
    std::mt19937 &g,
    double destructionRatio = 0.3)
{
    int n = solution.size();
    int numToRemove = std::max(2, static_cast<int>(n * destructionRatio));
    
    std::vector<int> perturbed = solution;
    std::vector<int> pos(totalNodes, -1);
    for(int i=0; i<n; ++i) pos[perturbed[i]] = i;
    
    // Remove random nodes
    std::vector<int> removed;
    std::uniform_int_distribution<int> indexDist(0, perturbed.size() - 1);
    
    for (int i = 0; i < numToRemove; ++i) {
        int idx = indexDist(g);
        removed.push_back(perturbed[idx]);
        pos[perturbed[idx]] = -1;
        perturbed.erase(perturbed.begin() + idx);
        if (!perturbed.empty() && indexDist.param().b() >= (int)perturbed.size()) {
            indexDist = std::uniform_int_distribution<int>(0, perturbed.size() - 1);
        }
    }
    
    // Update positions
    for(int i=0; i<(int)perturbed.size(); ++i) pos[perturbed[i]] = i;
    
    // Greedy reinsert: find best position for each removed node
    for (int node : removed) {
        int bestPos = 0;
        int bestCost = std::numeric_limits<int>::max();
        
        for (int p = 0; p <= (int)perturbed.size(); ++p) {
            perturbed.insert(perturbed.begin() + p, node);
            int costValue = evaluateSolution(perturbed, distanceMatrix, costVector);
            if (costValue < bestCost) {
                bestCost = costValue;
                bestPos = p;
            }
            perturbed.erase(perturbed.begin() + p);
        }
        perturbed.insert(perturbed.begin() + bestPos, node);
    }
    
    return perturbed;
}

void ILS_Steepest_LM(
    int **distanceMatrix,
    std::vector<int> &costVector,
    int size,
    int totalRuns = 20,
    double timeLimitPerRun = 0.0)
{
    if (size <= 0) return;
    std::random_device rd;
    std::mt19937 g(rd());

    long long totalSum = 0;
    int bestObjective = std::numeric_limits<int>::max();
    int worstObjective = std::numeric_limits<int>::min();
    std::vector<int> bestSolution;
    
    int totalLSRuns = 0;
    std::vector<int> lsRunsPerExperiment;
    
    auto globalStartTime = std::chrono::high_resolution_clock::now();

    // Lambda to apply local search
    auto applyLS = [&](std::vector<int> &sol, int &lsCounter) {
        lsCounter++;
        int n = sol.size();
        std::vector<int> pos(size, -1);
        for(int i=0; i<n; ++i) pos[sol[i]] = i;

        std::vector<Move> LM;
        LM.reserve(n * n); 
        generateMoves(distanceMatrix, costVector, sol, pos, LM, true, size);
        std::sort(LM.begin(), LM.end(), compareMoves);

        bool localOptimum = false;

        while (!localOptimum && !LM.empty())
        {
            bool moveApplied = false;
            auto it = LM.begin();
            while (it != LM.end())
            {
                Move m = *it;
                
                if (m.type == 1) {
                    int e1 = checkEdge(m.u, m.u_next, sol, pos);
                    int e2 = checkEdge(m.v, m.v_next, sol, pos);

                    if (e1 == 0 || e2 == 0) { it = LM.erase(it); continue; }
                    if (e1 != e2) { ++it; continue; }

                    moveApplied = true;
                    int p1, p2;
                    if (e1 == 1) { p1 = pos[m.u_next]; p2 = pos[m.v]; }
                    else { p1 = pos[m.u]; p2 = pos[m.v_next]; }

                    reverseCircularSegment(sol, p1, p2);
                    for(int k=0; k<n; ++k) pos[sol[k]] = k;
                    it = LM.erase(it);
                    
                    std::vector<int> changed = {m.u, m.u_next, m.v, m.v_next};
                    std::vector<Move> newMoves;
                    generateMoves(distanceMatrix, costVector, sol, pos, newMoves, false, size, changed);
                    std::sort(newMoves.begin(), newMoves.end(), compareMoves);
                    size_t oldSize = LM.size();
                    LM.insert(LM.end(), newMoves.begin(), newMoves.end());
                    std::inplace_merge(LM.begin(), LM.begin() + oldSize, LM.end(), compareMoves);
                    break; 
                }
                else if (m.type == 2) {
                    int u_idx = pos[m.u];
                    int v_idx = pos[m.v];

                    if (u_idx == -1) { it = LM.erase(it); continue; }
                    if (v_idx != -1) { it = LM.erase(it); continue; }

                    int u_prev = sol[(u_idx - 1 + n) % n];
                    int u_next = sol[(u_idx + 1) % n];
                    
                    int current_delta = (dist(u_prev, m.v) + dist(m.v, u_next) + cost(m.v)) 
                                      - (dist(u_prev, m.u) + dist(m.u, u_next) + cost(m.u));
                    
                    if (current_delta >= 0) {
                        it = LM.erase(it);
                        continue;
                    }

                    moveApplied = true;
                    sol[u_idx] = m.v;
                    pos[m.u] = -1;
                    pos[m.v] = u_idx;

                    it = LM.erase(it);

                    std::vector<int> changed = {m.v, u_prev, u_next, m.u};
                    std::vector<Move> newMoves;
                    generateMoves(distanceMatrix, costVector, sol, pos, newMoves, false, size, changed);
                    std::sort(newMoves.begin(), newMoves.end(), compareMoves);
                    size_t oldSize = LM.size();
                    LM.insert(LM.end(), newMoves.begin(), newMoves.end());
                    std::inplace_merge(LM.begin(), LM.begin() + oldSize, LM.end(), compareMoves);
                    break;
                }
            }
            if (!moveApplied) localOptimum = true;
        }
    };

    for (int run = 0; run < totalRuns; ++run)
    {
        int runLSCount = 0;
        auto runStartTime = std::chrono::high_resolution_clock::now();
        
        // Initial random solution + LS
        std::vector<int> currentSolution = randomPermutation(size, g);
        applyLS(currentSolution, runLSCount);
        int currentCost = evaluateSolution(currentSolution, distanceMatrix, costVector);
        int runBestCost = currentCost;
        std::vector<int> runBestSolution = currentSolution;
        
        // ILS loop
        while (true) {
            auto currentTime = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> elapsed = currentTime - runStartTime;
            if (timeLimitPerRun > 0 && elapsed.count() >= timeLimitPerRun) break;
            
            // Perturb
            std::vector<int> perturbedSolution = perturbSolution(
                currentSolution, distanceMatrix, costVector, size, g, 0.3);
            
            // Apply LS
            applyLS(perturbedSolution, runLSCount);
            int perturbedCost = evaluateSolution(perturbedSolution, distanceMatrix, costVector);
            
            // Acceptance criterion: accept if better or equal (to allow exploration)
            if (perturbedCost <= currentCost) {
                currentSolution = perturbedSolution;
                currentCost = perturbedCost;
                
                if (currentCost < runBestCost) {
                    runBestCost = currentCost;
                    runBestSolution = currentSolution;
                }
            }
        }
        
        lsRunsPerExperiment.push_back(runLSCount);
        totalLSRuns += runLSCount;
        totalSum += runBestCost;
        if (runBestCost < bestObjective) {
            bestObjective = runBestCost;
            bestSolution = runBestSolution;
        }
        if (runBestCost > worstObjective) worstObjective = runBestCost;
    }

    auto endTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = endTime - globalStartTime;
    
    std::cout << "====== ILS (Iterated Local Search) ======\n";
    std::cout << "  runs = " << totalRuns << "\n";
    std::cout << "  min = " << bestObjective << "\n";
    std::cout << "  max = " << worstObjective << "\n";
    std::cout << "  avg = " << (double)totalSum / totalRuns << "\n";
    std::cout << "Execution time: " << elapsed.count() << " s\n";
    std::cout << "Average time per run: " << elapsed.count() / totalRuns << " s\n";
    std::cout << "Total LS runs: " << totalLSRuns << "\n";
    std::cout << "Average LS runs per experiment: " << (double)totalLSRuns / totalRuns << "\n";
    std::cout << "\nLS runs per experiment: ";
    for (int count : lsRunsPerExperiment) {
        std::cout << count << " ";
    }
    std::cout << "\n\nBest solution: ";
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

        std::cout << "\n========================================\n";
        std::cout << "Processing: " << FILE_NAME << "\n";
        std::cout << "========================================\n\n";

        // Run MSLS first to get average time
        auto mslsStart = std::chrono::high_resolution_clock::now();
        M_Steepest_LM_RandomStart(distanceMatrix, costVector, size, 20);
        auto mslsEnd = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> mslsTime = mslsEnd - mslsStart;
        double avgTimePerRun = mslsTime.count() / 20.0;

        std::cout << "\n";
        
        // Run ILS with time limit based on MSLS
        ILS_Steepest_LM(distanceMatrix, costVector, size, 20, avgTimePerRun);

        for (int i = 0; i < size; i++) delete[] distanceMatrix[i];
        delete[] distanceMatrix;
    }
    return 0;
}