#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <cmath>
#include <vector>
#include <algorithm>


bool getDataFromFile(const std::string& filename, std::vector<std::vector<int>>& data) {
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

std::vector<std::vector<int>> computeDistanceMatrix(const std::vector<std::vector<int>>& data) {
    int n = data.size();
    std::vector<std::vector<int>> dist(n, std::vector<int>(n, 0));

    for (int i = 0; i < n; ++i) {
        for (int j = i + 1; j < n; ++j) {
            double dx = data[i][0] - data[j][0];
            double dy = data[i][1] - data[j][1];
            int d = static_cast<int>(std::round(std::sqrt(dx * dx + dy * dy)));
            dist[i][j] = dist[j][i] = d;
        }
    }

    return dist;
}

int main() {
    const std::string FILE_NAME = "../TSPA.csv";
    std::vector<std::vector<int>> data;
    if (!getDataFromFile(FILE_NAME, data)) {
        std::cerr << "Failed to read data from file." << std::endl;
        return 1;
    }

    
    return 0;
}
