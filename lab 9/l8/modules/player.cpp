#include <iostream>
#include <vector>
#include <sstream>
#include <tuple>
#include <random>
#include <string>
#include <map>


std::map<std::string, uint32_t> GetCmdTable();
std::vector<std::vector<uint16_t>> InputMatrix();
std::tuple<uint16_t, uint32_t, uint32_t> GetRandPoint(const std::vector<std::vector<uint16_t>> &matrix);

int main() {
    std::vector<std::vector<uint16_t>> matrix;
    std::string command;
    int32_t key;

    auto cmd_table = GetCmdTable();


    while (true) {
        std::getline(std::cin, command);

        try {
            key = cmd_table.at(command);
        } catch (std::out_of_range &) {
            key = -1;
        }

        switch (key) {
            case 0:
                return 0;

            case 1:
                matrix = InputMatrix();
                break;

            case 2:
                auto [element, i, j] = GetRandPoint(matrix);
                std::cout << element << " " << i << " " << j << std::endl;
                break;
        }

    }

}


std::map<std::string, uint32_t> GetCmdTable() {
    std::map<std::string, uint32_t> table{
            {"exit", 0},
            {"matrix", 1},
            {"go", 2}
    };
    return table;
}


std::vector<std::vector<uint16_t>> InputMatrix() {
    std::vector<std::vector<uint16_t>> matrix;
    uint32_t n, m;

    std::stringstream stream;
    std::string data;
    stream.exceptions(std::ios::failbit);

    try {
        // get size
        std::getline(std::cin, data);
        stream << data;

        stream >> n >> m;

        for (size_t i = 0; i < n; i++) {
            stream.str("");
            stream.clear();

            std::getline(std::cin, data);
            stream << data;

            std::vector<uint16_t> vector;
            uint16_t element;

            vector.reserve(m);

            while (!stream.eof()) {
                stream >> element;
                vector.push_back(element);
            }

            matrix.push_back(vector);
        }


    } catch (std::exception &) {
        std::cerr << "[player] Conversion error!" << std::endl;
    }

    return matrix;
}


std::tuple<uint16_t, uint32_t, uint32_t> GetRandPoint(const std::vector<std::vector<uint16_t>> &matrix) {
    std::random_device rd;
    std::mt19937 mersenne(rd());

    auto n = matrix.size();
    auto m = matrix[0].size();

    uint32_t i = mersenne() % n;
    uint32_t j = mersenne() % m;

    return std::tuple(matrix[i][j], i, j);
}

