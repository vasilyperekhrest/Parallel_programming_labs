//
// Created by Vasily Perekhrest on 24.09.2021.
//

#include "animal.h"


l5::Animal::Animal() {
    this->name = "None";
    this->animal_type = l5::AnimalType::RABBIT;
    this->is_disturbed = false;
}

[[maybe_unused]] l5::Animal::Animal(std::string_view _name, l5::AnimalType _type) {
    this->name = _name;
    this->animal_type = _type;
    this->is_disturbed = false;
}

void l5::Animal::set_disturb(bool _cond) {
    this->is_disturbed = _cond;
}

bool l5::Animal::is_disturb() const {
    return this->is_disturbed;
}

std::string_view l5::Animal::get_name() {
    return std::string_view(name);
}

l5::AnimalType l5::Animal::get_type() {
    return this->animal_type;
}


