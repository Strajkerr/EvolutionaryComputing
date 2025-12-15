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
        return false;
    std::string line;
    while (std::getline(file, line))
    {
        std::stringstream ss(line);
        std::string value;
        std::vector<int> row;
        while (std::getline(ss, value, ';'))
        {
            try
            {
                row.push_back(std::stoi(value));
            }
            catch (...)
            {
                return false;
            }
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
            if (i == j)
                distanceMatrix[i][j] = 0;
            else
                distanceMatrix[i][j] = getEuclidanDistance(data[i][0], data[i][1], data[j][0], data[j][1]);
        }
    }
    return distanceMatrix;
}

std::vector<int> getCostVector(std::vector<std::vector<int>> &data)
{
    std::vector<int> nodeCosts;
    for (const auto &row : data)
        nodeCosts.push_back(row[2]);
    data.clear();
    return nodeCosts;
}

int evaluateSolution(const std::vector<int> &solution, int **distanceMatrix, const std::vector<int> &costVector)
{
    int totalCost = 0;
    if (solution.empty())
        return 0;
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
    if (size % 2 != 0)
        nodesToVisit++; // Round up if odd
    std::vector<int> solution(size);
    std::iota(solution.begin(), solution.end(), 0);
    std::shuffle(solution.begin(), solution.end(), g);
    solution.resize(nodesToVisit);
    return solution;
}

void reverseCircularSegment(std::vector<int> &solution, int pos1, int pos2)
{
    int n = static_cast<int>(solution.size());
    if (n == 0)
        return;
    pos1 = (pos1 % n + n) % n;
    pos2 = (pos2 % n + n) % n;
    int halfLen;
    if (pos1 <= pos2)
        halfLen = (pos2 - pos1 + 1) / 2;
    else
        halfLen = (n - pos1 + pos2 + 1) / 2;
    for (int i = 0; i < halfLen; ++i)
    {
        std::swap(solution[pos1], solution[pos2]);
        pos1 = (pos1 + 1) % n;
        pos2 = (pos2 - 1 + n) % n;
    }
}

// ==================== LOCAL SEARCH (STEEPEST) ====================

struct Move
{
    int type; // 1 = Intra (2-opt), 2 = Inter (Node Swap)
    int delta;
    int u, u_next;
    int v, v_next;
};

bool compareMoves(const Move &a, const Move &b) { return a.delta < b.delta; }

int checkEdge(int u, int v, const std::vector<int> &sol, const std::vector<int> &pos)
{
    int n = sol.size();
    int u_idx = pos[u], v_idx = pos[v];
    if (u_idx == -1 || v_idx == -1)
        return 0;
    int u_next_idx = (u_idx + 1) % n;
    int u_prev_idx = (u_idx - 1 + n) % n;
    if (u_next_idx == v_idx)
        return 1;
    if (u_prev_idx == v_idx)
        return -1;
    return 0;
}

#define DIST_MAT(a, b) distanceMatrix[a][b]
#define NODE_COST(a) costVector[a]

void generateMoves(
    int **distanceMatrix, const std::vector<int> &costVector,
    const std::vector<int> &solution, const std::vector<int> &pos,
    std::vector<Move> &LM, bool fullScan, int totalNodes,
    const std::vector<int> &nodesToCheck = {})
{
    int n = solution.size();
    auto addIntra = [&](int u_idx)
    {
        int u = solution[u_idx];
        int u_next = solution[(u_idx + 1) % n];
        for (int v_idx = 0; v_idx < n; ++v_idx)
        {
            if (u_idx == v_idx || (u_idx + 1) % n == v_idx)
                continue;
            int v = solution[v_idx];
            int v_next = solution[(v_idx + 1) % n];
            if ((v_idx + 1) % n == u_idx)
                continue;
            int delta = (DIST_MAT(u, v) + DIST_MAT(u_next, v_next)) - (DIST_MAT(u, u_next) + DIST_MAT(v, v_next));
            if (delta < 0)
                LM.push_back({1, delta, u, u_next, v, v_next});
        }
    };
    auto addInter = [&](int u_idx)
    {
        int u = solution[u_idx];
        int u_prev = solution[(u_idx - 1 + n) % n];
        int u_next = solution[(u_idx + 1) % n];
        for (int v = 0; v < totalNodes; ++v)
        {
            if (pos[v] != -1)
                continue;
            int delta = (DIST_MAT(u_prev, v) + DIST_MAT(v, u_next) + NODE_COST(v)) - (DIST_MAT(u_prev, u) + DIST_MAT(u, u_next) + NODE_COST(u));
            if (delta < 0)
                LM.push_back({2, delta, u, -1, v, -1});
        }
    };

    if (fullScan)
    {
        for (int i = 0; i < n; ++i)
        {
            for (int j = i + 2; j < n + (i > 0 ? 0 : -1); ++j)
            {
                int u = solution[i], u_next = solution[(i + 1) % n];
                int v = solution[j], v_next = solution[(j + 1) % n];
                int delta = (DIST_MAT(u, v) + DIST_MAT(u_next, v_next)) - (DIST_MAT(u, u_next) + DIST_MAT(v, v_next));
                if (delta < 0)
                    LM.push_back({1, delta, u, u_next, v, v_next});
            }
            addInter(i);
        }
    }
    else
    {
        // Incremental update logic (omitted for brevity in this snippet, using full scan for safety in HEA)
        // In highly optimized versions, reuse the delta update logic from prev assignments
        // For this assignment, we call fullScan=true inside LS loop or implement simplified neighborhood
    }
}

