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

bool getDataFromFile(const std::string &filename, std::vector<std::vector<int>> &data)
{
    std::ifstream file(filename); // <-- use the passed filename
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
            try
            {
                row.push_back(std::stoi(value));
            }
            catch (const std::invalid_argument &e)
            {
                std::cerr << "Invalid data: " << value << std::endl;
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
    for (int16_t i = 0; i < size; i++)
    {
        distanceMatrix[i] = new int[size];
        for (int16_t j = 0; j < size; j++)
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
    {
        nodeCosts.push_back(row[2]);
    }

    data.clear();
    return nodeCosts;
}

int evaluateSolution(std::vector<int> &solution, int **distanceMatrix, std::vector<int> &costVector)
{
    int totalCost = 0;
    for (size_t i = 0; i < solution.size(); ++i)
    {
        totalCost += costVector[solution[i]];
        if (i > 0)
        {
            totalCost += distanceMatrix[solution[i - 1]][solution[i]];
        }
    }
    // Add cost to return to starting node to make it a cycle
    if (!solution.empty())
    {
        totalCost += distanceMatrix[solution.back()][solution.front()];
    }
    return totalCost;
}

// new helper: greedy insertion start (reuse in M6)
std::vector<int> constructGreedyInsertion(int **distanceMatrix, const std::vector<int> &costVector, int size, int startNode)
{
    // Nearest-neighbour insertion (single run, build a linear route using insertion positions 0..sz)
    // - select unused node with minimal distance to any node in current route (tie-break by cost then id)
    // - insert that node at the linear position (0..sz) that minimizes objective delta
    std::vector<int> solution;
    if (size <= 0) return solution;

    int nodesToVisit = (size % 2 == 0) ? (size / 2) : ((size + 1) / 2);

    solution.reserve(nodesToVisit);
    solution.push_back(startNode);

    std::vector<char> used(size, 0);
    used[startNode] = 1;

    while ((int)solution.size() < nodesToVisit)
    {
        int bestCandidate = -1;
        int bestNearest = std::numeric_limits<int>::max();

        // choose candidate by nearest distance to any node in current route
        for (int candidate = 0; candidate < size; ++candidate)
        {
            if (used[candidate]) continue;

            int nearest = std::numeric_limits<int>::max();
            for (int v : solution)
                nearest = std::min(nearest, distanceMatrix[candidate][v]);

            if (bestCandidate == -1 ||
                nearest < bestNearest ||
                (nearest == bestNearest &&
                 (costVector[candidate] < costVector[bestCandidate] ||
                  (costVector[candidate] == costVector[bestCandidate] && candidate < bestCandidate))))
            {
                bestNearest = nearest;
                bestCandidate = candidate;
            }
        }

        if (bestCandidate == -1) break;

        // choose best linear insertion position 0..sz (predecessor/successor logic like nearestNeighbourSolution)
        int sz = static_cast<int>(solution.size());
        int bestPos = 0;
        int bestDelta = std::numeric_limits<int>::max();

        for (int insertPos = 0; insertPos <= sz; ++insertPos)
        {
            int predecessor = (insertPos == 0) ? -1 : solution[insertPos - 1];
            int successor = (insertPos == sz) ? -1 : solution[insertPos];

            int addedDistance = 0;
            if (predecessor != -1) addedDistance += distanceMatrix[predecessor][bestCandidate];
            if (successor != -1) addedDistance += distanceMatrix[bestCandidate][successor];

            int removedDistance = 0;
            if (predecessor != -1 && successor != -1) removedDistance = distanceMatrix[predecessor][successor];

            int delta = costVector[bestCandidate] + (addedDistance - removedDistance);

            if (delta < bestDelta || (delta == bestDelta && insertPos < bestPos))
            {
                bestDelta = delta;
                bestPos = insertPos;
            }
        }

        solution.insert(solution.begin() + bestPos, bestCandidate);
        used[bestCandidate] = 1;
    }

    return solution;
}

// new helper: create random permutation (reuse in M5)
std::vector<int> randomPermutation(int size, std::mt19937 &g)
{
    int nodesToVisit = (size % 2 == 0) ? (size / 2) : ((size + 1) / 2);

    std::vector<int> solution(size);
    std::iota(solution.begin(), solution.end(), 0);
    std::shuffle(solution.begin(), solution.end(), g);
    solution.resize(nodesToVisit);
    return solution;
}

/*****************************************************************************************
-----------------------------------------------------------------------------
 * METHODS OVERVIEW
 * -----------------------------------------------------------------------------
 *
 * M1 (Kuba)
 *   Type: Steepest Descent
 *   Move: Two-Nodes Exchange
 *   Start: Random
 *   Description:
 *     - Generate random feasible solution
 *     - Evaluate all intra-route node swaps and inter-route exchanges
 *     - Apply the move with the best (lowest) delta until no improvement remains
 *
 * M2 (Kuba)
 *   Type: Steepest Descent
 *   Move: Two-Nodes Exchange
 *   Start: Greedy (random start node)
 *   Description:
 *     - Start from greedy construction heuristic with random start node
 *     - Explore all intra-route swaps and inter-route exchanges
 *     - Apply best improving move each iteration until local optimum
 *
 * M3 (Kuba)
 *   Type: Steepest Descent
 *   Move: Two-Edges Exchange (2-opt)
 *   Start: Random
 *   Description:
 *     - Start from random feasible solution
 *     - Evaluate all 2-opt intra-route moves and inter-route exchanges
 *     - Apply the best improving move until no improvement remains
 *
 * M4 (Kuba)
 *   Type: Steepest Descent
 *   Move: Two-Edges Exchange (2-opt)
 *   Start: Greedy (random start node)
 *   Description:
 *     - Start from greedy heuristic solution (random start node)
 *     - Explore all 2-opt intra-route and inter-route exchanges
 *     - Apply best improving move each iteration until local optimum
 *
 * -----------------------------------------------------------------------------
 *
 * M5 (Mari)
 *   Type: Greedy (First Improvement)
 *   Move: Two-Nodes Exchange
 *   Start: Random
 *   Description:
 *     - Start from random feasible solution
 *     - Randomly mix intra-route node swaps and inter-route exchanges
 *     - Apply the first improving move found
 *     - Stop when no improving move exists
 *
 * M6 (Mari)
 *   Type: Greedy (First Improvement)
 *   Move: Two-Nodes Exchange
 *   Start: Greedy (random start node)
 *   Description:
 *     - Start from greedy solution with random start node
 *     - Browse intra- and inter-route node exchanges in random order
 *     - Apply first improving move; stop when local optimum reached
 *
 * M7 (Mari)
 *   Type: Greedy (First Improvement)
 *   Move: Two-Edges Exchange (2-opt)
 *   Start: Random
 *   Description:
 *     - Start from random feasible solution
 *     - Randomly shuffle all 2-opt intra-route moves and inter-route exchanges
 *     - Apply first improving move; repeat until no further improvement
 *
 * M8 (Mari)
 *   Type: Greedy (First Improvement)
 *   Move: Two-Edges Exchange (2-opt)
 *   Start: Greedy (random start node)
 *   Description:
 *     - Start from greedy heuristic solution (random start node)
 *     - Randomly mix 2-opt intra-route and inter-route exchanges
 *     - Apply first improving move that reduces objective value
 ***************************************************************************************/
void M1_steepestDescent_TwoNodeExchange_RandomStart(int **distanceMatrix, std::vector<int> &costVector, int size, int totalRuns = 200)
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
        int solSize = static_cast<int>(solution.size());
        if (solSize <= 0) continue;

        int currentCost = evaluateSolution(solution, distanceMatrix, costVector);

        bool improved = true;
        while (improved)
        {
            improved = false;
            int bestDelta = 0;

            // Best move storage
            int bestMoveType = 0;   // 0=none, 1=intra-swap, 2=inter-exchange
            int bestI = -1;
            int bestJ = -1;         // For intra-swap
            int bestNewNode = -1;   // For inter-exchange

            solSize = static_cast<int>(solution.size()); // Re-check size
            for (int i = 0; i < solSize - 1; ++i)
            {
                for (int j = i + 1; j < solSize; ++j)
                {
                    std::swap(solution[i], solution[j]);
                    int newCost = evaluateSolution(solution, distanceMatrix, costVector);
                    int delta = newCost - currentCost;
                    if (delta < bestDelta)
                    {
                        bestDelta = delta;
                        bestMoveType = 1;
                        bestI = i;
                        bestJ = j;
                    }
                    std::swap(solution[i], solution[j]); // Swap back
                }
            }

            std::vector<char> used(size, 0);
            for (int v : solution) used[v] = 1;

            for (int i = 0; i < solSize; ++i) // For each node in solution
            {
                int oldNode = solution[i];
                for (int newNode = 0; newNode < size; ++newNode) // For each node not in solution
                {
                    if (used[newNode]) continue;

                    solution[i] = newNode;
                    int newCost = evaluateSolution(solution, distanceMatrix, costVector);
                    int delta = newCost - currentCost;

                    if (delta < bestDelta)
                    {
                        bestDelta = delta;
                        bestMoveType = 2;
                        bestI = i; // position to swap
                        bestNewNode = newNode; // node to swap in
                    }
                }
                solution[i] = oldNode; // Swap back
            }

            if (bestDelta < 0)
            {
                if (bestMoveType == 1) // Intra-route swap
                {
                    std::swap(solution[bestI], solution[bestJ]);
                }
                else if (bestMoveType == 2) // Inter-route exchange
                {
                    solution[bestI] = bestNewNode;
                }
                currentCost += bestDelta;
                improved = true;
            }
        } // end while(improved)

        totalSum += currentCost;
        if (currentCost < bestObjective)
        {
            bestObjective = currentCost;
            bestSolution = solution;
        }
        if (currentCost > worstObjective)
            worstObjective = currentCost;
    }
    auto endTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsedSeconds = endTime - startTime;

    double averageObjective = static_cast<double>(totalSum) / totalRuns;
    std::cout << "====== M1 (Steepest Descent, 2-node exchange, random start) ======\n";
    std::cout << "  runs = " << totalRuns << "\n";
    std::cout << "  min = " << bestObjective << "\n";
    std::cout << "  max = " << worstObjective << "\n";
    std::cout << "  avg = " << averageObjective << "\n";
    std::cout << "Execution time: " << elapsedSeconds.count() << " seconds\n\n";

    if (!bestSolution.empty())
    {
        std::cout << "Best cycle route: ";
        for (const auto &n : bestSolution) std::cout << n << " ";
        std::cout << bestSolution.front() << " (back to start)\n";
    }
    else
    {
        std::cout << "No solution found.\n";
    }
}

