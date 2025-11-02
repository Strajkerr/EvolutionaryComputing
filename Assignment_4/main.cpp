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
#include <numeric>

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

std::vector<int> randomPermutation(int size, std::mt19937 &g)
{
    int nodesToVisit = (size % 2 == 0) ? (size / 2) : ((size + 1) / 2);

    std::vector<int> solution(size);
    std::iota(solution.begin(), solution.end(), 0);
    std::shuffle(solution.begin(), solution.end(), g);
    solution.resize(nodesToVisit);
    return solution;
}

/**
 * @brief Creates a candidate list for local search.
 * For each vertex u, finds the K nearest vertices v based on the
 * metric: distanceMatrix[u][v] + costVector[v].
 *
 * @param distanceMatrix The n x n distance matrix.
 * @param costVector The cost vector of size n.
 * @param size The number of vertices (n).
 * @param K The number of nearest neighbors to store.
 * @return std::vector<std::vector<int>> A list where candidateList[u]
 * contains the K nearest neighbors of u.
 */
std::vector<std::vector<int>> createCandidateList(int **distanceMatrix, const std::vector<int> &costVector, int size, int K = 10)
{
    std::vector<std::vector<int>> candidateList(size);

    for (int u = 0; u < size; ++u)
    {
        std::vector<std::pair<int, int>> neighbors;
        neighbors.reserve(size - 1);

        for (int v = 0; v < size; ++v)
        {
            if (u == v) continue;
            // Metric: distance to v + cost of visiting v
            int metric = distanceMatrix[u][v] + costVector[v];
            neighbors.push_back({metric, v});
        }

        // Sort neighbors by the metric (lowest first)
        std::sort(neighbors.begin(), neighbors.end());

        // Add the K best neighbors to the candidate list for u
        for (int i = 0; i < std::min((int)neighbors.size(), K); ++i)
        {
            candidateList[u].push_back(neighbors[i].second);
        }
    }
    return candidateList;
}


void reverseCircularSegment(std::vector<int> &solution, int pos1, int pos2)
{
    int n = static_cast<int>(solution.size());
    if (n == 0) return;
    
    pos1 = (pos1 % n + n) % n;
    pos2 = (pos2 % n + n) % n;

    int halfLen = 0;
    if (pos1 <= pos2) {
        halfLen = (pos2 - pos1 + 1) / 2;
    } else {
        halfLen = (n - pos1 + pos2 + 1) / 2;
    }

    for (int i = 0; i < halfLen; ++i)
    {
        std::swap(solution[pos1], solution[pos2]);
        pos1 = (pos1 + 1) % n;
        pos2 = (pos2 - 1 + n) % n;
    }
}

// Helper macros for readability in delta calculations
#define dist(u, v) distanceMatrix[u][v]
#define cost(n) costVector[n]