void applySteepestLS(std::vector<int> &sol, int **distanceMatrix, const std::vector<int> &costVector, int totalNodes)
{
    bool improvement = true;
    while (improvement)
    {
        improvement = false;
        int n = sol.size();
        std::vector<int> pos(totalNodes, -1);
        for (int i = 0; i < n; ++i)
            pos[sol[i]] = i;

        Move bestMove = {0, 0, -1, -1, -1, -1};

        // 2-opt
        for (int i = 0; i < n; ++i)
        {
            for (int j = i + 2; j < n + (i > 0 ? 0 : -1); ++j)
            {
                int u = sol[i], u_next = sol[(i + 1) % n];
                int v = sol[j], v_next = sol[(j + 1) % n];
                int delta = (DIST_MAT(u, v) + DIST_MAT(u_next, v_next)) - (DIST_MAT(u, u_next) + DIST_MAT(v, v_next));
                if (delta < bestMove.delta)
                    bestMove = {1, delta, i, (i + 1) % n, j, (j + 1) % n};
            }
        }

        // Node Swap (Inter)
        for (int i = 0; i < n; ++i)
        {
            int u = sol[i];
            int u_prev = sol[(i - 1 + n) % n];
            int u_next = sol[(i + 1) % n];
            for (int v = 0; v < totalNodes; ++v)
            {
                if (pos[v] != -1)
                    continue;
                int delta = (DIST_MAT(u_prev, v) + DIST_MAT(v, u_next) + NODE_COST(v)) - (DIST_MAT(u_prev, u) + DIST_MAT(u, u_next) + NODE_COST(u));
                if (delta < bestMove.delta)
                    bestMove = {2, delta, i, -1, v, -1};
            }
        }

        if (bestMove.delta < 0)
        {
            if (bestMove.type == 1)
            {
                reverseCircularSegment(sol, bestMove.u_next, bestMove.v); // Indices stored in u_next/v
            }
            else
            {
                sol[bestMove.u] = bestMove.v; // Index stored in u, new node in v
            }
            improvement = true;
        }
    }
}

#undef DIST_MAT
#undef NODE_COST

// ==================== HEA OPERATORS ====================

// Repair function from LNS (2-Regret)
std::vector<int> repairSolution_Regret(
    const std::vector<int> &partialSolution,
    int **distanceMatrix, const std::vector<int> &costVector,
    int totalNodes, int targetSize)
{
    std::vector<int> repaired = partialSolution;
    std::vector<bool> inSolution(totalNodes, false);
    for (int node : repaired)
        inSolution[node] = true;

    while ((int)repaired.size() < targetSize)
    {
        int bestNode = -1;
        int maxRegret = -1;
        int bestPosForBestNode = -1;

        for (int node = 0; node < totalNodes; ++node)
        {
            if (inSolution[node])
                continue;

            int bestIncrease = std::numeric_limits<int>::max();
            int secondBestIncrease = std::numeric_limits<int>::max();
            int currentBestPos = -1;

            int n = repaired.size();
            for (int p = 0; p < n; ++p)
            {
                int u = repaired[p];
                int v = repaired[(p + 1) % n];
                int increase = distanceMatrix[u][node] + distanceMatrix[node][v] - distanceMatrix[u][v] + costVector[node];

                if (increase < bestIncrease)
                {
                    secondBestIncrease = bestIncrease;
                    bestIncrease = increase;
                    currentBestPos = p;
                }
                else if (increase < secondBestIncrease)
                {
                    secondBestIncrease = increase;
                }
            }

            int regret = (secondBestIncrease == std::numeric_limits<int>::max()) ? bestIncrease : (secondBestIncrease - bestIncrease);
            if (regret > maxRegret)
            {
                maxRegret = regret;
                bestNode = node;
                bestPosForBestNode = currentBestPos;
            }
        }
        if (bestNode == -1)
            break;
        repaired.insert(repaired.begin() + bestPosForBestNode + 1, bestNode);
        inSolution[bestNode] = true;
    }
    return repaired;
}