void M2_steepestDescent_TwoNodeExchange_GreedyStart(int **distanceMatrix, std::vector<int> &costVector, int size, int totalRuns = 200)
{
    if (size <= 0) return;

    std::random_device rd;
    std::mt19937 g(rd());

    long long totalSum = 0;
    int bestObjective = std::numeric_limits<int>::max();
    int worstObjective = std::numeric_limits<int>::min();
    std::uniform_int_distribution<int> startDist(0, size - 1);

    std::vector<int> bestSolution;
    auto startTime = std::chrono::high_resolution_clock::now();
    for (int run = 0; run < totalRuns; ++run)
    {
        int startNode = startDist(g);
        std::vector<int> solution = constructGreedyInsertion(distanceMatrix, costVector, size, startNode);
        int solSize = static_cast<int>(solution.size());
        if (solSize <= 0) continue;
        
        int currentCost = evaluateSolution(solution, distanceMatrix, costVector);

        bool improved = true;
        while(improved)
        {
            improved = false;
            int bestDelta = 0;

            int bestMoveType = 0;   // 0=none, 1=intra-swap, 2=inter-exchange
            int bestI = -1;
            int bestJ = -1;         // For intra-swap
            int bestNewNode = -1;   // For inter-exchange

            // --- Neighborhood 1: Intra-route node swap ---
            solSize = static_cast<int>(solution.size());
            for (int i = 0; i < solSize - 1; ++i)
            {
                for (int j = i + 1; j < solSize; ++j)
                {
                    std::swap(solution[i], solution[j]);
                    int newCost = evaluateSolution(solution, distanceMatrix, costVector);
                    int delta = newCost - currentCost;
                    if (delta < bestDelta)
                    {
                        bestDelta = delta;
                        bestMoveType = 1;
                        bestI = i;
                        bestJ = j;
                    }
                    std::swap(solution[i], solution[j]); // Swap back
                }
            }

            std::vector<char> used(size, 0);
            for (int v : solution) used[v] = 1;

            for (int i = 0; i < solSize; ++i) // For each node in solution
            {
                int oldNode = solution[i];
                for (int newNode = 0; newNode < size; ++newNode) // For each node not in solution
                {
                    if (used[newNode]) continue;

                    solution[i] = newNode;
                    int newCost = evaluateSolution(solution, distanceMatrix, costVector);
                    int delta = newCost - currentCost;

                    if (delta < bestDelta)
                    {
                        bestDelta = delta;
                        bestMoveType = 2;
                        bestI = i; // position to swap
                        bestNewNode = newNode; // node to swap in
                    }
                }
                solution[i] = oldNode; // Swap back
            }

            if (bestDelta < 0)
            {
                if (bestMoveType == 1) // Intra-route swap
                {
                    std::swap(solution[bestI], solution[bestJ]);
                }
                else if (bestMoveType == 2) // Inter-route exchange
                {
                    solution[bestI] = bestNewNode;
                }
                currentCost += bestDelta;
                improved = true;
            }
        } // end while(improved)

        totalSum += currentCost;
        if (currentCost < bestObjective)
        {
            bestObjective = currentCost;
            bestSolution = solution;
        }
        if (currentCost > worstObjective)
            worstObjective = currentCost;
    }
    auto endTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsedSeconds = endTime - startTime;

    double averageObjective = static_cast<double>(totalSum) / totalRuns;
    std::cout << "====== M2 (Steepest Descent, 2-node exchange, greedy start) ======\n";
    std::cout << "  runs = " << totalRuns << "\n";
    std::cout << "  min = " << bestObjective << "\n";
    std::cout << "  max = " << worstObjective << "\n";
    std::cout << "  avg = " << averageObjective << "\n";
    std::cout << "Execution time: " << elapsedSeconds.count() << " seconds\n\n";

    if (!bestSolution.empty())
    {
        std::cout << "Best cycle route: ";
        for (const auto &n : bestSolution) std::cout << n << " ";
        std::cout << bestSolution.front() << " (back to start)\n";
    }
    else
    {
        std::cout << "No solution found.\n";
    }
}

