#include "dataManager.h"

#include <cmath>

DataManager::DataManager(const std::vector<std::vector<int>>& inputData) {
    int size = inputData.size();
    this->distanceMatrix = new int*[size];
    getDistanceMatrix(inputData, size);
    getCostVector(inputData);
}

int DataManager::getEuclidanDistance (int x1, int y1, int x2, int y2) {
        return static_cast<int>(std::sqrt(std::pow(x2 - x1, 2) + std::pow(y2 - y1, 2)));
    }

void DataManager::getDistanceMatrix (const std::vector<std::vector<int>>& data, int& size) {
    for (int16_t i  = 0; i < size; i++) {
        this->distanceMatrix[i] = new int[size];
        for (int16_t j = 0; j < size; j++) {
            if (i == j)
                this->distanceMatrix[i][j] = 0;
            else
                this->distanceMatrix[i][j] = getEuclidanDistance(data[i][0], data[i][1], data[j][0], data[j][1]);
        }
    }
}

void DataManager::getCostVector (const std::vector<std::vector<int>>& data) {
    for (const auto& row : data) {
        this->costVector.push_back(row[2]);
    }
}

int DataManager::evaluateSolution (std::vector<int>& solution) {
    int totalCost = 0;
    for (size_t i = 0; i < solution.size(); ++i) {
        totalCost += costVector[solution[i]];
        if (i > 0) {
            totalCost += distanceMatrix[solution[i - 1]][solution[i]];
        }
    }
    return totalCost;
}