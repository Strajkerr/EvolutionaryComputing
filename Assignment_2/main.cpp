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
void greedy2Regret(int **distanceMatrix, const std::vector<int> &nodeCostVector, int numberOfNodes, int totalRuns = 200)
{
    if (numberOfNodes <= 0)
        return;
    int nodesToVisit = (numberOfNodes % 2 == 0) ? (numberOfNodes / 2) : ((numberOfNodes + 1) / 2);


    std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<int> startDist(0, numberOfNodes - 1);

    long long totalSum = 0;
    int bestObjective = std::numeric_limits<int>::max();
    int worstObjective = std::numeric_limits<int>::min();
    std::vector<int> bestSolution;

    auto startTime = std::chrono::high_resolution_clock::now();

    for (int run = 0; run < totalRuns; ++run)
    {
        std::vector<int> routeNodes;
        std::vector<char> isNodeUsed(numberOfNodes, 0);

        int startNode = startDist(rng);
        routeNodes.push_back(startNode);
        isNodeUsed[startNode] = 1;

        while ((int)routeNodes.size() < nodesToVisit)
        {
            int bestNode = -1;
            int bestInsertionPos = -1;
            int maxRegret = std::numeric_limits<int>::min();
            int bestCost = std::numeric_limits<int>::max();

            for (int candidateNode = 0; candidateNode < numberOfNodes; ++candidateNode)
            {
                if (isNodeUsed[candidateNode])
                    continue;

                std::vector<std::pair<int, int>> insertionData; // {cost, pos}

                for (size_t pos = 0; pos <= routeNodes.size(); ++pos)
                {
                    int pred = (pos == 0) ? routeNodes.back() : routeNodes[pos - 1];
                    int succ = (pos == routeNodes.size()) ? routeNodes.front() : routeNodes[pos];

                    int added = distanceMatrix[pred][candidateNode] + distanceMatrix[candidateNode][succ];
                    int removed = (pred != succ) ? distanceMatrix[pred][succ] : 0;

                    // include cost of connecting to start if closing the cycle early
                    if (routeNodes.size() == numberOfNodes - 1)
                    {
                        added += distanceMatrix[candidateNode][routeNodes.front()];
                        removed += distanceMatrix[routeNodes.back()][routeNodes.front()];
                    }

                    int cost = nodeCostVector[candidateNode] + (added - removed);
                    insertionData.push_back({cost, (int)pos});
                }

                std::sort(insertionData.begin(), insertionData.end());
                int bestInsertionCost = insertionData[0].first;
                int regret = (insertionData.size() > 1) ? (insertionData[1].first - insertionData[0].first) : insertionData[0].first;

                if (regret > maxRegret || (regret == maxRegret && bestInsertionCost < bestCost))
                {
                    maxRegret = regret;
                    bestCost = bestInsertionCost;
                    bestNode = candidateNode;
                    bestInsertionPos = insertionData[0].second;
                }
            }

            if (bestNode == -1 || bestInsertionPos == -1)
                break;

            routeNodes.insert(routeNodes.begin() + bestInsertionPos, bestNode);
            isNodeUsed[bestNode] = 1;
        }

        // Compute total cost including closing edge
        int totalCost = 0;
        for (size_t i = 0; i < routeNodes.size(); ++i)
        {
            totalCost += nodeCostVector[routeNodes[i]];
            totalCost += distanceMatrix[routeNodes[i]][routeNodes[(i + 1) % routeNodes.size()]]; // closing edge
        }

        totalSum += totalCost;
        if (totalCost < bestObjective)
        {
            bestObjective = totalCost;
            bestSolution = routeNodes;
        }
        if (totalCost > worstObjective)
            worstObjective = totalCost;
    }

    auto endTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsedSeconds = endTime - startTime;
    double averageObjective = static_cast<double>(totalSum) / totalRuns;

    std::cout << "====== Greedy 2-Regret Cycle ======\n";
    std::cout << "  min = " << bestObjective << "\n";
    std::cout << "  max = " << worstObjective << "\n";
    std::cout << "  avg = " << averageObjective << "\n";
    std::cout << "Execution time: " << elapsedSeconds.count() << " seconds\n";

    std::cout << "Best cycle route: ";
    for (int n : bestSolution)
        std::cout << n << " ";
    std::cout << bestSolution.front() << " (back to start)\n";
}

