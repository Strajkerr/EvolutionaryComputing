#include <vector>

class DataManager {
public:
    DataManager(const std::vector<std::vector<int>>& inputData);
    int getEuclidanDistance (int x1, int y1, int x2, int y2);

    void getDistanceMatrix (const std::vector<std::vector<int>>& data, int& size);

    void getCostVector (const std::vector<std::vector<int>>& data);
    int evaluateSolution (std::vector<int>& solution);

private:
    int** distanceMatrix;
    std::vector<int> costVector;
};