// Operator 1: Common Edges/Nodes + Random Fill
std::vector<int> recombination_CommonComponents(
    const std::vector<int> &p1,
    const std::vector<int> &p2,
    int **distanceMatrix,
    const std::vector<int> &costVector,
    int totalNodes,
    std::mt19937 &g)
{
    int targetSize = p1.size();

    // 1. Identify Common Nodes
    std::vector<bool> inP1(totalNodes, false);
    for (int n : p1)
        inP1[n] = true;

    std::vector<int> commonNodes;
    std::vector<bool> isCommon(totalNodes, false);
    for (int n : p2)
    {
        if (inP1[n])
        {
            commonNodes.push_back(n);
            isCommon[n] = true;
        }
    }

    // 2. Identify Common Edges within Common Nodes
    // Adjacency list for the subgraph of common elements
    std::vector<std::vector<int>> adj(totalNodes);
    auto addEdges = [&](const std::vector<int> &p)
    {
        for (size_t i = 0; i < p.size(); ++i)
        {
            int u = p[i];
            int v = p[(i + 1) % p.size()];
            if (isCommon[u] && isCommon[v])
            {
                adj[u].push_back(v);
                adj[v].push_back(u);
            }
        }
    };
    // Note: We only want edges present in BOTH.
    // Let's create a set of edges for P1
    std::set<std::pair<int, int>> edgesP1;
    for (size_t i = 0; i < p1.size(); ++i)
    {
        int u = p1[i];
        int v = p1[(i + 1) % p1.size()];
        edgesP1.insert({std::min(u, v), std::max(u, v)});
    }

    std::vector<std::vector<int>> commonAdj(totalNodes);
    int commonEdgeCount = 0;
    for (size_t i = 0; i < p2.size(); ++i)
    {
        int u = p2[i];
        int v = p2[(i + 1) % p2.size()];
        std::pair<int, int> e = {std::min(u, v), std::max(u, v)};
        if (edgesP1.count(e))
        {
            commonAdj[u].push_back(v);
            commonAdj[v].push_back(u);
            commonEdgeCount++;
        }
    }

    // 3. Build Subpaths from Common Edges
    std::vector<bool> visited(totalNodes, false);
    std::vector<std::vector<int>> fragments;

    // Function to traverse a path segment
    for (int node : commonNodes)
    {
        if (!visited[node] && commonAdj[node].size() < 2)
        {
            // Start of a path or isolated node
            std::vector<int> path;
            int curr = node;
            while (true)
            {
                visited[curr] = true;
                path.push_back(curr);
                int next = -1;
                for (int neighbor : commonAdj[curr])
                {
                    if (!visited[neighbor])
                    {
                        next = neighbor;
                        break;
                    }
                }
                if (next == -1)
                    break;
                curr = next;
            }
            fragments.push_back(path);
        }
    }
    // Handle cycles (if common edges form a complete loop, break it to allow insertion)
    // or isolated nodes that were part of cycles but didn't trigger above
    for (int node : commonNodes)
    {
        if (!visited[node])
        {
            // Must be a cycle
            std::vector<int> path;
            int curr = node;
            while (true)
            {
                visited[curr] = true;
                path.push_back(curr);
                int next = -1;
                for (int neighbor : commonAdj[curr])
                {
                    if (!visited[neighbor])
                    {
                        next = neighbor;
                        break;
                    }
                }
                if (next == -1)
                    break;
                curr = next;
            }
            fragments.push_back(path);
        }
    }

    // 4. Fill with Random Nodes until targetSize
    std::vector<bool> inFragment(totalNodes, false);
    int currentCount = 0;
    for (const auto &frag : fragments)
    {
        for (int n : frag)
            inFragment[n] = true;
        currentCount += frag.size();
    }

    std::vector<int> available;
    for (int i = 0; i < totalNodes; ++i)
        if (!inFragment[i])
            available.push_back(i);
    std::shuffle(available.begin(), available.end(), g);

    int needed = targetSize - currentCount;
    for (int i = 0; i < needed && i < (int)available.size(); ++i)
    {
        fragments.push_back({available[i]});
    }

    // 5. Connect Fragments Randomly
    std::shuffle(fragments.begin(), fragments.end(), g);
    std::vector<int> offspring;
    for (const auto &frag : fragments)
    {
        // Decide orientation randomly
        if (g() % 2 == 0)
        {
            offspring.insert(offspring.end(), frag.begin(), frag.end());
        }
        else
        {
            offspring.insert(offspring.end(), frag.rbegin(), frag.rend());
        }
    }

    return offspring;
}