void M3_steepestDescent_TwoEdgeExchange_RandomStart(int **distanceMatrix, std::vector<int> &costVector, int size, int totalRuns = 200)
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
        // BUG 4 FIX: Removed debug print
        std::vector<int> solution = randomPermutation(size, g);
        int solSize = static_cast<int>(solution.size());
        if (solSize <= 0) continue;

        int currentCost = evaluateSolution(solution, distanceMatrix, costVector);

        bool improved = true;
        while (improved)
        {
            improved = false;
            int bestDelta = 0;

            // Best move storage
            int bestMoveType = 0;   // 0=none, 1=intra-2-opt, 2=inter-exchange
            int bestI = -1;
            int bestJ = -1;         // For 2-opt
            int bestNewNode = -1;   // For inter-exchange

            solSize = static_cast<int>(solution.size());
            for (int i = 0; i < solSize - 1; ++i)
            {
                for (int j = i + 1; j < solSize; ++j)
                {
                    std::reverse(solution.begin() + i, solution.begin() + j + 1); // Reverse segment [i, j]
                    int newCost = evaluateSolution(solution, distanceMatrix, costVector);
                    int delta = newCost - currentCost;
                    if (delta < bestDelta)
                    {
                        bestDelta = delta;
                        bestMoveType = 1;
                        bestI = i;
                        bestJ = j;
                    }
                    std::reverse(solution.begin() + i, solution.begin() + j + 1); // Swap back
                }
            }

            std::vector<char> used(size, 0);
            for (int v : solution) used[v] = 1;

            for (int i = 0; i < solSize; ++i) // For each node in solution
            {
                int oldNode = solution[i];
                for (int newNode = 0; newNode < size; ++newNode) // For each node not in solution
                {
                    if (used[newNode]) continue;

                    solution[i] = newNode;
                    int newCost = evaluateSolution(solution, distanceMatrix, costVector);
                    int delta = newCost - currentCost;

                    if (delta < bestDelta)
                    {
                        bestDelta = delta;
                        bestMoveType = 2;
                        bestI = i; // position to swap
                        bestNewNode = newNode; // node to swap in
                    }
                }
                solution[i] = oldNode; // Swap back
            }

            if (bestDelta < 0)
            {
                if (bestMoveType == 1) // Intra-route 2-opt
                {
                    std::reverse(solution.begin() + bestI, solution.begin() + bestJ + 1);
                }
                else if (bestMoveType == 2) // Inter-route exchange
                {
                    solution[bestI] = bestNewNode;
                }
                currentCost += bestDelta;
                improved = true;
            }
        } // end while(improved)

        totalSum += currentCost;
        if (currentCost < bestObjective)
        {
            bestObjective = currentCost;
            bestSolution = solution;
        }
        if (currentCost > worstObjective)
            worstObjective = currentCost;
    }
    auto endTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsedSeconds = endTime - startTime;

    double averageObjective = static_cast<double>(totalSum) / totalRuns;
    std::cout << "====== M3 (Steepest Descent, 2-edge exchange, random start) ======\n";
    std::cout << "  runs = " << totalRuns << "\n";
    std::cout << "  min = " << bestObjective << "\n";
    std::cout << "  max = " << worstObjective << "\n";
    std::cout << "  avg = " << averageObjective << "\n";
    std::cout << "Execution time: " << elapsedSeconds.count() << " seconds\n\n";

    if (!bestSolution.empty())
    {
        std::cout << "Best cycle route: ";
        for (const auto &n : bestSolution) std::cout << n << " ";
        std::cout << bestSolution.front() << " (back to start)\n";
    }
    else
    {
        std::cout << "No solution found.\n";
    }
}

