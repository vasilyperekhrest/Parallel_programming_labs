#ifndef LAB_8_MATRIX_H
#define LAB_8_MATRIX_H

#include <iostream>
#include <vector>
#include <mutex>


namespace l8 {

    class Matrix {
    private:
        std::vector<std::vector<uint16_t>> matrix;
        uint32_t n;
        uint32_t m;

        uint32_t numb_of_ones;

        std::mutex mutex;

    public:
        ~Matrix() = default;
        explicit Matrix(uint32_t n = 5, uint32_t m = 5, uint32_t ones = 15);

        void gen_matrix();
        void dec_ones();

        uint32_t get_num_of_ones();

        std::pair<uint32_t, uint32_t> get_size();
        std::vector<uint16_t> &operator[](uint32_t i);
        friend std::ostream &operator<< (std::ostream &out, l8::Matrix &mt);

    };

} // namespace l8

#endif //LAB_8_MATRIX_H
