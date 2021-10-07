#ifndef LAB_8_BARRIER_H
#define LAB_8_BARRIER_H

#include <mutex>
#include <condition_variable>


namespace l8 {

    class Barrier {
    private:
        const uint32_t num_of_threads;
        uint32_t num_of_threads_wait;

        std::condition_variable cond_wait;
        std::mutex mutex;

    public:
        Barrier(const Barrier &) = delete;
        explicit Barrier(uint32_t n) : num_of_threads(n), num_of_threads_wait(0) {}

        void wait();

    };

} // namespace l8

#endif //LAB_8_BARRIER_H