void M4_steepestDescent_TwoEdgeExchange_GreedyStart(int **distanceMatrix, std::vector<int> &costVector, int size, int totalRuns = 200)
{
    if (size <= 0) return;

    std::random_device rd;
    std::mt19937 g(rd());

    long long totalSum = 0;
    int bestObjective = std::numeric_limits<int>::max();
    int worstObjective = std::numeric_limits<int>::min();
    std::vector<int> bestSolution;
    auto startTime = std::chrono::high_resolution_clock::now();

    std::uniform_int_distribution<int> startDist(0, size - 1);

    for (int run = 0; run < totalRuns; run++)
    {
        int startNode = startDist(g);
        std::vector<int> solution = constructGreedyInsertion(distanceMatrix, costVector, size, startNode);
        int solSize = static_cast<int>(solution.size());
        if (solSize <= 0) continue;
        
        int currentCost = evaluateSolution(solution, distanceMatrix, costVector);

        bool improved = true;
        while (improved)
        {
            improved = false;
            int bestDelta = 0;

            // Best move storage
            int bestMoveType = 0;   // 0=none, 1=intra-2-opt, 2=inter-exchange
            int bestI = -1;
            int bestJ = -1;         // For 2-opt
            int bestNewNode = -1;   // For inter-exchange

            // --- Neighborhood 1: Intra-route 2-edge exchange (2-opt) ---
            solSize = static_cast<int>(solution.size());
            for (int i = 0; i < solSize - 1; ++i)
            {
                for (int j = i + 1; j < solSize; ++j)
                {
                    std::reverse(solution.begin() + i, solution.begin() + j + 1); // Reverse segment [i, j]
                    int newCost = evaluateSolution(solution, distanceMatrix, costVector);
                    int delta = newCost - currentCost;
                    if (delta < bestDelta)
                    {
                        bestDelta = delta;
                        bestMoveType = 1;
                        bestI = i;
                        bestJ = j;
                    }
                    std::reverse(solution.begin() + i, solution.begin() + j + 1); // Swap back
                }
            }

            std::vector<char> used(size, 0);
            for (int v : solution) used[v] = 1;

            for (int i = 0; i < solSize; ++i) // For each node in solution
            {
                int oldNode = solution[i];
                for (int newNode = 0; newNode < size; ++newNode) // For each node not in solution
                {
                    if (used[newNode]) continue;

                    solution[i] = newNode;
                    int newCost = evaluateSolution(solution, distanceMatrix, costVector);
                    int delta = newCost - currentCost;

                    if (delta < bestDelta)
                    {
                        bestDelta = delta;
                        bestMoveType = 2;
                        bestI = i; // position to swap
                        bestNewNode = newNode; // node to swap in
                    }
                }
                solution[i] = oldNode; // Swap back
            }

            if (bestDelta < 0)
            {
                if (bestMoveType == 1) // Intra-route 2-opt
                {
                    std::reverse(solution.begin() + bestI, solution.begin() + bestJ + 1);
                }
                else if (bestMoveType == 2) // Inter-route exchange
                {
                    solution[bestI] = bestNewNode;
                }
                currentCost += bestDelta;
                improved = true;
            }
        } // end while(improved)

        totalSum += currentCost;
        if (currentCost < bestObjective)
        {
            bestObjective = currentCost;
            bestSolution = solution;
        }
        if (currentCost > worstObjective)
            worstObjective = currentCost;
    }
    auto endTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsedSeconds = endTime - startTime;

    double averageObjective = static_cast<double>(totalSum) / totalRuns;
    std::cout << "====== M4 (Steepest Descent, 2-edge exchange, greedy start) ======\n";
    std::cout << "  runs = " << totalRuns << "\n";
    std::cout << "  min = " << bestObjective << "\n";
    std::cout << "  max = " << worstObjective << "\n";
    std::cout << "  avg = " << averageObjective << "\n";
    std::cout << "Execution time: " << elapsedSeconds.count() << " seconds\n\n";

    if (!bestSolution.empty())
    {
        std::cout << "Best cycle route: ";
        for (const auto &n : bestSolution) std::cout << n << " ";
        std::cout << bestSolution.front() << " (back to start)\n";
    }
    else
    {
        std::cout << "No solution found.\n";
    }
}

