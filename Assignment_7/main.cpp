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
    int nodesToVisit = size / 2;
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

// ==================== LOCAL SEARCH LOGIC ====================

struct Move {
    int type; // 1 = Intra (2-opt), 2 = Inter (Swap In-Node with Out-Node)
    int delta;
    int u, u_next;
    int v, v_next;
};

bool compareMoves(const Move &a, const Move &b) {
    return a.delta < b.delta;
}

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
    const std::vector<int> &pos,
    std::vector<Move> &LM,
    bool fullScan,
    int totalNodes,
    const std::vector<int> &nodesToCheck = {})
{
    int n = solution.size();

    auto addIntraMoves = [&](int u_idx) {
        int u = solution[u_idx];
        int u_next_idx = (u_idx + 1) % n;
        int u_next = solution[u_next_idx];

        for (int v_idx = 0; v_idx < n; ++v_idx) {
             if (u_idx == v_idx || u_next_idx == v_idx) continue;
             
             int v = solution[v_idx];
             int v_next_idx = (v_idx + 1) % n;
             if (v_next_idx == u_idx) continue;

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
        
        for (int v = 0; v < totalNodes; ++v) {
            if (pos[v] != -1) continue;

            int current_cost = dist(u_prev, u) + dist(u, u_next) + cost(u);
            int new_cost = dist(u_prev, v) + dist(v, u_next) + cost(v);
            
            int delta = new_cost - current_cost;
            
            if (delta < 0) {
                LM.push_back({2, delta, u, -1, v, -1});
            }
        }
    };

    if (fullScan) {
        for (int i = 0; i < n; ++i) {
            for (int j = i + 2; j < n + (i > 0 ? 0 : -1); ++j) {
                 int u = solution[i];
                 int u_next = solution[(i+1)%n];
                 int v = solution[j];
                 int v_next = solution[(j+1)%n];
                 int delta = (dist(u, v) + dist(u_next, v_next)) - (dist(u, u_next) + dist(v, v_next));
                 if(delta < 0) LM.push_back({1, delta, u, u_next, v, v_next});
            }
        }
        for (int i = 0; i < n; ++i) {
            addInterMoves(i);
        }
    } 
    else {
        for (int node : nodesToCheck) {
            int idx = pos[node];
            if (idx != -1) {
                addInterMoves(idx); 
                int u = node;
                int u_next = solution[(idx + 1) % n];
                for(int j=0; j<n; ++j) {
                     if(std::abs(idx - j) <= 1 || (idx==0 && j==n-1) || (idx==n-1 && j==0)) continue;
                     int v = solution[j];
                     int v_next = solution[(j+1)%n];
                     int delta = (dist(u, v) + dist(u_next, v_next)) - (dist(u, u_next) + dist(v, v_next));
                     if(delta < 0) LM.push_back({1, delta, u, u_next, v, v_next});
                }
            }
            else {
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

void applySteepestLS(
    std::vector<int> &sol,
    int **distanceMatrix,
    const std::vector<int> &costVector,
    int totalNodes)
{
    int n = sol.size();
    std::vector<int> pos(totalNodes, -1);
    for(int i=0; i<n; ++i) pos[sol[i]] = i;

    std::vector<Move> LM;
    LM.reserve(n * n); 
    generateMoves(distanceMatrix, costVector, sol, pos, LM, true, totalNodes);
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
                generateMoves(distanceMatrix, costVector, sol, pos, newMoves, false, totalNodes, changed);
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
                generateMoves(distanceMatrix, costVector, sol, pos, newMoves, false, totalNodes, changed);
                std::sort(newMoves.begin(), newMoves.end(), compareMoves);
                size_t oldSize = LM.size();
                LM.insert(LM.end(), newMoves.begin(), newMoves.end());
                std::inplace_merge(LM.begin(), LM.begin() + oldSize, LM.end(), compareMoves);
                break;
            }
        }
        if (!moveApplied) localOptimum = true;
    }
}

// ==================== DESTROY & REPAIR OPERATORS ====================

std::vector<int> destroySolution(
    const std::vector<int> &solution,
    int **distanceMatrix,
    const std::vector<int> &costVector,
    std::mt19937 &g,
    double destructionRatio = 0.3)
{
    int n = solution.size();
    int numToRemove = std::max(2, static_cast<int>(n * destructionRatio));
    
    std::vector<int> destroyed = solution;
    std::uniform_int_distribution<int> indexDist(0, destroyed.size() - 1);
    
    // Remove random nodes
    for (int i = 0; i < numToRemove && !destroyed.empty(); ++i) {
        int idx = indexDist(g);
        destroyed.erase(destroyed.begin() + idx);
        if (!destroyed.empty() && indexDist.param().b() >= (int)destroyed.size()) {
            indexDist = std::uniform_int_distribution<int>(0, destroyed.size() - 1);
        }
    }
    
    return destroyed;
}

// TODO FOR JAKUB: Implement alternative destroy operator
// This should remove nodes based on their cost (expensive nodes more likely to be removed)
// Use probabilistic selection: probability proportional to node cost
std::vector<int> destroySolution_CostBased(
    const std::vector<int> &solution,
    int **distanceMatrix,
    const std::vector<int> &costVector,
    std::mt19937 &g,
    double destructionRatio = 0.3)
{
    // TODO: Implement cost-based destruction
    // Hints:
    // 1. Calculate numToRemove (same as random version)
    // 2. For each node in solution, calculate probability based on its cost
    // 3. Use std::discrete_distribution with probabilities
    // 4. Remove nodes according to probabilities
    // 5. Return destroyed solution
    
    std::vector<int> destroyed = solution;
    int numToRemove = std::max(2, static_cast<int>(solution.size() * destructionRatio));
    
    // YOUR CODE HERE
    
    return destroyed;
}

std::vector<int> repairSolution(
    const std::vector<int> &partialSolution,
    int **distanceMatrix,
    const std::vector<int> &costVector,
    int totalNodes,
    int targetSize)
{
    std::vector<int> repaired = partialSolution;
    std::vector<bool> inSolution(totalNodes, false);
    for (int node : repaired) inSolution[node] = true;
    
    // Greedy insertion with best position
    while ((int)repaired.size() < targetSize) {
        int bestNode = -1;
        int bestPos = -1;
        int bestCost = std::numeric_limits<int>::max();
        
        // Try all nodes not in solution
        for (int node = 0; node < totalNodes; ++node) {
            if (inSolution[node]) continue;
            
            // Try all positions
            for (int p = 0; p <= (int)repaired.size(); ++p) {
                repaired.insert(repaired.begin() + p, node);
                int costValue = evaluateSolution(repaired, distanceMatrix, costVector);
                if (costValue < bestCost) {
                    bestCost = costValue;
                    bestNode = node;
                    bestPos = p;
                }
                repaired.erase(repaired.begin() + p);
            }
        }
        
        if (bestNode == -1) break;
        
        repaired.insert(repaired.begin() + bestPos, bestNode);
        inSolution[bestNode] = true;
    }
    
    return repaired;
}

// TODO FOR JAKUB: Implement 2-regret repair operator
// Use 2-regret heuristic from Assignment 2
std::vector<int> repairSolution_Regret(
    const std::vector<int> &partialSolution,
    int **distanceMatrix,
    const std::vector<int> &costVector,
    int totalNodes,
    int targetSize)
{
    // TODO: Implement 2-regret repair
    // Hints:
    // 1. For each missing node, find best and second-best insertion positions
    // 2. Calculate regret = second_best_cost - best_cost
    // 3. Select node with highest regret
    // 4. Insert at its best position
    // 5. Repeat until solution is complete
    
    std::vector<int> repaired = partialSolution;
    std::vector<bool> inSolution(totalNodes, false);
    for (int node : repaired) inSolution[node] = true;
    
    // YOUR CODE HERE
    
    return repaired;
}

// ==================== LNS IMPLEMENTATIONS ====================

void LNS_WithLS(
    int **distanceMatrix,
    std::vector<int> &costVector,
    int size,
    int totalRuns = 20,
    double timeLimitPerRun = 13.35)
{
    std::random_device rd;
    std::mt19937 g(rd());

    long long totalSum = 0;
    int bestObjective = std::numeric_limits<int>::max();
    int worstObjective = std::numeric_limits<int>::min();
    std::vector<int> bestSolution;
    
    int totalIterations = 0;
    std::vector<int> iterationsPerRun;
    
    auto globalStartTime = std::chrono::high_resolution_clock::now();

    for (int run = 0; run < totalRuns; ++run)
    {
        int iterations = 0;
        auto runStartTime = std::chrono::high_resolution_clock::now();
        
        // Initial random solution + LS
        std::vector<int> currentSolution = randomPermutation(size, g);
        int targetSize = currentSolution.size();
        applySteepestLS(currentSolution, distanceMatrix, costVector, size);
        int currentCost = evaluateSolution(currentSolution, distanceMatrix, costVector);
        int runBestCost = currentCost;
        std::vector<int> runBestSolution = currentSolution;
        
        // LNS loop
        while (true) {
            auto currentTime = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> elapsed = currentTime - runStartTime;
            if (elapsed.count() >= timeLimitPerRun) break;
            
            iterations++;
            
            // Destroy
            std::vector<int> destroyed = destroySolution(currentSolution, distanceMatrix, costVector, g, 0.3);
            
            // Repair
            std::vector<int> repaired = repairSolution(destroyed, distanceMatrix, costVector, size, targetSize);
            
            // Local Search
            applySteepestLS(repaired, distanceMatrix, costVector, size);
            int repairedCost = evaluateSolution(repaired, distanceMatrix, costVector);
            
            // Acceptance criterion: accept if better
            if (repairedCost < currentCost) {
                currentSolution = repaired;
                currentCost = repairedCost;
                
                if (currentCost < runBestCost) {
                    runBestCost = currentCost;
                    runBestSolution = currentSolution;
                }
            }
        }
        
        iterationsPerRun.push_back(iterations);
        totalIterations += iterations;
        totalSum += runBestCost;
        if (runBestCost < bestObjective) {
            bestObjective = runBestCost;
            bestSolution = runBestSolution;
        }
        if (runBestCost > worstObjective) worstObjective = runBestCost;
    }

    auto endTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = endTime - globalStartTime;
    
    std::cout << "====== LNS with Local Search ======\n";
    std::cout << "  runs = " << totalRuns << "\n";
    std::cout << "  min = " << bestObjective << "\n";
    std::cout << "  max = " << worstObjective << "\n";
    std::cout << "  avg = " << (double)totalSum / totalRuns << "\n";
    std::cout << "Execution time: " << elapsed.count() << " s\n";
    std::cout << "Average time per run: " << elapsed.count() / totalRuns << " s\n";
    std::cout << "Total iterations: " << totalIterations << "\n";
    std::cout << "Average iterations per run: " << (double)totalIterations / totalRuns << "\n";
    std::cout << "\nIterations per run: ";
    for (int count : iterationsPerRun) {
        std::cout << count << " ";
    }
    std::cout << "\n\nBest solution: ";
    for (auto node : bestSolution) {
        std::cout << node << " ";
    }
    std::cout << "\n\n";
}


int main()
{
    std::vector<std::string> fileNames = {"../TSPA.csv", "../TSPB.csv"};
    for (const auto &FILE_NAME : fileNames)
    {
        std::vector<std::vector<int>> data;
        if (!getDataFromFile(FILE_NAME, data)) {
            std::cerr << "Failed to open: " << FILE_NAME << "\n";
            continue;
        }
        
        int size = data.size();
        int **distanceMatrix = getDistanceMatrix(data, size);
        std::vector<int> costVector = getCostVector(data);
        
        std::cout << "\n========================================\n";
        std::cout << "Processing: " << FILE_NAME << "\n";
        std::cout << "========================================\n\n";
        
        double timeLimit = (FILE_NAME.find("TSPA") != std::string::npos) ? 13.69 : 14.37;
        
        LNS_WithLS(distanceMatrix, costVector, size, 20, timeLimit);

        for (int i = 0; i < size; i++)
            delete[] distanceMatrix[i];
        delete[] distanceMatrix;
    }
    return 0;
}