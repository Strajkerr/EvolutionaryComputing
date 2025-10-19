#include <iostream>
#include <fstream>
#include <string>
#include <vector>

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

class FileReader {
public:
    FileReader(const std::string& filename) : filename(filename) {}

    bool getDataFromFile (std::vector<std::vector<int>>& data);

private:
    std::string filename;
};