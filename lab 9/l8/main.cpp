#include <iostream>
#include <algorithm>
#include <sstream>
#include <thread>
#include <map>
#include <list>
#include <tuple>
#include <mutex>

#include <boost/process.hpp>

#include "matrix.h"
#include "barrier.h"

namespace bp = boost::process;

using thread_point_t = std::tuple<uint32_t, uint32_t, uint32_t, uint32_t>;

void ThreadCompileProcess();
void ThreadPlayer(
        l8::Matrix &matrix,
        std::list<thread_point_t> &container,
        std::mutex &mutex,
        l8::Barrier &barrier,
        bool &is_working,
        uint32_t id
);
void ThreadReferee(
        l8::Matrix &matrix,
        std::list<thread_point_t> &container,
        l8::Barrier &barrier,
        std::mutex &mutex,
        std::map<uint32_t, int32_t> &table_scores,
        bool &is_working
);



int main() {
    std::thread(ThreadCompileProcess).join();
    std::map<uint32_t, int32_t> table_scores;
    std::vector<std::thread> thread_ids;

    uint32_t num_of_threads = 3;
    bool is_working = true;

    std::list<thread_point_t> container;
    std::mutex mutex;

    l8::Matrix matrix(10, 10);
    l8::Barrier barrier(num_of_threads + 1);


    for (size_t i = 0; i < num_of_threads; i++) {
        table_scores[i] = 0;
        thread_ids.emplace_back(
                ThreadPlayer,
                std::ref(matrix),
                std::ref(container),
                std::ref(mutex),
                std::ref(barrier),
                std::ref(is_working),
                i
        );
    }

    std::thread(
            ThreadReferee,
            std::ref(matrix),
            std::ref(container),
            std::ref(barrier),
            std::ref(mutex),
            std::ref(table_scores),
            std::ref(is_working)
    ).join();

    for (auto &thread : thread_ids) {
        thread.join();
    }

    for (const auto &[id, score] : table_scores) {
        std::cout << "Thread " << id << ": " << score << std::endl;
    }

    return 0;
}


void ThreadCompileProcess(){
    bp::child (
            bp::search_path("clang++"),
            std::vector<std::string>{"modules/player.cpp", "-omodules/player", "-std=c++20"},
            bp::std_out > stdout, bp::std_err > stderr
    ).wait();
    bp::child (
            bp::search_path("clang++"),
            std::vector<std::string>{"modules/referee.cpp", "-omodules/referee", "-std=c++20"},
            bp::std_out > stdout, bp::std_err > stderr
    ).wait();
}


void ThreadPlayer(
        l8::Matrix &matrix,
        std::list<thread_point_t> &container,
        std::mutex &mutex,
        l8::Barrier &barrier,
        bool &is_working,
        uint32_t id
) {
    std::stringstream stream;
    std::string line;

    bp::ipstream istream;
    bp::opstream ostream;

    bp::child player_proc("modules/player", bp::std_out > istream, bp::std_in < ostream, bp::std_err > stderr);

    uint32_t init_n = -1, init_m = -1;

    stream.exceptions(std::ios::failbit);

    while (is_working) {

        auto [n, m] = matrix.get_size();
        if (n != init_n or m != init_m) {
            init_n = n;
            init_m = m;

            ostream << "matrix" << std::endl;
            ostream << n << " " << m << std::endl;

            for (size_t i = 0; i < n; i++) {
                for (size_t j = 0; j < m - 1; j++) {
                    ostream << matrix[i][j] << " ";
                }
                ostream << matrix[i][m - 1] << std::endl;
            }
        }

        ostream << "go" << std::endl;

        try {
            stream.str("");
            stream.clear();

            std::getline(istream, line);
            stream << line;

            uint32_t element, i, j;
            stream >> element >> i >> j;

            {
                std::lock_guard<std::mutex> lg(mutex);
                container.emplace_back(id, element, i, j);
            }

        } catch (std::exception &) {
            {
                std::lock_guard<std::mutex> lg(mutex);
                is_working = false;
            }
            player_proc.terminate();
            std::cerr << "[Thread " << id << "] Conversion error!" << std::endl;
        }

        barrier.wait();
        barrier.wait();
    }

    ostream << "exit" << std::endl;
    player_proc.wait();

}


void ThreadReferee(
        l8::Matrix &matrix,
        std::list<thread_point_t> &container,
        l8::Barrier &barrier,
        std::mutex &mutex,
        std::map<uint32_t, int32_t> &table_scores,
        bool &is_working
) {
    std::stringstream stream;
    std::string line;

    bp::ipstream istream;
    bp::opstream ostream;

    bp::child referee_proc("modules/referee", bp::std_out > istream, bp::std_in < ostream, bp::std_err > stderr);

    uint32_t critical_val_ones = matrix.get_num_of_ones() / 2;

    while (is_working) {
        barrier.wait();

        ostream << "go" << std::endl;

        ostream << container.size() << std::endl;
        for (const auto &[id, val, i, j] : container) {
            ostream << id << " " << val << " " << i << " " << j << std::endl;
        }

        try {
            stream.str("");
            stream.clear();

            std::getline(istream, line);
            stream << line;

            uint32_t n;
            uint32_t id;
            int32_t score;

            stream >> n;

            for (size_t i = 0; i < n; i++) {
                stream.str("");
                stream.clear();

                std::getline(istream, line);
                stream << line;

                stream >> id >> score;
                table_scores[id] += score;

            }

        } catch (std::exception &) {
            {
                std::lock_guard<std::mutex> lg(mutex);
                is_working = false;
            }
            referee_proc.terminate();
            std::cerr << "[Referee] Conversion error!" << std::endl;
        }

        if (!container.empty()) {
            matrix.dec_ones();
            matrix.gen_matrix();
        }


        if (matrix.get_num_of_ones() <= critical_val_ones) {
            std::lock_guard<std::mutex> lg(mutex);
            is_working = false;
        }

        barrier.wait();
    }

    ostream << "exit" << std::endl;
    referee_proc.wait();
}