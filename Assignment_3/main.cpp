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
    std::vector<int> solution;
    if (size <= 0) return solution;

    solution.reserve(size);
    solution.push_back(startNode);

    std::vector<char> used(size, 0);
    used[startNode] = 1;

    while ((int)solution.size() < size)
    {
        int bestCandidate = -1;
        int bestPos = -1;
        int bestCost = std::numeric_limits<int>::max();

        for (int candidate = 0; candidate < size; ++candidate)
        {
            if (used[candidate]) continue;

            for (int pos = 0; pos <= (int)solution.size(); ++pos)
            {
                int added = 0;
                int removed = 0;
                if (pos > 0) added += distanceMatrix[solution[pos - 1]][candidate];
                if (pos < (int)solution.size()) added += distanceMatrix[candidate][solution[pos]];
                if (pos > 0 && pos < (int)solution.size()) removed = distanceMatrix[solution[pos - 1]][solution[pos]];

                int cost = costVector[candidate] + (added - removed);
                if (cost < bestCost)
                {
                    bestCost = cost;
                    bestCandidate = candidate;
                    bestPos = pos;
                }
            }
        }

        if (bestCandidate == -1) break;
        solution.insert(solution.begin() + bestPos, bestCandidate);
        used[bestCandidate] = 1;
    }

    return solution;
}

// new helper: create random permutation (reuse in M5)
std::vector<int> randomPermutation(int size, std::mt19937 &g)
{
    std::vector<int> solution(size);
    std::iota(solution.begin(), solution.end(), 0);
    std::shuffle(solution.begin(), solution.end(), g);
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
        // random initial feasible solution (permutation of nodes)
        std::vector<int> solution = randomPermutation(size, g);

        int currentCost = evaluateSolution(solution, distanceMatrix, costVector);

        bool improved = true;
        while (improved)
        {
            improved = false;

            // generate all possible two-node exchanges (i < j) and shuffle order
            std::vector<std::pair<int, int>> moves;
            moves.reserve((size * (size - 1)) / 2);
            for (int i = 0; i < size - 1; ++i)
                for (int j = i + 1; j < size; ++j)
                    moves.emplace_back(i, j);

            std::shuffle(moves.begin(), moves.end(), g);

            // apply first improving move found
            for (const auto &mv : moves)
            {
                int i = mv.first;
                int j = mv.second;
                std::swap(solution[i], solution[j]);
                int newCost = evaluateSolution(solution, distanceMatrix, costVector);
                if (newCost < currentCost)
                {
                    currentCost = newCost;
                    improved = true;
                    break; // first improvement accepted
                }
                else
                {
                    std::swap(solution[i], solution[j]); // revert
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
        // Greedy construction with random start node (insertion at best position)
        int startNode = startDist(g);
        std::vector<int> solution = constructGreedyInsertion(distanceMatrix, costVector, size, startNode);

        int currentCost = evaluateSolution(solution, distanceMatrix, costVector);

        bool improved = true;
        while (improved)
        {
            improved = false;
            std::vector<std::pair<int, int>> moves;
            moves.reserve((size * (size - 1)) / 2);
            for (int i = 0; i < size - 1; ++i)
                for (int j = i + 1; j < size; ++j)
                    moves.emplace_back(i, j);

            std::shuffle(moves.begin(), moves.end(), g);

            for (const auto &mv : moves)
            {
                int i = mv.first;
                int j = mv.second;
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

        std::cout << "\nRunning M5 on file: " << FILE_NAME << std::endl;
        M5_greedyFirstImprovement_TwoNodeExchange_RandomStart(distanceMatrix, costVector, size);

        std::cout << "\nRunning M6 on file: " << FILE_NAME << std::endl;
        M6_greedyFirstImprovement_TwoNodeExchange_GreedyStart(distanceMatrix, costVector, size);

        for (int i = 0; i < size; i++)
        {
            delete[] distanceMatrix[i];
        }
        delete[] distanceMatrix;
    }

    return 0;
}
