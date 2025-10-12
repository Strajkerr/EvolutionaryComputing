#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <cmath>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <random>
#include <climits>
#include <limits>
#include <cstdint>


bool getDataFromFile (const std::string& filename, std::vector<std::vector<int>>& data) {
    std::ifstream file("../TSPA.csv");
    if (!file.is_open()) {
        std::cerr << "Error: Could not open the file!" << std::endl;
        return false;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string value;
        std::vector<int> row;

        while (std::getline(ss, value, ';')) {
            try {
                row.push_back(std::stoi(value));
            } catch (const std::invalid_argument& e) {
                std::cerr << "Invalid data: " << value << std::endl;
                return false;
            }
        }

        data.push_back(row);
    }

    file.close();
    return true;
}

int getEuclidanDistance (int x1, int y1, int x2, int y2) {
    return static_cast<int>(std::sqrt(std::pow(x2 - x1, 2) + std::pow(y2 - y1, 2)));
}

int** getDistanceMatrix (std::vector<std::vector<int>>& data, int& size) {
    int** distanceMatrix = new int*[size];
    for (int16_t i  = 0; i < size; i++) {
        distanceMatrix[i] = new int[size];
        for (int16_t j = 0; j < size; j++) {
            if (i == j)
                distanceMatrix[i][j] = 0;
            else
                distanceMatrix[i][j] = getEuclidanDistance(data[i][0], data[i][1], data[j][0], data[j][1]);
        }
    }

    return distanceMatrix;
}

std::vector<int> getCostVector (std::vector<std::vector<int>>& data) {
    std::vector<int> nodeCosts;
    for (const auto& row : data) {
        nodeCosts.push_back(row[2]);
    }

    data.clear();
    return nodeCosts;
}

int evaluateSolution (std::vector<int>& solution, int** distanceMatrix, std::vector<int>& costVector) {
    int totalCost = 0;
    for (size_t i = 0; i < solution.size(); ++i) {
        totalCost += costVector[solution[i]];
        if (i > 0) {
            totalCost += distanceMatrix[solution[i - 1]][solution[i]];
        }
    }
    return totalCost;
}

// Random solution algorithm
void randomSolution (int** distanceMatrix, std::vector<int>& costVector, int& dataSize) {
    srand(time(NULL));
    if (dataSize % 2 != 0){
        dataSize++;
    }
    int numberOfNodesToVisit = dataSize / 2;
    std::vector<int> bestSolution;
    int bestSolutionScore = INT_MAX;
    int worstSolutionScore = INT_MIN;
    float avgScore = 0.0f;
    for (int i = 0; i < 200; i++) {
        std::vector<int> currentSolution;
        while (currentSolution.size() < numberOfNodesToVisit) {
            int randomNode = rand() % dataSize;
            if (std::find(currentSolution.begin(), currentSolution.end(), randomNode) == currentSolution.end()) {
                currentSolution.push_back(randomNode);
            }
        }
        int currentScore = evaluateSolution(currentSolution, distanceMatrix, costVector);
        if (currentScore < bestSolutionScore) {
            bestSolution = currentSolution;
            bestSolutionScore = evaluateSolution(currentSolution, distanceMatrix, costVector);
        }
        
        if (currentScore > worstSolutionScore) {
            worstSolutionScore = currentScore;
        }

        avgScore += currentScore;
    }

    avgScore /= 200;

    std::cout << "====== Random solution ======" << std::endl;
    std::cout << "Best solution score: " << bestSolutionScore << " | Worst solution score: " << worstSolutionScore << " | Average score: " << avgScore << std::endl;
    std::cout << "Best solution: ";
    for (const auto& node : bestSolution) {
        std::cout << node << " ";
    }
    std::cout << std::endl;
}

// Nearest Neighbour algorithm (only adding at the end)
int getBestNearestNeighbour (int currentNode, std::vector<int>& unvisitedNodes, int** distanceMatrix, std::vector<int>& costVector) {
    int bestNode = -1;
    int bestScore = INT_MAX;
    for (const auto& node : unvisitedNodes) {
        if (distanceMatrix[currentNode][node] + costVector[node] < bestScore) {
            bestScore = distanceMatrix[currentNode][node] + costVector[node];
            bestNode = node;
        }
    }

    return bestNode;
}

