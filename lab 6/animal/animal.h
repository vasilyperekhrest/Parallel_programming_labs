//
// Created by Vasily Perekhrest on 24.09.2021.
//

#ifndef LAB_5_ANIMAL_H
#define LAB_5_ANIMAL_H

#include <iostream>
#include <string>
#include <string_view>


namespace l5 {

    enum class AnimalType {
        RABBIT = 0,
        CAT,
        DOG,
    };


    class Animal {
    private:
        std::string name;
        l5::AnimalType animal_type;
        bool is_disturbed;

    public:
        Animal();
        explicit Animal(std::string_view _name, l5::AnimalType _type);
        ~Animal() = default;

        std::string_view get_name();
        l5::AnimalType get_type();

        void set_disturb(bool _cond);
        [[nodiscard]] bool is_disturb() const;

    };

} // namespace l5

#endif //LAB_5_ANIMAL_H