// Operator 2: Filter + Repair
std::vector<int> recombination_FilterRepair(
    const std::vector<int> &p1,
    const std::vector<int> &p2,
    int **distanceMatrix,
    const std::vector<int> &costVector,
    int totalNodes,
    int targetSize)
{
    // Start with P1
    std::vector<int> offspring;
    std::vector<bool> inP2(totalNodes, false);
    for (int n : p2)
        inP2[n] = true;

    // Remove nodes not in P2
    for (int node : p1)
    {
        if (inP2[node])
        {
            offspring.push_back(node);
        }
    }

    // Repair using Regret Heuristic
    return repairSolution_Regret(offspring, distanceMatrix, costVector, totalNodes, targetSize);
}

// ==================== HEA MAIN LOGIC ====================

struct Individual
{
    std::vector<int> solution;
    int cost;

    bool operator<(const Individual &other) const
    {
        return cost < other.cost;
    }
};

bool isUnique(const std::vector<Individual> &pop, const Individual &ind, bool useExactSolutionCheck = true)
{
    for (const auto &p : pop)
    {
        if (p.cost == ind.cost)
        {
            if (!useExactSolutionCheck)
                return false;
            // Exact check
            if (p.solution.size() != ind.solution.size())
                continue;
            // Check if vectors are identical (assuming standard orientation or sorted for set check)
            // For TSP, cycles can be rotated. Strict equality of vector implies strict sequence.
            // Requirement: "compare entire solution or value". Value is easiest.
            // Let's rely on cost uniqueness primarily as it's efficient for this assignment.
            return false;
        }
    }
    return true;
}

std::vector<int> runHEA(
    int **distanceMatrix,
    std::vector<int> &costVector,
    int size,
    int targetSize,
    int operatorType, // 1 = Common, 2 = Filter+LS, 3 = Filter+NoLS
    double timeLimit)
{
    std::random_device rd;
    std::mt19937 g{rd()};
    std::vector<Individual> population;
    int eliteSize = 20;

    auto startTime = std::chrono::high_resolution_clock::now();

    // 1. Initialize Population
    // Fill with random solutions + LS until we have 20 unique
    int attempts = 0;
    while (population.size() < eliteSize && attempts < 1000)
    {
        std::vector<int> sol = randomPermutation(size, g);
        applySteepestLS(sol, distanceMatrix, costVector, size);
        int c = evaluateSolution(sol, distanceMatrix, costVector);

        Individual ind = {sol, c};
        if (isUnique(population, ind))
        {
            population.push_back(ind);
        }
        attempts++;
    }
    std::sort(population.begin(), population.end()); // Best (lowest cost) first

    std::vector<int> bestSol = population[0].solution;
    int bestCost = population[0].cost;

    // 2. Steady State Loop
    while (true)
    {
        auto currTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = currTime - startTime;
        if (elapsed.count() >= timeLimit)
            break;

        // Parent Selection (Uniform)
        if (population.size() < 2)
            break; // Safety
        std::uniform_int_distribution<int> parentSelector(0, population.size() - 1);
        int idx1 = parentSelector(g);
        int idx2 = parentSelector(g);
        while (idx1 == idx2)
            idx2 = parentSelector(g);

        const std::vector<int> &p1 = population[idx1].solution;
        const std::vector<int> &p2 = population[idx2].solution;

        std::vector<int> childSol;

        // Apply Operator
        if (operatorType == 1)
        {
            childSol = recombination_CommonComponents(p1, p2, distanceMatrix, costVector, size, g);
            applySteepestLS(childSol, distanceMatrix, costVector, size); // Always LS for Op1
        }
        else if (operatorType == 2)
        {
            childSol = recombination_FilterRepair(p1, p2, distanceMatrix, costVector, size, targetSize);
            applySteepestLS(childSol, distanceMatrix, costVector, size); // LS enabled
        }
        else if (operatorType == 3)
        {
            childSol = recombination_FilterRepair(p1, p2, distanceMatrix, costVector, size, targetSize);
            // LS disabled (Operator 2 variant)
        }

        int childCost = evaluateSolution(childSol, distanceMatrix, costVector);
        Individual child = {childSol, childCost};

        // Population Management (Steady State)
        // If unique and better than the worst in population
        if (childCost < population.back().cost && isUnique(population, child))
        {
            population.back() = child; // Replace worst
            // Re-sort to maintain order
            std::sort(population.begin(), population.end());

            // Track Global Best
            if (population[0].cost < bestCost)
            {
                bestCost = population[0].cost;
                bestSol = population[0].solution;
            }
        }
    }

    return bestSol;
}

