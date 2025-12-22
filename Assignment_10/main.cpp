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
#include <set>
#include <iomanip>

// ==================== DATA & HELPER FUNCTIONS ====================

bool getDataFromFile(const std::string &filename, std::vector<std::vector<int>> &data) {
    std::ifstream file(filename);
    if (!file.is_open()) return false;
    std::string line;
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string value;
        std::vector<int> row;
        while (std::getline(ss, value, ';')) {
            try { row.push_back(std::stoi(value)); }
            catch (...) { return false; }
        }
        data.push_back(row);
    }
    file.close();
    return true;
}

int getEuclidanDistance(int x1, int y1, int x2, int y2) {
    return static_cast<int>(std::sqrt(std::pow(x2 - x1, 2) + std::pow(y2 - y1, 2)));
}

int **getDistanceMatrix(std::vector<std::vector<int>> &data, int &size) {
    int **distanceMatrix = new int *[size];
    for (int i = 0; i < size; i++) {
        distanceMatrix[i] = new int[size];
        for (int j = 0; j < size; j++) {
            if (i == j) distanceMatrix[i][j] = 0;
            else distanceMatrix[i][j] = getEuclidanDistance(data[i][0], data[i][1], data[j][0], data[j][1]);
        }
    }
    return distanceMatrix;
}

std::vector<int> getCostVector(std::vector<std::vector<int>> &data) {
    std::vector<int> nodeCosts;
    for (const auto &row : data) nodeCosts.push_back(row[2]);
    data.clear();
    return nodeCosts;
}

int evaluateSolution(const std::vector<int> &solution, int **distanceMatrix, const std::vector<int> &costVector) {
    int totalCost = 0;
    if (solution.empty()) return 0;
    for (size_t i = 0; i < solution.size(); ++i) {
        totalCost += costVector[solution[i]];
        totalCost += distanceMatrix[solution[i]][solution[(i + 1) % solution.size()]];
    }
    return totalCost;
}

std::vector<int> randomPermutation(int size, std::mt19937 &g) {
    int nodesToVisit = size / 2;
    if (size % 2 != 0) nodesToVisit++;
    std::vector<int> solution(size);
    std::iota(solution.begin(), solution.end(), 0);
    std::shuffle(solution.begin(), solution.end(), g);
    solution.resize(nodesToVisit);
    return solution;
}

void reverseCircularSegment(std::vector<int> &solution, int pos1, int pos2) {
    int n = static_cast<int>(solution.size());
    if (n == 0) return;
    pos1 = (pos1 % n + n) % n;
    pos2 = (pos2 % n + n) % n;
    int halfLen;
    if (pos1 <= pos2) halfLen = (pos2 - pos1 + 1) / 2;
    else halfLen = (n - pos1 + pos2 + 1) / 2;
    for (int i = 0; i < halfLen; ++i) {
        std::swap(solution[pos1], solution[pos2]);
        pos1 = (pos1 + 1) % n;
        pos2 = (pos2 - 1 + n) % n;
    }
}

// ==================== LOCAL SEARCH (STEEPEST) ====================

struct Move {
    int type; // 1 = Intra (2-opt), 2 = Inter (Node Swap)
    int delta;
    int u, u_next;
    int v, v_next;
};

#define dist(a, b) distanceMatrix[a][b]
#define cost(a) costVector[a]

