#include <iostream>
#include <numeric>
#include <sstream>
#include <ios>
#include <vector>


template <class T>
std::vector<T> StrToVector(const std::string& data, size_t capacity = 10) {
    std::vector<T> values;
    values.reserve(capacity);

    std::stringstream stream(data);
    stream.exceptions(std::ios::failbit);

    T value;
    try {
        while (!stream.eof()) {
            stream >> value;
            values.push_back(value);
        }
    } catch (std::exception &e) {
        std::cout << "Conversion error" << std::endl;
        exit(1);
    }

    return values;
}


int main() {
    std::string values;
    size_t capacity = 0;

    std::getline(std::cin, values);
    std::stringstream (values) >> capacity;

    std::string buffer;

    // first vector
    std::getline(std::cin, buffer);
    auto vct_1 = StrToVector<int32_t>(buffer, capacity);

    // second vector
    std::getline(std::cin, buffer);
    auto vct_2 = StrToVector<int32_t>(buffer, capacity);

    for (size_t i = 0; i < vct_1.size(); i++) {
        auto gcd = std::gcd(vct_1[i], vct_2[i]);
        std::cout << gcd << " " << vct_1[i] << " " << vct_2[i] << std::endl;
    }

    return 0;
}