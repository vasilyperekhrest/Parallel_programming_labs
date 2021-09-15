#include <iostream>
#include <numeric>
#include <vector>
#include <tuple>
#include <random>
#include <thread>


static void thread_func(
        std::vector<std::tuple<uint32_t, uint32_t, uint32_t>> &res,
        const std::vector<uint32_t> &vct1,
        const std::vector<uint32_t> &vct2,
        uint32_t begin,
        uint32_t end);


int main() {
    std::random_device rd;
    std::mt19937 mersenne(rd());

    uint32_t size = 23;
    uint32_t num_of_threads = 11;

    std::vector<std::thread> threads_id;

    std::vector<uint32_t> vct1(size, 0);
    std::vector<uint32_t> vct2(size, 0);
    std::vector<std::tuple<uint32_t, uint32_t, uint32_t>> res(size, std::make_tuple(0, 0, 0));

    // init vct1 and vct2
    for (uint32_t i = 0; i < size; i++) {
        vct1[i] = mersenne() % 11 + 1;
        vct2[i] = mersenne() % 11 + 1;
    }


    uint32_t step = size / num_of_threads;
    uint32_t add = size % num_of_threads;

    threads_id.emplace_back(thread_func, std::ref(res), std::cref(vct1), std::cref(vct2), 0, step + add);
    for (uint32_t i = 1; i < num_of_threads; i++) {
        uint32_t begin = i * step + add;
        uint32_t end = step * (i + 1) + add;

        threads_id.emplace_back(
                thread_func,
                std::ref(res),
                std::cref(vct1),
                std::cref(vct2),
                begin,
                end);
    }

    for (uint32_t i = 0; i < num_of_threads; i++) {
        threads_id[i].join();
    }


    std::cout << "Outputting the result of " << num_of_threads << " threads" << std::endl;

    for (uint32_t i = 0; i < size; i++) {
        auto [first, second, d] = res[i];

        std::cout << i + 1 << ") " << vct1[i] << " / " << vct2[i] << "\t -> "
                  << " gcd = " << d << " \t" << first << " / " << second << std::endl;

    }

    return 0;
}


void thread_func(std::vector<std::tuple<uint32_t, uint32_t, uint32_t>> &res,
                 const std::vector<uint32_t> &vct1,
                 const std::vector<uint32_t> &vct2,
                 uint32_t begin,
                 uint32_t end) {
    for (uint32_t i = begin; i < end; i++) {
        uint32_t d = std::gcd(vct1[i], vct2[i]);
        res[i] = std::tuple(vct1[i] / d, vct2[i] / d, d);
    }
}