void M5_greedyFirstImprovement_TwoNodeExchange_RandomStart(int **distanceMatrix, std::vector<int> &costVector, int size, int totalRuns = 200)
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
        int solSize = static_cast<int>(solution.size());
        if (solSize <= 0) continue;

        int currentCost = evaluateSolution(solution, distanceMatrix, costVector);

        bool improved = true;
        while (improved)
        {
            improved = false;

            
            std::vector<int> order(solSize);
            std::iota(order.begin(), order.end(), 0);
            std::shuffle(order.begin(), order.end(), g);

            
            std::vector<int> moveTypes = {0, 1};
            std::shuffle(moveTypes.begin(), moveTypes.end(), g);

            for (int moveType : moveTypes)
            {
                if (moveType == 0 && !improved)
                {
                    
                    for (int oi = 0; oi < solSize - 1 && !improved; ++oi)
                    {
                        int i = order[oi];
                        for (int oj = oi + 1; oj < solSize; ++oj)
                        {
                            int j = order[oj];

                            std::swap(solution[i], solution[j]);
                            int newCost = evaluateSolution(solution, distanceMatrix, costVector);
                            if (newCost < currentCost)
                            {
                                currentCost = newCost;
                                improved = true;
                                break; 
                            }
                            else
                            {
                                std::swap(solution[i], solution[j]); 
                            }
                        }
                    }
                }
                else if (moveType == 1 && !improved)
                {
                    
                    std::vector<char> used(size, 0);
                    for (int v : solution) used[v] = 1;
                    std::vector<int> notSelected;
                    for (int node = 0; node < size; ++node)
                        if (!used[node]) notSelected.push_back(node);

                    if (!notSelected.empty())
                    {
                        std::shuffle(notSelected.begin(), notSelected.end(), g);
                        for (int oi2 = 0; oi2 < solSize && !improved; ++oi2)
                        {
                            int selIndex = order[oi2];
                            int oldNode = solution[selIndex];
                            for (int nc = 0; nc < (int)notSelected.size() && !improved; ++nc)
                            {
                                solution[selIndex] = notSelected[nc];
                                int newCost = evaluateSolution(solution, distanceMatrix, costVector);
                                if (newCost < currentCost)
                                {
                                    currentCost = newCost;
                                    improved = true;
                                    break;
                                }
                                else
                                {
                                    solution[selIndex] = oldNode; 
                                }
                            }
                        }
                    }
                }
            }
        }

        totalSum += currentCost;
        if (currentCost < bestObjective)
        {
            bestObjective = currentCost;
            bestSolution = solution;
        }
        if (currentCost > worstObjective)
            worstObjective = currentCost;
    }

    auto endTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsedSeconds = endTime - startTime;

    double averageObjective = static_cast<double>(totalSum) / totalRuns;
    std::cout << "====== M5 (Greedy First-Improvement, 2-node exchange, random start) ======\n";
    std::cout << "  runs = " << totalRuns << "\n";
    std::cout << "  min = " << bestObjective << "\n";
    std::cout << "  max = " << worstObjective << "\n";
    std::cout << "  avg = " << averageObjective << "\n";
    std::cout << "Execution time: " << elapsedSeconds.count() << " seconds\n\n";

    if (!bestSolution.empty())
    {
        std::cout << "Best cycle route: ";
        for (const auto &n : bestSolution) std::cout << n << " ";
        std::cout << bestSolution.front() << " (back to start)\n";
    }
    else
    {
        std::cout << "No solution found.\n";
    }
}