void applySteepestLS(std::vector<int> &sol, int **distanceMatrix, const std::vector<int> &costVector, int totalNodes) {
    bool improvement = true;
    while (improvement) {
        improvement = false;
        int n = sol.size();
        std::vector<int> pos(totalNodes, -1);
        for(int i=0; i<n; ++i) pos[sol[i]] = i;
        
        Move bestMove = {0, 0, -1, -1, -1, -1};
        
        // 2-opt
        for (int i = 0; i < n; ++i) {
            for (int j = i + 2; j < n + (i > 0 ? 0 : -1); ++j) {
                int u = sol[i], u_next = sol[(i+1)%n];
                int v = sol[j], v_next = sol[(j+1)%n];
                int delta = (dist(u, v) + dist(u_next, v_next)) - (dist(u, u_next) + dist(v, v_next));
                if (delta < bestMove.delta) bestMove = {1, delta, i, (i+1)%n, j, (j+1)%n};
            }
        }
        
        // Node Swap (Inter)
        for (int i = 0; i < n; ++i) {
            int u = sol[i];
            int u_prev = sol[(i - 1 + n) % n];
            int u_next = sol[(i + 1) % n];
            for (int v = 0; v < totalNodes; ++v) {
                if (pos[v] != -1) continue;
                int delta = (dist(u_prev, v) + dist(v, u_next) + cost(v)) - (dist(u_prev, u) + dist(u, u_next) + cost(u));
                if (delta < bestMove.delta) bestMove = {2, delta, i, -1, v, -1};
            }
        }

        if (bestMove.delta < 0) {
            if (bestMove.type == 1) {
                reverseCircularSegment(sol, bestMove.u_next, bestMove.v);
            } else {
                sol[bestMove.u] = bestMove.v;
            }
            improvement = true;
        }
    }
}

// ==================== REPAIR OPERATOR (FROM LNS) ====================

std::vector<int> repairSolution_Regret(
    const std::vector<int> &partialSolution,
    int **distanceMatrix, const std::vector<int> &costVector,
    int totalNodes, int targetSize)
{
    std::vector<int> repaired = partialSolution;
    std::vector<bool> inSolution(totalNodes, false);
    for (int node : repaired) inSolution[node] = true;
    
    // Safety check: if partial is empty, add random start node
    if (repaired.empty()) {
        for(int i=0; i<totalNodes; ++i) {
            repaired.push_back(i);
            inSolution[i] = true;
            break; 
        }
    }

    while ((int)repaired.size() < targetSize) {
        int bestNode = -1;
        int maxRegret = -1;
        int bestPosForBestNode = -1;

        for (int node = 0; node < totalNodes; ++node) {
            if (inSolution[node]) continue;

            int bestIncrease = std::numeric_limits<int>::max();
            int secondBestIncrease = std::numeric_limits<int>::max();
            int currentBestPos = -1;

            int n = repaired.size();
            for (int p = 0; p < n; ++p) {
                int u = repaired[p];
                int v = repaired[(p + 1) % n];
                int increase = distanceMatrix[u][node] + distanceMatrix[node][v] - distanceMatrix[u][v] + costVector[node];
                
                if (increase < bestIncrease) {
                    secondBestIncrease = bestIncrease;
                    bestIncrease = increase;
                    currentBestPos = p;
                } else if (increase < secondBestIncrease) {
                    secondBestIncrease = increase;
                }
            }

            int regret = (secondBestIncrease == std::numeric_limits<int>::max()) ? bestIncrease : (secondBestIncrease - bestIncrease);
            if (regret > maxRegret) {
                maxRegret = regret;
                bestNode = node;
                bestPosForBestNode = currentBestPos;
            }
        }
        if (bestNode == -1) break; 
        repaired.insert(repaired.begin() + bestPosForBestNode + 1, bestNode);
        inSolution[bestNode] = true;
    }
    return repaired;
}

// ==================== OWN METHOD: SMART RECOMBINATION ====================

