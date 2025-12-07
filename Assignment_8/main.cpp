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

std::vector<int> destroySolution_CostBased(
    const std::vector<int> &solution,
    int **distanceMatrix,
    const std::vector<int> &costVector,
    std::mt19937 &g,
    double destructionRatio = 0.3)
{
    std::vector<int> destroyed = solution;
    int numToRemove = std::max(2, static_cast<int>(solution.size() * destructionRatio));
    
    for (int k = 0; k < numToRemove && !destroyed.empty(); ++k) {
        std::vector<double> weights;
        weights.reserve(destroyed.size());
        
        for (int node : destroyed) {
            weights.push_back(static_cast<double>(costVector[node]) + 0.1);
        }
        
        std::discrete_distribution<> d(weights.begin(), weights.end());
        int idxToRemove = d(g);
        destroyed.erase(destroyed.begin() + idxToRemove);
    }
    return destroyed;
}

std::vector<int> repairSolution_Regret(
    const std::vector<int> &partialSolution,
    int **distanceMatrix,
    const std::vector<int> &costVector,
    int totalNodes,
    int targetSize)
{
    std::vector<int> repaired = partialSolution;
    std::vector<bool> inSolution(totalNodes, false);
    for (int node : repaired) inSolution[node] = true;
    
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
                
                int increase = distanceMatrix[u][node] + distanceMatrix[node][v] 
                             - distanceMatrix[u][v] + costVector[node];
                
                if (increase < bestIncrease) {
                    secondBestIncrease = bestIncrease;
                    bestIncrease = increase;
                    currentBestPos = p;
                } else if (increase < secondBestIncrease) {
                    secondBestIncrease = increase;
                }
            }

            int regret = (secondBestIncrease == std::numeric_limits<int>::max()) 
                         ? -1 : (secondBestIncrease - bestIncrease);

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

// ==================== LNS ====================

// Modifed to return the best solution found
std::vector<int> runLNS(
    int **distanceMatrix,
    const std::vector<int> &costVector,
    int size,
    bool enableLS, 
    double timeLimit)
{
    std::random_device rd;
    std::mt19937 g(rd());

    std::vector<int> currentSolution = randomPermutation(size, g);
    applySteepestLS(currentSolution, distanceMatrix, costVector, size);
    
    int currentCost = evaluateSolution(currentSolution, distanceMatrix, costVector);
    int runBestCost = currentCost;
    std::vector<int> runBestSolution = currentSolution;
    int targetSize = currentSolution.size();

    auto runStartTime = std::chrono::high_resolution_clock::now();

    while (true) {
        auto currentTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = currentTime - runStartTime;
        if (elapsed.count() >= timeLimit) break;
        
        std::vector<int> destroyed = destroySolution_CostBased(currentSolution, distanceMatrix, costVector, g, 0.3);
        std::vector<int> repaired = repairSolution_Regret(destroyed, distanceMatrix, costVector, size, targetSize);
        
        if (enableLS) {
            applySteepestLS(repaired, distanceMatrix, costVector, size);
        }
        
        int repairedCost = evaluateSolution(repaired, distanceMatrix, costVector);
        
        if (repairedCost < currentCost) {
            currentSolution = repaired;
            currentCost = repairedCost;
            
            if (currentCost < runBestCost) {
                runBestCost = currentCost;
                runBestSolution = currentSolution;
            }
        }
    }
    return runBestSolution;
}

// ==================== OPTIMIZED SIMILARITY & ANALYSIS ====================

// [cite: 7] Jaccard Similarity for Nodes
double calculateNodeSimilarity(const std::vector<int> &sol1, const std::vector<int> &sol2)
{
    // O(N) using boolean array
    static std::vector<bool> inSol1;
    if (inSol1.size() < 200) inSol1.resize(200, false); // Adjust size if needed
    
    // Clear array
    std::fill(inSol1.begin(), inSol1.end(), false);

    for (int node : sol1) inSol1[node] = true;
    
    int commonNodes = 0;
    for (int node : sol2) {
        if (inSol1[node]) commonNodes++;
    }
    
    // Union = Size1 + Size2 - Intersection
    int unionSize = sol1.size() + sol2.size() - commonNodes;
    return (unionSize > 0) ? (double)commonNodes / unionSize : 0.0;
}