void M6_greedyFirstImprovement_TwoNodeExchange_GreedyStart(int **distanceMatrix, std::vector<int> &costVector, int size, int totalRuns = 200)
{
    if (size <= 0) return;

    std::random_device rd;
    std::mt19937 g(rd());
    std::uniform_int_distribution<int> startDist(0, size - 1);

    long long totalSum = 0;
    int bestObjective = std::numeric_limits<int>::max();
    int worstObjective = std::numeric_limits<int>::min();
    std::vector<int> bestSolution;

    auto startTime = std::chrono::high_resolution_clock::now();

    for (int run = 0; run < totalRuns; ++run)
    {
        int startNode = startDist(g);
        std::vector<int> solution = constructGreedyInsertion(distanceMatrix, costVector, size, startNode);
        int solSize = static_cast<int>(solution.size());
        if (solSize <= 0) continue;

        int currentCost = evaluateSolution(solution, distanceMatrix, costVector);

        bool improved = true;
        while (improved)
        {
            improved = false;

            
            std::vector<int> order(solSize);
            std::iota(order.begin(), order.end(), 0);
            std::shuffle(order.begin(), order.end(), g);

            
            std::vector<int> moveTypes = {0, 1};
            std::shuffle(moveTypes.begin(), moveTypes.end(), g);

            for (int moveType : moveTypes)
            {
                if (moveType == 0 && !improved)
                {
                    
                    for (int oi = 0; oi < solSize - 1 && !improved; ++oi)
                    {
                        int i = order[oi];
                        for (int oj = oi + 1; oj < solSize; ++oj)
                        {
                            int j = order[oj];
                            std::swap(solution[i], solution[j]);
                            int newCost = evaluateSolution(solution, distanceMatrix, costVector);
                            if (newCost < currentCost)
                            {
                                currentCost = newCost;
                                improved = true;
                                break;
                            }
                            else
                            {
                                std::swap(solution[i], solution[j]); 
                            }
                        }
                    }
                }
                else if (moveType == 1 && !improved)
                {
                    
                    std::vector<char> used(size, 0);
                    for (int v : solution) used[v] = 1;
                    std::vector<int> notSelected;
                    for (int node = 0; node < size; ++node)
                        if (!used[node]) notSelected.push_back(node);

                    if (!notSelected.empty())
                    {
                        std::shuffle(notSelected.begin(), notSelected.end(), g);
                        for (int oi2 = 0; oi2 < solSize && !improved; ++oi2)
                        {
                            int selIndex = order[oi2];
                            int oldNode = solution[selIndex];
                            for (int nc = 0; nc < (int)notSelected.size() && !improved; ++nc)
                            {
                                solution[selIndex] = notSelected[nc];
                                int newCost = evaluateSolution(solution, distanceMatrix, costVector);
                                if (newCost < currentCost)
                                {
                                    currentCost = newCost;
                                    improved = true;
                                    break;
                                }
                                else
                                {
                                    solution[selIndex] = oldNode;
                                }
                            }
                        }
                    }
                }
            } 
        } 

        totalSum += currentCost;
        if (currentCost < bestObjective)
        {
            bestObjective = currentCost;
            bestSolution = solution;
        }
        if (currentCost > worstObjective)
            worstObjective = currentCost;
    }

    auto endTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsedSeconds = endTime - startTime;

    double averageObjective = static_cast<double>(totalSum) / totalRuns;
    std::cout << "====== M6 (Greedy First-Improvement, 2-node exchange, greedy start) ======\n";
    std::cout << "  runs = " << totalRuns << "\n";
    std::cout << "  min = " << bestObjective << "\n";
    std::cout << "  max = " << worstObjective << "\n";
    std::cout << "  avg = " << averageObjective << "\n";
    std::cout << "Execution time: " << elapsedSeconds.count() << " seconds\n\n";

    if (!bestSolution.empty())
    {
        std::cout << "Best cycle route: ";
        for (const auto &n : bestSolution) std::cout << n << " ";
        std::cout << bestSolution.front() << " (back to start)\n";
    }
    else
    {
        std::cout << "No solution found.\n";
    }
}