// Combines HEA "Common Components" logic with LNS "Regret Repair" logic
std::vector<int> recombination_SmartCommon(
    const std::vector<int> &p1,
    const std::vector<int> &p2,
    int **distanceMatrix,
    const std::vector<int> &costVector,
    int totalNodes,
    int targetSize,
    std::mt19937 &g)
{
    // 1. Identify Common Nodes
    std::vector<bool> inP1(totalNodes, false);
    for (int n : p1) inP1[n] = true;
    
    std::vector<int> commonNodes;
    std::vector<bool> isCommon(totalNodes, false);
    for (int n : p2) {
        if (inP1[n]) {
            commonNodes.push_back(n);
            isCommon[n] = true;
        }
    }

    // 2. Identify Common Edges
    std::set<std::pair<int,int>> edgesP1;
    for (size_t i = 0; i < p1.size(); ++i) {
        int u = p1[i];
        int v = p1[(i+1)%p1.size()];
        edgesP1.insert({std::min(u,v), std::max(u,v)});
    }

    std::vector<std::vector<int>> commonAdj(totalNodes);
    for (size_t i = 0; i < p2.size(); ++i) {
        int u = p2[i];
        int v = p2[(i+1)%p2.size()];
        std::pair<int,int> e = {std::min(u,v), std::max(u,v)};
        if (edgesP1.count(e)) {
            commonAdj[u].push_back(v);
            commonAdj[v].push_back(u);
        }
    }

    // 3. Build Subpaths from Common Edges
    std::vector<bool> visited(totalNodes, false);
    std::vector<std::vector<int>> fragments;
    
    // Logic to extract paths/cycles from commonAdj
    for (int node : commonNodes) {
        if (!visited[node] && commonAdj[node].size() < 2) { 
            std::vector<int> path;
            int curr = node;
            while (true) {
                visited[curr] = true;
                path.push_back(curr);
                int next = -1;
                for (int neighbor : commonAdj[curr]) {
                    if (!visited[neighbor]) {
                        next = neighbor;
                        break;
                    }
                }
                if (next == -1) break;
                curr = next;
            }
            fragments.push_back(path);
        }
    }
    // Handle closed cycles or isolated nodes missed above
    for (int node : commonNodes) {
        if (!visited[node]) {
            std::vector<int> path;
            int curr = node;
            while (true) {
                visited[curr] = true;
                path.push_back(curr);
                int next = -1;
                for (int neighbor : commonAdj[curr]) {
                    if (!visited[neighbor]) {
                        next = neighbor;
                        break;
                    }
                }
                if (next == -1) break;
                curr = next;
            }
            fragments.push_back(path);
        }
    }

    // 4. Connect Fragments Randomly to form a single partial cycle
    // Note: Previous assignment filled with random nodes first. 
    // HERE IS THE IMPROVEMENT: We connect ONLY the common parts, then let Regret fill the rest.
    std::shuffle(fragments.begin(), fragments.end(), g);
    std::vector<int> partialOffspring;
    for (const auto& frag : fragments) {
        if (g() % 2 == 0) partialOffspring.insert(partialOffspring.end(), frag.begin(), frag.end());
        else partialOffspring.insert(partialOffspring.end(), frag.rbegin(), frag.rend());
    }
    
    // 5. Smart Fill using Regret Heuristic
    return repairSolution_Regret(partialOffspring, distanceMatrix, costVector, totalNodes, targetSize);
}

// ==================== ALGORITHM LOOP ====================

struct Individual {
    std::vector<int> solution;
    int cost;
    bool operator<(const Individual &other) const { return cost < other.cost; }
};

bool isUnique(const std::vector<Individual> &pop, const Individual &ind) {
    for (const auto &p : pop) {
        if (p.cost == ind.cost) return false; 
    }
    return true;
}