void M_Steepest_CandidateList_RandomStart(
    int **distanceMatrix,
    std::vector<int> &costVector,
    const std::vector<std::vector<int>>& candidateList,
    int size,
    int totalRuns = 200)
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
        std::vector<int> solution = randomPermutation(size, g);
        int solSize = static_cast<int>(solution.size());
        if (solSize <= 1) continue;

        int currentCost = evaluateSolution(solution, distanceMatrix, costVector);

        bool improved = true;
        while (improved)
        {
            improved = false;
            int bestDelta = 0;

            // --- Best move storage ---
            int bestMoveType = 0; // 0=none, 1/2=intra, 3/4=inter
            // For intra-route
            int best_u_pos = -1, best_v_pos = -1;
            // For inter-route
            int best_replace_pos = -1, best_newNode = -1;

            std::vector<char> used(size, 0);
            std::vector<int> sol_pos(size, -1);
            for (int i = 0; i < solSize; ++i) {
                used[solution[i]] = 1;
                sol_pos[solution[i]] = i;
            }

            for (int u_pos = 0; u_pos < solSize; ++u_pos)
            {
                int u = solution[u_pos];

                for (int v : candidateList[u])
                {
                    if (u == v) continue;

                    if (used[v])
                    {
                        int v_pos = sol_pos[v];

                        if ((u_pos + 1) % solSize == v_pos || (v_pos + 1) % solSize == u_pos) continue;

                        int u_prev_pos = (u_pos - 1 + solSize) % solSize;
                        int u_next_pos = (u_pos + 1) % solSize;
                        int v_prev_pos = (v_pos - 1 + solSize) % solSize;
                        int v_next_pos = (v_pos + 1) % solSize;

                        if (u_prev_pos != v_pos && v_prev_pos != u_pos)
                        {
                            int u_prev = solution[u_prev_pos];
                            int v_prev = solution[v_prev_pos];
                            int delta = (dist(u_prev, v_prev) + dist(u, v)) - (dist(u_prev, u) + dist(v_prev, v));
                            if (delta < bestDelta) {
                                bestDelta = delta;
                                bestMoveType = 1;
                                best_u_pos = u_pos;
                                best_v_pos = v_prev_pos;
                            }
                        }


                        if (u_next_pos != v_pos && v_next_pos != u_pos)
                        {
                            int u_next = solution[u_next_pos];
                            int v_next = solution[v_next_pos];
                            int delta = (dist(u, v) + dist(u_next, v_next)) - (dist(u, u_next) + dist(v, v_next));
                            if (delta < bestDelta) {
                                bestDelta = delta;
                                bestMoveType = 2;
                                best_u_pos = u_next_pos;
                                best_v_pos = v_pos;
                            }
                        }
                    }
                    else
                    {
                        if (solSize < 3) continue;

                        int u_prev_pos = (u_pos - 1 + solSize) % solSize;
                        int u_next_pos = (u_pos + 1) % solSize;
                        
                        // Move B1: Replace u's *successor* (u_next) with v. Creates (u, v).
                        int w_pos = u_next_pos;
                        int w = solution[w_pos];
                        int w_next_pos = (w_pos + 1) % solSize;
                        int w_next = solution[w_next_pos];
                        int delta = (dist(u, v) + dist(v, w_next) + cost(v)) - (dist(u, w) + dist(w, w_next) + cost(w));
                        if (delta < bestDelta) {
                            bestDelta = delta;
                            bestMoveType = 3;
                            best_replace_pos = w_pos;
                            best_newNode = v;
                        }

                        // Move B2: Replace u's *predecessor* (u_prev) with v. Creates (v, u).
                        w_pos = u_prev_pos;
                        w = solution[w_pos];
                        int w_prev_pos = (w_pos - 1 + solSize) % solSize;
                        int w_prev = solution[w_prev_pos];
                        delta = (dist(w_prev, v) + dist(v, u) + cost(v)) - (dist(w_prev, w) + dist(w, u) + cost(w));
                        if (delta < bestDelta) {
                            bestDelta = delta;
                            bestMoveType = 4;
                            best_replace_pos = w_pos;
                            best_newNode = v;
                        }
                    }
                } // end for v
            } // end for u_pos

            // --- Apply the single best move found ---
            if (bestDelta < 0)
            {
                improved = true;
                currentCost += bestDelta;

                if (bestMoveType == 1) // Intra-route A1: reverse [u_pos...v_prev_pos]
                {
                    reverseCircularSegment(solution, best_u_pos, best_v_pos);
                }
                else if (bestMoveType == 2) // Intra-route A2: reverse [u_next_pos...v_pos]
                {
                    reverseCircularSegment(solution, best_u_pos, best_v_pos);
                }
                else if (bestMoveType == 3 || bestMoveType == 4) // Inter-route B1/B2
                {
                    solution[best_replace_pos] = best_newNode;
                }
            }
        } // end while(improved)

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
    std::cout << "====== M_Steepest (Candidate List, 2-opt+Exchange, random start) ======\n";
    std::cout << "  K (neighbors) = " << candidateList[0].size() << "\n";
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
#undef dist
#undef cost

int main()
{
    std::vector<std::string> fileNames = {"../TSPA.csv", "../TSPB.csv"};
    const int K_NEIGHBORS = 10; // The 'K' parameter

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

        std::cout << "\nBuilding candidate list for: " << FILE_NAME << " (K=" << K_NEIGHBORS << ")\n";
        auto candidateList = createCandidateList(distanceMatrix, costVector, size, K_NEIGHBORS);

        std::cout << "\nRunning M_Steepest_CandidateList on file: " << FILE_NAME << std::endl;
        M_Steepest_CandidateList_RandomStart(distanceMatrix, costVector, candidateList, size);

        for (int i = 0; i < size; i++)
        {
            delete[] distanceMatrix[i];
        }
        delete[] distanceMatrix;
    }

    return 0;
}