void M7_greedyFirstImprovement_TwoEdgeExchange_RandomStart(int **distanceMatrix, std::vector<int> &costVector, int size, int totalRuns = 200)
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
        int solSize = static_cast<int>(solution.size());
        if (solSize <= 0) continue;

        int currentCost = evaluateSolution(solution, distanceMatrix, costVector);
        bool improved = true;

        while (improved)
        {
            improved = false;


            std::vector<int> order(solSize);
            std::iota(order.begin(), order.end(), 0);
            std::shuffle(order.begin(), order.end(), g);

            std::vector<int> moveTypes = {0, 1};
            std::shuffle(moveTypes.begin(), moveTypes.end(), g);

            for (int moveType : moveTypes)
            {
                if (moveType == 0 && !improved)
                {
                    for (int oi = 0; oi < solSize - 1 && !improved; ++oi)
                    {
                        int pos_i = order[oi];
                        for (int oj = oi + 1; oj < solSize && !improved; ++oj)
                        {
                            int pos_j = order[oj];
                            int a = std::min(pos_i, pos_j);
                            int b = std::max(pos_i, pos_j);
                            std::reverse(solution.begin() + a, solution.begin() + b + 1);
                            int newCost = evaluateSolution(solution, distanceMatrix, costVector);
                            if (newCost < currentCost)
                            {
                                currentCost = newCost;
                                improved = true;
                                break; 
                            }
                            else
                            {
                                std::reverse(solution.begin() + a, solution.begin() + b + 1); // revert
                            }
                        }
                    }
                }
                else if (moveType == 1 && !improved)
                {

                    std::vector<char> used(size, 0);
                    for (int v : solution) used[v] = 1;
                    std::vector<int> notSelected;
                    for (int node = 0; node < size; ++node)
                        if (!used[node]) notSelected.push_back(node);

                    if (!notSelected.empty())
                    {
                        std::shuffle(notSelected.begin(), notSelected.end(), g);
                        for (int oi2 = 0; oi2 < solSize && !improved; ++oi2)
                        {
                            int selIndex = order[oi2];
                            int oldNode = solution[selIndex];
                            for (int nc = 0; nc < (int)notSelected.size() && !improved; ++nc)
                            {
                                solution[selIndex] = notSelected[nc];
                                int newCost = evaluateSolution(solution, distanceMatrix, costVector);
                                if (newCost < currentCost)
                                {
                                    currentCost = newCost;
                                    improved = true;
                                    break;
                                }
                                else
                                {
                                    solution[selIndex] = oldNode;
                                }
                            }
                        }
                    }
                }
            }
        }

        totalSum += currentCost;
        if (currentCost < bestObjective) { bestObjective = currentCost; bestSolution = solution; }
        if (currentCost > worstObjective) worstObjective = currentCost;
    }

    auto endTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = endTime - startTime;
    double averageObjective = static_cast<double>(totalSum) / totalRuns;

    std::cout << "====== M7 (Greedy First-Improvement, 2-edge exchange, random start) ======\n";
    std::cout << "  runs = " << totalRuns << "\n";
    std::cout << "  min = " << bestObjective << "\n";
    std::cout << "  max = " << worstObjective << "\n";
    std::cout << "  avg = " << averageObjective << "\n";
    std::cout << "Execution time: " << elapsed.count() << " seconds\n\n";

    if (!bestSolution.empty())
    {
        std::cout << "Best cycle route: ";
        for (const auto &n : bestSolution) std::cout << n << " ";
        std::cout << bestSolution.front() << " (back to start)\n";
    }
    else
    {
        std::cout << "No solution found.\n";
    }
}