// [cite: 7, 9] Similarity for Edges
// Optimization: Return sorted edges vector to allow fast intersection
using Edge = std::pair<int, int>;
std::vector<Edge> getSortedEdges(const std::vector<int>& sol) {
    std::vector<Edge> edges;
    edges.reserve(sol.size());
    for(size_t i=0; i<sol.size(); ++i) {
        int u = sol[i];
        int v = sol[(i+1)%sol.size()];
        if(u > v) std::swap(u, v);
        edges.push_back({u, v});
    }
    std::sort(edges.begin(), edges.end());
    return edges;
}

double calculateEdgeSimilarity(const std::vector<Edge> &edges1, const std::vector<Edge> &edges2)
{
    int common = 0;
    size_t i = 0, j = 0;
    while(i < edges1.size() && j < edges2.size()) {
        if(edges1[i] < edges2[j]) i++;
        else if(edges2[j] < edges1[i]) j++;
        else {
            common++;
            i++;
            j++;
        }
    }
    
    int unionSize = edges1.size() + edges2.size() - common;
    return (unionSize > 0) ? (double)common / unionSize : 0.0;
}

//  Pearson Correlation Coefficient
double calculateCorrelation(const std::vector<double>& x, const std::vector<double>& y) {
    if(x.size() != y.size() || x.empty()) return 0.0;
    double n = x.size();
    double sumX = 0, sumY = 0, sumXY = 0;
    double sumX2 = 0, sumY2 = 0;

    for(size_t i=0; i<n; ++i) {
        sumX += x[i];
        sumY += y[i];
        sumXY += x[i] * y[i];
        sumX2 += x[i] * x[i];
        sumY2 += y[i] * y[i];
    }

    double numerator = n * sumXY - sumX * sumY;
    double denominator = std::sqrt((n * sumX2 - sumX * sumX) * (n * sumY2 - sumY * sumY));

    if(denominator == 0) return 0.0;
    return numerator / denominator;
}

