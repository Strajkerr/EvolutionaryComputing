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

    for (int i = 0; i < size; i++) {
        delete[] distanceMatrix[i];
    }
    delete[] distanceMatrix;

    return 0;
}
