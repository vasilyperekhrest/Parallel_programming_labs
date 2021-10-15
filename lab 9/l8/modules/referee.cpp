#include <iostream>
#include <vector>
#include <sstream>
#include <list>
#include <tuple>
#include <string>
#include <map>

using thread_data_t = std::tuple<uint32_t, uint32_t, uint32_t, uint32_t>;

std::map<std::string, uint32_t> GetCmdTable();
std::list<thread_data_t> GetScores();
std::map<uint32_t, int32_t> ProcessData(const std::list<thread_data_t> &data);


int main() {
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
                auto data = GetScores();
                auto table_scores = ProcessData(data);

                std::cout << table_scores.size() << std::endl;

                for (const auto &[id, value] : table_scores) {
                    std::cout << id << " " << value << std::endl;
                }
                break;

        }

    }

}


std::map<std::string, uint32_t> GetCmdTable() {
    std::map<std::string, uint32_t> table{
            {"exit", 0},
            {"go", 1}
    };
    return table;
}


std::list<thread_data_t> GetScores() {
    std::list<thread_data_t> table;
    uint32_t n;

    std::stringstream stream;
    std::string data;
    stream.exceptions(std::ios::failbit);

    try {
        // get size
        std::getline(std::cin, data);
        stream << data;

        stream >> n;

        uint32_t id, point, pnt_i, pnt_j;

        for (size_t i = 0; i < n; i++) {
            stream.str("");
            stream.clear();

            std::getline(std::cin, data);
            stream << data;

            stream >> id >> point >> pnt_i >> pnt_j;
            table.emplace_back(id, point, pnt_i, pnt_j);
        }


    } catch (std::exception &) {
        std::cerr << "[referee] Conversion error!" << std::endl;
    }

    return table;
}


std::map<uint32_t, int32_t> ProcessData(const std::list<thread_data_t> &data) {
    using point_t = std::tuple<uint32_t, uint32_t, uint32_t>;

    std::map<point_t, std::vector<uint32_t>> points_table;
    std::map<uint32_t, int32_t> table_scores;

    for (const auto &[id, value, i, j] : data) {
        if (value == 1) {
            points_table[std::tuple(value, i, j)].push_back(id);
        }
    }

    for (const auto &[point, ids] : points_table) {
        if (ids.size() > 1) {
            for (const auto &id : ids) {
                table_scores[id] -= 1;
            }
        } else {
            table_scores[ids[0]] += 1;
        }
    }

    return table_scores;
}