// ==================== EXPERIMENT RUNNER ====================

void runExperiment(const std::string &instanceName, int **distanceMatrix, std::vector<int> &costs, int size, int targetSize)
{
    int runs = 20;
    double timeLimit = (instanceName == "TSPA") ? 13.69 : 13.35; // Example limits from prev context, adjust if needed

    struct Result
    {
        std::string name;
        int minC = INT_MAX, maxC = INT_MIN;
        double avgC = 0;
        double avgTime = 0;
    };

    std::vector<std::pair<int, std::string>> variants = {
        {1, "HEA_Op1 (Common)"},
        {2, "HEA_Op2 (Filter+LS)"},
        {3, "HEA_Op2_NoLS (Filter)"}};

    std::cout << "Instance: " << instanceName << " (Target Size: " << targetSize << ")\n";
    std::cout << std::string(80, '-') << "\n";
    std::cout << std::left << std::setw(25) << "Method"
              << std::setw(10) << "Min"
              << std::setw(10) << "Avg"
              << std::setw(10) << "Max" << "\n";
    std::cout << std::string(80, '-') << "\n";

    for (auto &var : variants)
    {
        long long sumCost = 0;
        int bestOfVar = INT_MAX;
        int worstOfVar = INT_MIN;

        auto varStart = std::chrono::high_resolution_clock::now();

        std::vector<int> bestSolution;

        for (int r = 0; r < runs; ++r)
        {
            std::vector<int> sol = runHEA(distanceMatrix, costs, size, targetSize, var.first, timeLimit);
            int c = evaluateSolution(sol, distanceMatrix, costs);
            if (c < bestOfVar)
            {
                bestSolution = sol;
                bestOfVar = c;
            }
            if (c > worstOfVar)
                worstOfVar = c;
            sumCost += c;
        }

        auto varEnd = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> totalTime = varEnd - varStart;

        std::cout << std::left << std::setw(25) << var.second
                  << std::setw(10) << bestOfVar
                  << std::setw(10) << (sumCost / (double)runs)
                  << std::setw(10) << worstOfVar << "\n";

        std::cout << "Solution:";
        for (int node : bestSolution)
        {
            std::cout << " " << node;
        }
        std::cout << "\n";
    }
    std::cout << "\n";
}

int main()
{
    std::vector<std::string> files = {"TSPA.csv", "TSPB.csv"};
    for (const auto &fname : files)
    {
        std::vector<std::vector<int>> data;
        if (!getDataFromFile(fname, data))
        {
            if (!getDataFromFile("../" + fname, data))
                continue;
        }

        int size = data.size();
        int **distMat = getDistanceMatrix(data, size);
        std::vector<int> costs = getCostVector(data);
        int targetSize = size / 2 + (size % 2);

        runExperiment(fname.substr(0, 4), distMat, costs, size, targetSize);

        for (int i = 0; i < size; ++i)
            delete[] distMat[i];
        delete[] distMat;
    }
    return 0;
}