void performGlobalConvexityTest(
    const std::string &instanceName,
    int **distanceMatrix,
    std::vector<int> &costVector,
    int size)
{
    std::cout << "Starting analysis for " << instanceName << "...\n";
    std::mt19937 g(std::random_device{}());

    // 1.  Generate a "Very Good Solution" using LNS (Best method so far)
    std::cout << "  Generating Reference Solution (LNS)...\n";
    double lnsTime = (instanceName.find("TSPA") != std::string::npos) ? 4.0 : 4.0; // Short run for reference
    std::vector<int> globalBestSol = runLNS(distanceMatrix, costVector, size, true, lnsTime);
    int globalBestCost = evaluateSolution(globalBestSol, distanceMatrix, costVector);
    std::vector<Edge> globalBestEdges = getSortedEdges(globalBestSol);
    std::cout << "  Global Reference Cost: " << globalBestCost << "\n";

    // 2.  Generate 1000 Random Local Optima
    std::cout << "  Generating 1000 Local Optima...\n";
    int numSamples = 1000;
    std::vector<std::vector<int>> localOptima;
    std::vector<int> costs;
    localOptima.reserve(numSamples);
    costs.reserve(numSamples);

    int bestLOCost = std::numeric_limits<int>::max();
    int bestLOIdx = -1;

    for(int i=0; i<numSamples; ++i) {
        std::vector<int> sol = randomPermutation(size, g);
        applySteepestLS(sol, distanceMatrix, costVector, size);
        int c = evaluateSolution(sol, distanceMatrix, costVector);
        localOptima.push_back(sol);
        costs.push_back(c);

        if(c < bestLOCost) {
            bestLOCost = c;
            bestLOIdx = i;
        }
    }
    std::vector<int> bestLOSol = localOptima[bestLOIdx];
    std::vector<Edge> bestLOEdges = getSortedEdges(bestLOSol);

    // Pre-calculate edges for all local optima to speed up pairwise comparison
    std::vector<std::vector<Edge>> allEdges(numSamples);
    for(int i=0; i<numSamples; ++i) {
        allEdges[i] = getSortedEdges(localOptima[i]);
    }

    // 3. [cite: 3, 4, 7] Calculate Similarities
    // Vectors to store results for CSV
    std::vector<double> x_obj; 
    std::vector<double> y_avg_nodes, y_bestLO_nodes, y_global_nodes;
    std::vector<double> y_avg_edges, y_bestLO_edges, y_global_edges;

    // Buffers for correlation calculation (filtering out self-comparison for BestLO)
    std::vector<double> corr_x_bestLO, corr_y_bestLO_nodes, corr_y_bestLO_edges;

    std::cout << "  Calculating Similarities...\n";
    for(int i=0; i<numSamples; ++i) {
        x_obj.push_back((double)costs[i]);

        // A. Similarity to Global Best (LNS)
        y_global_nodes.push_back(calculateNodeSimilarity(localOptima[i], globalBestSol));
        y_global_edges.push_back(calculateEdgeSimilarity(allEdges[i], globalBestEdges));

        // B. Similarity to Best of Local Optima
        // [cite: 6] For correlation/charts involving "Best of LO", strictly exclude the best solution itself?
        // Usually, we keep the array aligned for CSV, but filter for correlation calculation.
        double simBestNode = calculateNodeSimilarity(localOptima[i], bestLOSol);
        double simBestEdge = calculateEdgeSimilarity(allEdges[i], bestLOEdges);
        y_bestLO_nodes.push_back(simBestNode);
        y_bestLO_edges.push_back(simBestEdge);

        if (i != bestLOIdx) {
            corr_x_bestLO.push_back((double)costs[i]);
            corr_y_bestLO_nodes.push_back(simBestNode);
            corr_y_bestLO_edges.push_back(simBestEdge);
        }

        // C. Average Similarity to All Others
        double sumSimNode = 0;
        double sumSimEdge = 0;
        for(int j=0; j<numSamples; ++j) {
            if(i == j) continue;
            sumSimNode += calculateNodeSimilarity(localOptima[i], localOptima[j]);
            sumSimEdge += calculateEdgeSimilarity(allEdges[i], allEdges[j]);
        }
        y_avg_nodes.push_back(sumSimNode / (numSamples - 1));
        y_avg_edges.push_back(sumSimEdge / (numSamples - 1));
    }

    // 4.  Calculate Correlations
    std::cout << "\n  === Correlations (" << instanceName << ") ===\n";
    std::cout << std::fixed << std::setprecision(4);
    
    // Avg Similarity
    double r_avg_node = calculateCorrelation(x_obj, y_avg_nodes);
    double r_avg_edge = calculateCorrelation(x_obj, y_avg_edges);
    std::cout << "  1. Objective vs Avg Node Sim: " << r_avg_node << "\n";
    std::cout << "  2. Objective vs Avg Edge Sim: " << r_avg_edge << "\n";

    // Best of 1000 (excluding self)
    double r_bestLO_node = calculateCorrelation(corr_x_bestLO, corr_y_bestLO_nodes);
    double r_bestLO_edge = calculateCorrelation(corr_x_bestLO, corr_y_bestLO_edges);
    std::cout << "  3. Objective vs Best-of-1000 Node Sim: " << r_bestLO_node << "\n";
    std::cout << "  4. Objective vs Best-of-1000 Edge Sim: " << r_bestLO_edge << "\n";

    // Global Best (LNS)
    double r_global_node = calculateCorrelation(x_obj, y_global_nodes);
    double r_global_edge = calculateCorrelation(x_obj, y_global_edges);
    std::cout << "  5. Objective vs Global Best Node Sim: " << r_global_node << "\n";
    std::cout << "  6. Objective vs Global Best Edge Sim: " << r_global_edge << "\n";

    // 5.  Export to CSV
    std::string csvName = instanceName + "_correlations.csv";
    std::ofstream csv(csvName);
    csv << "Objective,AvgSim_Nodes,BestLO_Nodes,Global_Nodes,AvgSim_Edges,BestLO_Edges,Global_Edges\n";
    for(size_t k=0; k<x_obj.size(); ++k) {
        csv << x_obj[k] << ","
            << y_avg_nodes[k] << ","
            << y_bestLO_nodes[k] << ","
            << y_global_nodes[k] << ","
            << y_avg_edges[k] << ","
            << y_bestLO_edges[k] << ","
            << y_global_edges[k] << "\n";
    }
    csv.close();
    std::cout << "  Data saved to " << csvName << "\n\n";
}

int main()
{
    std::vector<std::string> fileNames = {"TSPA.csv", "TSPB.csv"}; 
    for (const auto &FILE_NAME : fileNames)
    {
        std::vector<std::vector<int>> data;
        if (!getDataFromFile(FILE_NAME, data)) {
            if (!getDataFromFile("../" + FILE_NAME, data)) {
                std::cerr << "Failed to open: " << FILE_NAME << "\n";
                continue;
            }
        }
        
        int size = data.size();
        int **distanceMatrix = getDistanceMatrix(data, size);
        std::vector<int> costVector = getCostVector(data);
        
        std::string instanceName = FILE_NAME.substr(0, FILE_NAME.find('.'));
        performGlobalConvexityTest(instanceName, distanceMatrix, costVector, size);

        for (int i = 0; i < size; i++)
            delete[] distanceMatrix[i];
        delete[] distanceMatrix;
    }
    return 0;
}