void nearestNeighbourSolutionOnlyAtEnd (int** distanceMatrix, std::vector<int>& costVector, int& dataSize) {
    srand(time(NULL));
    if (dataSize % 2 != 0){
        dataSize++;
    }
    int numberOfNodesToVisit = dataSize / 2;
    std::vector<int> bestSolution;
    int bestSolutionScore = INT_MAX;
    int worstSolutionScore = INT_MIN;
    float avgScore = 0.0f;
    for (int i = 0; i < 200; i++) {
        std::vector<int> currentSolution;
        int startingNode = rand() % dataSize;
        currentSolution.push_back(startingNode);

        std::vector<int> unvisitedNodes;
        for (int j = 0; j < dataSize; j++) {
            if (j != startingNode) {
                unvisitedNodes.push_back(j);
            }
        }

        while (currentSolution.size() < numberOfNodesToVisit) {
            int nextNode = getBestNearestNeighbour(currentSolution.back(), unvisitedNodes, distanceMatrix, costVector);
            if (nextNode != -1) {
                currentSolution.push_back(nextNode);
                unvisitedNodes.erase(std::remove(unvisitedNodes.begin(), unvisitedNodes.end(), nextNode), unvisitedNodes.end());
            } else {
                break;
            }
        }

        int currentScore = evaluateSolution(currentSolution, distanceMatrix, costVector);
        if (currentScore < bestSolutionScore) {
            bestSolution = currentSolution;
            bestSolutionScore = currentScore;
        }
        
        if (currentScore > worstSolutionScore) {
            worstSolutionScore = currentScore;
        }

        avgScore += currentScore;
    }

    avgScore /= 200;

    std::cout << "====== Nearest Neighbour solution only adding at the end ======" << std::endl;
    std::cout << "Best solution score: " << bestSolutionScore << " | Worst solution score: " << worstSolutionScore << " | Average score: " << avgScore << std::endl;
    std::cout << "Best solution: ";
    for (const auto& node : bestSolution) {
        std::cout << node << " ";
    }
    std::cout << std::endl;
}

void nearestNeighbourSolution(int** distanceMatrix, std::vector<int>& nodeCostVector, int numberOfNodes, int numberOfSolutionsPerStart = 200) {
    if (numberOfNodes <= 0) return;

    int nodesToVisit = (numberOfNodes % 2 == 0) ? (numberOfNodes / 2) : ((numberOfNodes + 1) / 2);

    std::mt19937 rng(std::random_device{}());

    long long totalSum = 0;
    int bestObjective = std::numeric_limits<int>::max();
    int worstObjective = std::numeric_limits<int>::min();
    std::vector<int> bestSolution;

    for (int startNode = 0; startNode < numberOfNodes; ++startNode) {
        for (int run = 0; run < numberOfSolutionsPerStart; ++run) {
            std::vector<int> routeNodes;
            routeNodes.reserve(nodesToVisit);
            std::vector<char> isNodeUsed(numberOfNodes, 0);

            routeNodes.push_back(startNode);
            isNodeUsed[startNode] = 1;

            while ((int)routeNodes.size() < nodesToVisit) {
                int bestObjectiveDelta = std::numeric_limits<int>::max();
                std::vector<int> candidates;
                int bestInsertionPosition = 0;

                for (int candidateNode = 0; candidateNode < numberOfNodes; ++candidateNode) {
                    if (isNodeUsed[candidateNode]) continue;

                    for (size_t insertionPosition = 0; insertionPosition <= routeNodes.size(); ++insertionPosition) {
                        int predecessorNode = (insertionPosition == 0) ? -1 : routeNodes[insertionPosition - 1];
                        int successorNode = (insertionPosition == routeNodes.size()) ? -1 : routeNodes[insertionPosition];

                        int addedDistance = 0;
                        if (predecessorNode != -1) addedDistance += distanceMatrix[predecessorNode][candidateNode];
                        if (successorNode != -1) addedDistance += distanceMatrix[candidateNode][successorNode];

                        int removedDistance = 0;
                        if (predecessorNode != -1 && successorNode != -1) removedDistance = distanceMatrix[predecessorNode][successorNode];

                        int objectiveDelta = nodeCostVector[candidateNode] + (addedDistance - removedDistance);

                        if (objectiveDelta < bestObjectiveDelta) {
                            bestObjectiveDelta = objectiveDelta;
                            candidates.clear();
                            candidates.push_back(static_cast<int>(insertionPosition) << 24 | candidateNode);
                        } else if (objectiveDelta == bestObjectiveDelta) {
                            candidates.push_back(static_cast<int>(insertionPosition) << 24 | candidateNode);
                        }
                    }
                }

                if (candidates.empty()) break;
                std::uniform_int_distribution<int> pick(0, (int)candidates.size() - 1);
                int chosen = candidates[pick(rng)];
                int chosenInsertion = (chosen >> 24) & 0xFF;
                int chosenCandidate = chosen & 0xFFFFFF;

                routeNodes.insert(routeNodes.begin() + chosenInsertion, chosenCandidate);
                isNodeUsed[chosenCandidate] = 1;
            }

            int objectiveValue = evaluateSolution(routeNodes, distanceMatrix, nodeCostVector);
            totalSum += objectiveValue;
            if (objectiveValue < bestObjective) {
                bestObjective = objectiveValue;
                bestSolution = routeNodes;
            }
            if (objectiveValue > worstObjective) worstObjective = objectiveValue;
        }
    }

    double averageObjective = static_cast<double>(totalSum) / (numberOfNodes * numberOfSolutionsPerStart);
    std::cout << "====== Nearest neighbor (insert anywhere) ======\n";
    std::cout << "  min = " << bestObjective << "\n";
    std::cout << "  max = " << worstObjective << "\n";
    std::cout << "  avg = " << averageObjective << "\n";

    std::cout << "Best solution score: " << bestObjective << std::endl;
    std::cout << "Best solution: ";
    for (const auto& n : bestSolution) std::cout << n << " ";
    std::cout << std::endl;
}

