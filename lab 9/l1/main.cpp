#include <iostream>
#include <list>
#include <vector>
#include <random>
#include <thread>
#include <mutex>

#include <boost/process.hpp>

namespace bp = boost::process;


std::vector<int32_t> GenArray(size_t size);
void ThreadRunProcessGCD(std::list<std::tuple<int32_t, int32_t, int32_t>> &container,
                         const std::vector<int32_t> &array_one,
                         const std::vector<int32_t> &array_two,
                         std::mutex &mutex,
                         uint32_t begin_id,
                         uint32_t end_id);


int main() {
    bp::child compile_proc(bp::search_path("clang++"),
                           std::vector<std::string>{"modules/array_gcd.cpp", "-omodules/array_gcd", "-std=c++20"},
                           bp::std_out > stdout, bp::std_err > stderr);
    compile_proc.wait();


    std::list<std::tuple<int32_t, int32_t, int32_t>> container;
    std::mutex mutex;
    std::vector<std::thread> threads_proc_id;
    size_t num_of_processes = 3;
    size_t array_size = 20;

    auto array_one = GenArray(array_size);
    auto array_two = GenArray(array_size);


    uint32_t step = array_size / num_of_processes;
    uint32_t add = array_size % num_of_processes;

    for (size_t i = 0; i < num_of_processes - 1; i++) {
        threads_proc_id.emplace_back(ThreadRunProcessGCD,
                                     std::ref(container),
                                     std::cref(array_one),
                                     std::cref(array_two),
                                     std::ref(mutex),
                                     step * i,
                                     step * i + step);
    }

    threads_proc_id.emplace_back(ThreadRunProcessGCD,
                                 std::ref(container),
                                 std::cref(array_one),
                                 std::cref(array_two),
                                 std::ref(mutex),
                                 array_size - step - add,
                                 array_size);


    for (auto &thread : threads_proc_id) {
        thread.join();
    }

    for (auto [gcd, a, b] : container) {
        std::cout << "gcd = " << gcd << " " << "(" << a << "; " << b << ")" << std::endl;
    }

    return 0;
}


void ThreadRunProcessGCD(std::list<std::tuple<int32_t, int32_t, int32_t>> &container,
                         const std::vector<int32_t> &array_one,
                         const std::vector<int32_t> &array_two,
                         std::mutex &mutex,
                         uint32_t begin_id,
                         uint32_t end_id) {

    bp::ipstream istream;
    bp::opstream ostream;

    bp::child proc_gcd("modules/array_gcd", bp::std_out > istream, bp::std_in < ostream);

    // sent size
    ostream << end_id - begin_id << std::endl;

    // first array
    for (size_t i = begin_id; i < end_id - 1; i++) {
        ostream << array_one[i] << " ";
    }
    ostream << array_one[end_id - 1] << std::endl;

    // first array
    for (size_t i = begin_id; i < end_id - 1; i++) {
        ostream << array_two[i] << " ";
    }
    ostream << array_two[end_id - 1] << std::endl;


    std::string line;
    int32_t gcd = 0;
    int32_t first_num = 0;
    int32_t second_num = 0;
    std::stringstream stream;
    stream.exceptions(std::ios::failbit);

    while (std::getline(istream, line)) {
        stream.str("");
        stream.clear();

        stream << line;

        try {
            stream >> gcd >> first_num >> second_num;


        } catch (std::exception &e) {
            std::cout << "Conversion error" << std::endl;
            proc_gcd.terminate();
            return;
        }

        {
            std::lock_guard<std::mutex> lg(mutex);
            container.emplace_back(gcd, first_num, second_num);
        }
    }
    ostream.pipe().close();

    proc_gcd.wait();
}


std::vector<int32_t> GenArray(size_t size) {
    std::random_device rd;
    std::mt19937 mersenne(rd());

    std::vector<int32_t> array;
    array.reserve(size);

    for (size_t i = 0; i < size; i++) {
        array.emplace_back(mersenne() % 1000 - 500);
    }

    return array;
}
