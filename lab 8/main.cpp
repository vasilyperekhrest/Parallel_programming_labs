#include <iostream>
#include <random>
#include <thread>
#include <map>
#include <queue>

#include "matrix.h"
#include "barrier.h"

using point_from_thread_t = std::tuple<uint32_t, uint16_t, uint32_t, uint32_t>;

[[noreturn]] void ThreadPlayer(l8::Matrix &matrix,
                               std::queue<point_from_thread_t> &data_stream,
                               l8::Barrier &barrier,
                               uint32_t id);

void ThreadReferee(l8::Matrix &matrix,
                   std::queue<point_from_thread_t> &data_stream,
                   l8::Barrier &barrier,
                   std::map<uint32_t, uint32_t> &results_table);


int main() {
    uint32_t num_of_threads = 3;

    std::map<uint32_t, uint32_t> results_table;
    std::queue<point_from_thread_t> data_stream;
    l8::Matrix matrix;
    l8::Barrier barrier(num_of_threads + 1);

    for (uint32_t i = 0; i < num_of_threads; i++) {
        std::thread(ThreadPlayer,
                    std::ref(matrix),
                    std::ref(data_stream),
                    std::ref(barrier),
                    i).detach();

        results_table.emplace(i, 0);
    }

    std::thread(ThreadReferee,
                std::ref(matrix),
                std::ref(data_stream),
                std::ref(barrier),
                std::ref(results_table)).join();


    for (auto [id, val] : results_table) {
        std::cout << id << " " << val << std::endl;
    }


    return 0;
}


void ThreadPlayer(l8::Matrix &matrix,
                  std::queue<point_from_thread_t> &data_stream,
                  l8::Barrier &barrier,
                  uint32_t id) {
    std::random_device rd;
    std::mt19937 mersenne(rd());

    while (true) {
        auto [n, m] = matrix.get_size();

        uint32_t i = mersenne() % n;
        uint32_t j = mersenne() % m;

        uint16_t value = matrix[i][j];

        data_stream.emplace(id, value, i, j);

        barrier.wait();
        barrier.wait();
    }
}


void ThreadReferee(l8::Matrix &matrix,
                   std::queue<point_from_thread_t> &data_stream,
                   l8::Barrier &barrier,
                   std::map<uint32_t, uint32_t> &results_table) {
    using point_t = std::tuple<uint16_t, uint32_t, uint32_t>;

    uint32_t critical_val_ones = matrix.get_num_of_ones() / 2;

    while (true) {
        barrier.wait();

        std::map<point_t, std::vector<uint32_t>> points_table;

        while (!data_stream.empty()) {
            auto [id, val, i, j] = data_stream.front();
            data_stream.pop();

            if (val == 1) {
                points_table[std::make_tuple(val, i, j)].push_back(id);
            }
        }

        for (auto [point, ids] : points_table) {

            if (ids.size() > 1) {
                for (auto id : ids) {
                    results_table[id] -= 1;
                }

            } else {
                results_table[ids[0]] += 1;
            }
        }

        if (matrix.get_num_of_ones() <= critical_val_ones) {
            return;
        }

        matrix.dec_ones();
        matrix.gen_matrix();

        barrier.wait();
    }
}