void fullyGreedySolution(int** distanceMatrix, std::vector<int>& nodeCostVector, int numberOfNodes, int numberOfSolutionsPerStart = 200) {
    if (numberOfNodes <= 0) return;

    int nodesToVisit = (numberOfNodes % 2 == 0) ? (numberOfNodes / 2) : ((numberOfNodes + 1) / 2);

    std::mt19937 rng(std::random_device{}());

    std::vector<int> bestSolution;
    int bestScore = std::numeric_limits<int>::max();

    long long totalSum = 0;
    int bestObjective = std::numeric_limits<int>::max();
    int worstObjective = std::numeric_limits<int>::min();

    for (int startNode = 0; startNode < numberOfNodes; ++startNode) {
        for (int run = 0; run < numberOfSolutionsPerStart; ++run) {
            std::vector<int> routeNodes;
            routeNodes.reserve(nodesToVisit);
            std::vector<char> isUsed(numberOfNodes, 0);

            routeNodes.push_back(startNode);
            isUsed[startNode] = 1;

            while ((int)routeNodes.size() < nodesToVisit) {
                int bestDelta = std::numeric_limits<int>::max();
                std::vector<int> candidates;
                int bestInsertPos = 0;

                for (int candidate = 0; candidate < numberOfNodes; ++candidate) {
                    if (isUsed[candidate]) continue;

                    for (size_t insertPos = 0; insertPos <= routeNodes.size(); ++insertPos) {
                        int pred = (insertPos == 0) ? -1 : routeNodes[insertPos - 1];
                        int succ = (insertPos == routeNodes.size()) ? -1 : routeNodes[insertPos];

                        int added = 0;
                        if (pred != -1) added += distanceMatrix[pred][candidate];
                        if (succ != -1) added += distanceMatrix[candidate][succ];

                        int removed = 0;
                        if (pred != -1 && succ != -1) removed = distanceMatrix[pred][succ];

                        int delta = nodeCostVector[candidate] + (added - removed);

                        if (delta < bestDelta) {
                            bestDelta = delta;
                            candidates.clear();
                            candidates.push_back(static_cast<int>(insertPos) << 24 | candidate);
                        } else if (delta == bestDelta) {
                            candidates.push_back(static_cast<int>(insertPos) << 24 | candidate);
                        }
                    }
                }

                if (candidates.empty()) break;
                std::uniform_int_distribution<int> pick(0, (int)candidates.size() - 1);
                int chosen = candidates[pick(rng)];
                int chosenInsert = (chosen >> 24) & 0xFF;
                int chosenCandidate = chosen & 0xFFFFFF;

                routeNodes.insert(routeNodes.begin() + chosenInsert, chosenCandidate);
                isUsed[chosenCandidate] = 1;
            }

            int score = evaluateSolution(routeNodes, distanceMatrix, nodeCostVector);
            totalSum += score;
            if (score < bestObjective) bestObjective = score;
            if (score > worstObjective) worstObjective = score;

            if (score < bestScore) {
                bestScore = score;
                bestSolution = std::move(routeNodes);
            }
        }
    }

    double average = static_cast<double>(totalSum) / (numberOfNodes * numberOfSolutionsPerStart);

    std::cout << "====== Greedy cycle ======" << std::endl;
    std::cout << "  min = " << bestObjective << std::endl;
    std::cout << "  max = " << worstObjective << std::endl;
    std::cout << "  avg = " << average << std::endl;

    std::cout << "Best solution score: " << bestScore << std::endl;
    std::cout << "Best solution: ";
    for (const auto& n : bestSolution) std::cout << n << " ";
    std::cout << std::endl;
}
int main() {
    const std::string FILE_NAME = "../TSPA.csv";
    std::vector<std::vector<int>> data;
    if (!getDataFromFile(FILE_NAME, data)) {
        std::cerr << "Failed to read data from file." << std::endl;
        return 1;
    }

    int size = data.size();
    int** distanceMatrix = getDistanceMatrix(data, size);
    std::vector<int> costVector = getCostVector(data);

    // Running all algorithms
    randomSolution(distanceMatrix, costVector, size);
    std::cout << std::endl;
    // Nearest Neighbour algorithm (only adding at the end)
    nearestNeighbourSolutionOnlyAtEnd(distanceMatrix, costVector, size);
    std::cout << std::endl;

    // Nearest Neighbour algorithm (adding anywhere)
    nearestNeighbourSolution(distanceMatrix, costVector, size);
    std::cout << std::endl;
    // Fully Greedy algorithm
    fullyGreedySolution(distanceMatrix, costVector, size);      
    std::cout << std::endl;


    for (int i = 0; i < size; i++) {
        delete[] distanceMatrix[i];
    }
    delete[] distanceMatrix;

    return 0;
}
