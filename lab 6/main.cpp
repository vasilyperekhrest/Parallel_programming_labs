#include <iostream>
#include <memory>
#include <vector>
#include <list>
#include <thread>
#include <memory>
#include <chrono>

#include "animal/animal.h"


using animal_sptr_t = std::shared_ptr<l5::Animal>;

void thread_function(std::list<animal_sptr_t> &bowls,
                     std::vector<bool> &bowls_full,
                     std::mutex &mutex,
                     const std::string& _name,
                     uint32_t _id,
                     l5::AnimalType _type);

[[noreturn]] void thread_pavel(std::vector<bool> &bowl_full, std::mutex &mutex);



int main() {
    uint32_t num_cats = 4;
    uint32_t num_dogs = 5;
    uint32_t num_rabbits = 4;
    uint32_t num_bowls = 3;

    std::list<animal_sptr_t> bowls(num_bowls, nullptr);
    std::mutex mutex;

    std::vector<bool> bowls_full(num_bowls, true);

    std::vector<std::thread> threads_cats;
    std::vector<std::thread> threads_dogs;
    std::vector<std::thread> threads_rabbits;

    auto thread_pv = std::thread(thread_pavel, std::ref(bowls_full), std::ref(mutex));
    thread_pv.detach();

    for (uint32_t i = 0; i < num_cats; i++) {
        threads_cats.emplace_back(thread_function,
                                  std::ref(bowls),
                                  std::ref(bowls_full),
                                  std::ref(mutex),
                                  "Cat ",
                                  i,
                                  l5::AnimalType::CAT);
    }


    for (uint32_t i = 0; i < num_dogs; i++) {
        threads_dogs.emplace_back(thread_function,
                                  std::ref(bowls),
                                  std::ref(bowls_full),
                                  std::ref(mutex),
                                  "Dog ",
                                  i,
                                  l5::AnimalType::DOG);
    }


    for (uint32_t i = 0; i < num_rabbits; i++) {
        threads_rabbits.emplace_back(thread_function,
                                     std::ref(bowls),
                                     std::ref(bowls_full),
                                     std::ref(mutex),
                                     "Rabbit ",
                                     i,
                                     l5::AnimalType::RABBIT);
    }


    for (auto &thread : threads_cats) {
        thread.join();
    }


    for (auto &thread : threads_dogs) {
        thread.join();
    }


    for (auto &thread : threads_rabbits) {
        thread.join();
    }

    return 0;
}


void thread_function(std::list<animal_sptr_t> &bowls,
                     std::vector<bool> &bowls_full,
                     std::mutex &mutex,
                     const std::string& _name,
                     uint32_t id,
                     l5::AnimalType _type) {
    auto animal = std::make_shared<l5::Animal>(_name + std::to_string(id + 1), _type);
    bool is_full = false;
    uint32_t num_of_bowl = 0;

    while (!is_full) {

        for (auto &bowl : bowls) {
            num_of_bowl %= bowls.size();
            num_of_bowl++;
            std::unique_lock<std::mutex> ulock(mutex);

            if (bowl == nullptr) {
                // if bowl is empty
                bowl = animal;

            } else if (animal->get_type() > bowl->get_type()) {
                bowl->set_disturb(true);

                std::cout << animal->get_name() << " kick away " << bowl->get_name() << std::endl;

                bowl = animal;

            } else {
                // if bowl is not empty, then go next
                continue;
            }

            std::cout << animal->get_name() << " starts eating in a bowl " << num_of_bowl << std::endl;

            ulock.unlock();


            if (!bowls_full[num_of_bowl - 1]) {
                ulock.lock();
                std::cout << "bowl " << num_of_bowl << " is empty :( " << std::this_thread::get_id() << " wait" << std::endl;
                ulock.unlock();

                while (!bowls_full[num_of_bowl - 1]) {
                    std::this_thread::sleep_for(std::chrono::seconds(1));
                }
            }



            for (uint32_t i = 0; i < 100; i++) {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                if (animal->is_disturb()) {
                    break;
                }
            }

            ulock.lock();

            if (!animal->is_disturb()) {
                // animal is full
                is_full = true;
                // bowl is empty
                bowl = nullptr;
                std::cout << animal->get_name() << " ate successfully, bowl " << num_of_bowl << " is free" << std::endl;
                bowls_full[num_of_bowl - 1] = false;
                break;

            } else {
                // animal looking for a bowl again
                animal->set_disturb(false);
            }

        }

    }
}


[[noreturn]] void thread_pavel(std::vector<bool> &bowl_full, std::mutex &mutex) {
    while (true) {
        for (uint32_t i = 0; i < bowl_full.size(); i++) {
            if (!bowl_full[i]) {
                std::this_thread::sleep_for(std::chrono::seconds(5));

                {
                    std::lock_guard<std::mutex> lg(mutex);
                    std::cout << "Pavel added food in bowl " << i + 1 << std::endl;
                }

                bowl_full[i] = true;
            }
        }
    }
}




