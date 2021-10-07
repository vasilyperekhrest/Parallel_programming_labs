#include "barrier.h"


void l8::Barrier::wait() {
    std::unique_lock<std::mutex> u_lock(this->mutex);
    this->num_of_threads_wait++;

    if (this->num_of_threads_wait >= this->num_of_threads) {
        num_of_threads_wait = 0;
        this->cond_wait.notify_all();

    } else {
        this->cond_wait.wait(u_lock);
    }
}
