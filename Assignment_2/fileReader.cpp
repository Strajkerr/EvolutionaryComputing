#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "fileReader.h"


FileReader::FileReader(const std::string& filename) : filename(filename) {}

bool FileReader::getDataFromFile (std::vector<std::vector<int>>& data) {
    std::ifstream file(this->filename);
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