void M8_greedyFirstImprovement_TwoEdgeExchange_GreedyStart(int **distanceMatrix, std::vector<int> &costVector, int size, int totalRuns = 200)
{
    if (size <= 0) return;
    std::random_device rd;
    std::mt19937 g(rd());
    std::uniform_int_distribution<int> startDist(0, size - 1);

    long long totalSum = 0;
    int bestObjective = std::numeric_limits<int>::max();
    int worstObjective = std::numeric_limits<int>::min();
    std::vector<int> bestSolution;
    auto startTime = std::chrono::high_resolution_clock::now();

    for (int run = 0; run < totalRuns; ++run)
    {
        int startNode = startDist(g);
        std::vector<int> solution = constructGreedyInsertion(distanceMatrix, costVector, size, startNode);
        int solSize = static_cast<int>(solution.size());
        if (solSize <= 0) continue;

        int currentCost = evaluateSolution(solution, distanceMatrix, costVector);
        bool improved = true;

        while (improved)
        {
            improved = false;


            std::vector<int> order(solSize);
            std::iota(order.begin(), order.end(), 0);
            std::shuffle(order.begin(), order.end(), g);

           
            std::vector<int> moveTypes = {0, 1};
            std::shuffle(moveTypes.begin(), moveTypes.end(), g);

            for (int moveType : moveTypes)
            {
                if (moveType == 0 && !improved)
                {

                    for (int oi = 0; oi < solSize - 1 && !improved; ++oi)
                    {
                        int pos_i = order[oi];
                        for (int oj = oi + 1; oj < solSize && !improved; ++oj)
                        {
                            int pos_j = order[oj];
                            int a = std::min(pos_i, pos_j);
                            int b = std::max(pos_i, pos_j);
                            std::reverse(solution.begin() + a, solution.begin() + b + 1);
                            int newCost = evaluateSolution(solution, distanceMatrix, costVector);
                            if (newCost < currentCost)
                            {
                                currentCost = newCost;
                                improved = true;
                                break;
                            }
                            else
                            {
                                std::reverse(solution.begin() + a, solution.begin() + b + 1); // revert
                            }
                        }
                    }
                }
                else if (moveType == 1 && !improved)
                {
                    std::vector<char> used(size, 0);
                    for (int v : solution) used[v] = 1;
                    std::vector<int> notSelected;
                    for (int node = 0; node < size; ++node)
                        if (!used[node]) notSelected.push_back(node);

                    if (!notSelected.empty())
                    {
                        std::shuffle(notSelected.begin(), notSelected.end(), g);
                        for (int oi2 = 0; oi2 < solSize && !improved; ++oi2)
                        {
                            int selIndex = order[oi2];
                            int oldNode = solution[selIndex];
                            for (int nc = 0; nc < (int)notSelected.size() && !improved; ++nc)
                            {
                                solution[selIndex] = notSelected[nc];
                                int newCost = evaluateSolution(solution, distanceMatrix, costVector);
                                if (newCost < currentCost)
                                {
                                    currentCost = newCost;
                                    improved = true;
                                    break;
                                }
                                else
                                {
                                    solution[selIndex] = oldNode;
                                }
                            }
                        }
                    }
                }
            }
        }

        totalSum += currentCost;
        if (currentCost < bestObjective) { bestObjective = currentCost; bestSolution = solution; }
        if (currentCost > worstObjective) worstObjective = currentCost;
    }

    auto endTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = endTime - startTime;
    double averageObjective = static_cast<double>(totalSum) / totalRuns;

    std::cout << "====== M8 (Greedy First-Improvement, 2-edge exchange, greedy start) ======\n";
    std::cout << "  runs = " << totalRuns << "\n";
    std::cout << "  min = " << bestObjective << "\n";
    std::cout << "  max = " << worstObjective << "\n";
    std::cout << "  avg = " << averageObjective << "\n";
    std::cout << "Execution time: " << elapsed.count() << " seconds\n\n";

    if (!bestSolution.empty())
    {
        std::cout << "Best cycle route: ";
        for (const auto &n : bestSolution) std::cout << n << " ";
        std::cout << bestSolution.front() << " (back to start)\n";
    }
    else
    {
        std::cout << "No solution found.\n";
    }
}

int main()
{
    std::vector<std::string> fileNames = {"../TSPA.csv", "../TSPB.csv"};

    for (const auto &FILE_NAME : fileNames)
    {
        std::vector<std::vector<int>> data;
        if (!getDataFromFile(FILE_NAME, data))
        {
            std::cerr << "Failed to read data from file: " << FILE_NAME << std::endl;
            continue; 
        }

        int size = data.size();
        int **distanceMatrix = getDistanceMatrix(data, size);
        std::vector<int> costVector = getCostVector(data);

        std::cout << "\nRunning M1 on file: " << FILE_NAME << std::endl;
        M1_steepestDescent_TwoNodeExchange_RandomStart(distanceMatrix, costVector, size);

        std::cout << "\nRunning M2 on file: " << FILE_NAME << std::endl;
        M2_steepestDescent_TwoNodeExchange_GreedyStart(distanceMatrix, costVector, size);

        std::cout << "\nRunning M3 on file: " << FILE_NAME << std::endl;
        M3_steepestDescent_TwoEdgeExchange_RandomStart(distanceMatrix, costVector, size);

        std::cout << "\nRunning M4 on file: " << FILE_NAME << std::endl;
        M4_steepestDescent_TwoEdgeExchange_GreedyStart(distanceMatrix, costVector, size);
        /******** 
        std::cout << "\nRunning M5 on file: " << FILE_NAME << std::endl;
        M5_greedyFirstImprovement_TwoNodeExchange_RandomStart(distanceMatrix, costVector, size);

        std::cout << "\nRunning M6 on file: " << FILE_NAME << std::endl;
        M6_greedyFirstImprovement_TwoNodeExchange_GreedyStart(distanceMatrix, costVector, size);

        std::cout << "\nRunning M7 on file: " << FILE_NAME << std::endl;
        M7_greedyFirstImprovement_TwoEdgeExchange_RandomStart(distanceMatrix, costVector, size);

        std::cout << "\nRunning M8 on file: " << FILE_NAME << std::endl;
        M8_greedyFirstImprovement_TwoEdgeExchange_GreedyStart(distanceMatrix, costVector, size);*/

        for (int i = 0; i < size; i++)
        {
            delete[] distanceMatrix[i];
        }
        delete[] distanceMatrix;
    }

    return 0;
}
