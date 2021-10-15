#include <random>

#include "matrix.h"


l8::Matrix::Matrix(uint32_t n, uint32_t m, uint32_t ones) {
    assert(n * m >= ones);

    this->n = n;
    this->m = m;

    this->numb_of_ones = ones;

    this->gen_matrix();
}

void l8::Matrix::gen_matrix() {
    std::lock_guard<std::mutex> l_guard(this->mutex);

    if (!this->matrix.empty()) {
        this->matrix.clear();
    }

    for (uint32_t i = 0; i < this->n; i++) {
        this->matrix.emplace_back(this->m, 0);
    }

    std::random_device rd;
    std::mt19937 mersenne(rd());

    for (uint32_t k = 0; k < this->numb_of_ones; k++) {

        while (true) {
            uint32_t i = mersenne() % this->n;
            uint32_t j = mersenne() % this->m;

            if (this->matrix[i][j] == 0) {
                this->matrix[i][j] = 1;
                break;
            }
        }
    }
}

std::ostream &l8::operator<<(std::ostream &out, l8::Matrix &mt) {
    std::lock_guard<std::mutex> l_guard(mt.mutex);

    for (const auto &array : mt.matrix) {
        for (const auto &elem : array) {
            out << elem << " ";
        }
        out << '\n';
    }

    return out;
}

std::vector<uint16_t> &l8::Matrix::operator[](uint32_t i) {
    std::lock_guard<std::mutex> l_guard(this->mutex);
    return this->matrix[i];
}

std::pair<uint32_t, uint32_t> l8::Matrix::get_size() {
    std::lock_guard<std::mutex> l_guard(this->mutex);
    return std::make_pair(this->n, this->m);
}

void l8::Matrix::dec_ones() {
    std::lock_guard<std::mutex> l_guard(this->mutex);
    this->numb_of_ones--;
}

uint32_t l8::Matrix::get_num_of_ones() {
    std::lock_guard<std::mutex> l_guard(this->mutex);
    return this->numb_of_ones;
}
