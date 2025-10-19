#include <vector>

class DataManager {
public:
    DataManager(const std::vector<std::vector<int>>& inputData);
    ~DataManager();
    int getEuclidanDistance (int x1, int y1, int x2, int y2);
    void setDistanceMatrix (const std::vector<std::vector<int>>& data, int& size);
    void setCostVector (const std::vector<std::vector<int>>& data);
    int evaluateSolution (std::vector<int>& solution);
    int** getDistanceMatrix() const { return distanceMatrix; }
    const std::vector<int>& getCostVector() const { return costVector; }

private:
    int** distanceMatrix;
    std::vector<int> costVector;
};