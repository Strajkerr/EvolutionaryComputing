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
int main()
{
    std::vector<std::string> fileNames = {"../TSPA.csv", "../TSPB.csv"};

    for (const auto &FILE_NAME : fileNames)
    {
        std::vector<std::vector<int>> data;
        if (!getDataFromFile(FILE_NAME, data))
        {
            std::cerr << "Failed to read data from file: " << FILE_NAME << std::endl;
            continue; // move to next file
        }

        int size = data.size();
        int **distanceMatrix = getDistanceMatrix(data, size);
        std::vector<int> costVector = getCostVector(data);

        std::cout << "\nRunning Greedy 2-Regret on file: " << FILE_NAME << std::endl;
        greedy2Regret(distanceMatrix, costVector, size);
        
        std::cout << "\nRunning Greedy Weighted Regret on file: " << FILE_NAME << std::endl;
        std::cout << "Alpha = 0.2\n" << std::endl;
        greedyWeightedRegret(distanceMatrix, costVector, size, 0.2);
        std::cout << "Alpha = 0.5\n" << std::endl;
        greedyWeightedRegret(distanceMatrix, costVector, size, 0.5);
        std::cout << "Alpha = 0.7\n" << std::endl;
        greedyWeightedRegret(distanceMatrix, costVector, size, 0.7);
        for (int i = 0; i < size; i++)
        {
            delete[] distanceMatrix[i];
        }
        delete[] distanceMatrix;
    }

    return 0;
}