std::vector<int> runGuidedMemetic(
    int **distanceMatrix,
    std::vector<int> &costVector,
    int size,
    int targetSize,
    double timeLimit
) {
    std::mt19937 g(std::random_device{}());
    std::vector<Individual> population;
    int eliteSize = 20;

    auto startTime = std::chrono::high_resolution_clock::now();

    // 1. Initialization
    int attempts = 0;
    while (population.size() < eliteSize && attempts < 2000) {
        std::vector<int> sol = randomPermutation(size, g);
        applySteepestLS(sol, distanceMatrix, costVector, size);
        int c = evaluateSolution(sol, distanceMatrix, costVector);
        if (isUnique(population, {sol, c})) population.push_back({sol, c});
        attempts++;
    }
    std::sort(population.begin(), population.end());

    std::vector<int> globalBestSol = population[0].solution;
    int globalBestCost = population[0].cost;
    int iterationsWithoutImprovement = 0;

    // 2. Steady State Loop
    while (true) {
        auto currTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = currTime - startTime;
        if (elapsed.count() >= timeLimit) break;

        // Selection
        if (population.size() < 2) break;
        std::uniform_int_distribution<int> indexDist(0, population.size() - 1);
        int idx1 = indexDist(g);
        int idx2 = indexDist(g);
        while (idx1 == idx2) idx2 = indexDist(g);

        // Crossover: Smart Common
        std::vector<int> childSol = recombination_SmartCommon(
            population[idx1].solution, population[idx2].solution,
            distanceMatrix, costVector, size, targetSize, g
        );

        // Local Search
        applySteepestLS(childSol, distanceMatrix, costVector, size);
        
        int childCost = evaluateSolution(childSol, distanceMatrix, costVector);
        Individual child = {childSol, childCost};

        // Replacement
        if (childCost < population.back().cost && isUnique(population, child)) {
            population.back() = child;
            std::sort(population.begin(), population.end());
            
            if (population[0].cost < globalBestCost) {
                globalBestCost = population[0].cost;
                globalBestSol = population[0].solution;
                iterationsWithoutImprovement = 0;
            } else {
                iterationsWithoutImprovement++;
            }
        } else {
            iterationsWithoutImprovement++;
        }

        // Diversity Preservation: Restart if stagnant
        // If no improvement for 1500 iterations, kill worst 50% and replace with random+LS
        if (iterationsWithoutImprovement > 1500) {
            int preserve = eliteSize / 2;
            population.resize(preserve);
            while (population.size() < eliteSize) {
                std::vector<int> sol = randomPermutation(size, g);
                applySteepestLS(sol, distanceMatrix, costVector, size);
                int c = evaluateSolution(sol, distanceMatrix, costVector);
                if (isUnique(population, {sol, c})) population.push_back({sol, c});
            }
            std::sort(population.begin(), population.end());
            iterationsWithoutImprovement = 0;
        }
    }

    return globalBestSol;
}

void runExperiment(const std::string& instanceName, int **dist, std::vector<int>& costs, int size, int targetSize) {
    int runs = 20;
    double timeLimit = (instanceName == "TSPA") ? 13.69 : 13.35; // Standard limits

    long long sumCost = 0;
    int bestCost = INT_MAX;
    int worstCost = INT_MIN;
    
    std::cout << "Running Own Method (GMA-SQR) on " << instanceName << "...\n";
    
    for (int r = 0; r < runs; ++r) {
        std::vector<int> sol = runGuidedMemetic(dist, costs, size, targetSize, timeLimit);
        int c = evaluateSolution(sol, dist, costs);
        
        sumCost += c;
        if (c < bestCost) bestCost = c;
        if (c > worstCost) worstCost = c;
    }

    std::cout << "Instance: " << instanceName << "\n";
    std::cout << "Min: " << bestCost << "\n";
    std::cout << "Avg: " << (double)sumCost / runs << "\n";
    std::cout << "Max: " << worstCost << "\n\n";
}

int main() {
    std::vector<std::string> files = {"TSPA.csv", "TSPB.csv"};
    for (const auto &fname : files) {
        std::vector<std::vector<int>> data;
        if (!getDataFromFile(fname, data)) {
            if (!getDataFromFile("../" + fname, data)) continue;
        }

        int size = data.size();
        int **distMat = getDistanceMatrix(data, size);
        std::vector<int> costs = getCostVector(data);
        int targetSize = size / 2 + (size % 2);

        runExperiment(fname.substr(0, 4), distMat, costs, size, targetSize);

        for(int i=0; i<size; ++i) delete[] distMat[i];
        delete[] distMat;
    }
    return 0;
}