void greedyWeightedRegret(int** distanceMatrix, const std::vector<int>& nodeCostVector, 
                          int numberOfNodes, double alpha = 0.5, int totalRuns = 200) {

    if (numberOfNodes <= 0) return;
    
    int nodesToVisit = (numberOfNodes % 2 == 0) ? (numberOfNodes / 2) : ((numberOfNodes + 1) / 2);

    std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<int> startDist(0, numberOfNodes - 1);

    long long totalSum = 0;
    int bestObjective = std::numeric_limits<int>::max();
    int worstObjective = std::numeric_limits<int>::min();
    std::vector<int> bestSolution;

    auto startTime = std::chrono::high_resolution_clock::now();

    for (int run = 0; run < totalRuns; ++run) {
        std::vector<int> routeNodes;
        std::vector<char> isNodeUsed(numberOfNodes, 0);

        int startNode = startDist(rng);
        routeNodes.push_back(startNode);
        isNodeUsed[startNode] = 1;

        while ((int)routeNodes.size() < nodesToVisit) {
            int bestNode = -1;
            int bestInsertionPos = -1;
            double bestWeightedScore = std::numeric_limits<double>::lowest();

            for (int candidateNode = 0; candidateNode < numberOfNodes; ++candidateNode) {
                if (isNodeUsed[candidateNode]) continue;

                std::vector<std::pair<int, int>> insertionData; // {cost, pos}

                for (size_t pos = 0; pos <= routeNodes.size(); ++pos) {
                    int pred = (pos == 0) ? routeNodes.back() : routeNodes[pos - 1];
                    int succ = (pos == routeNodes.size()) ? routeNodes.front() : routeNodes[pos];

                    int added = distanceMatrix[pred][candidateNode] + distanceMatrix[candidateNode][succ];
                    int removed = (pred != succ) ? distanceMatrix[pred][succ] : 0;

                    int cost = nodeCostVector[candidateNode] + (added - removed);
                    insertionData.push_back({cost, (int)pos});
                }

                std::sort(insertionData.begin(), insertionData.end());
                
                int bestInsertionCost = insertionData[0].first;
                int regret = (insertionData.size() > 1)
                    ? (insertionData[1].first - insertionData[0].first)
                    : bestInsertionCost;

                double weightedScore = alpha * regret - (1.0 - alpha) * bestInsertionCost;

                if (weightedScore > bestWeightedScore) {
                    bestWeightedScore = weightedScore;
                    bestNode = candidateNode;
                    bestInsertionPos = insertionData[0].second;
                }
            }

            if (bestNode == -1 || bestInsertionPos == -1) break;

            routeNodes.insert(routeNodes.begin() + bestInsertionPos, bestNode);
            isNodeUsed[bestNode] = 1;
        }

        int totalCost = 0;
        for (size_t i = 0; i < routeNodes.size(); ++i) {
            totalCost += nodeCostVector[routeNodes[i]];
            totalCost += distanceMatrix[routeNodes[i]][routeNodes[(i + 1) % routeNodes.size()]];
        }

        totalSum += totalCost;
        if (totalCost < bestObjective) {
            bestObjective = totalCost;
            bestSolution = routeNodes;
        }
        if (totalCost > worstObjective)
            worstObjective = totalCost;
    }

    auto endTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsedSeconds = endTime - startTime;

    double averageObjective = static_cast<double>(totalSum) / totalRuns;
    std::cout << "====== Greedy Weighted (2-Regret + Best Change) ======\n";
    std::cout << "  alpha (regret weight) = " << alpha << "\n";
    std::cout << "  min = " << bestObjective << "\n";
    std::cout << "  max = " << worstObjective << "\n";
    std::cout << "  avg = " << averageObjective << "\n";
    std::cout << "Execution time: " << elapsedSeconds.count() << " seconds\n\n";

    std::cout << "Best cycle route: ";
    for (const auto& n : bestSolution) std::cout << n << " ";
    std::cout << bestSolution.front() << " (back to start)\n";
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
