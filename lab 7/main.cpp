#include <iostream>
#include <chrono>
#include <thread>
#include <random>

#include "database.h"


[[noreturn]] void ThreadAddTransactions(l7::Database &db);
[[noreturn]] void ThreadTransactionOutput(l7::Database &db, std::mutex &mutex);
[[noreturn]] void ThreadMinMaxTransactions(l7::Database &db, std::mutex &mutex);
[[noreturn]] void ThreadDeleteTransactions(l7::Database &db, uint32_t limit_transact, uint32_t num_delete);


int main() {
    l7::Database db;
    std::mutex mutex;

    std::thread(ThreadTransactionOutput, std::ref(db), std::ref(mutex)).detach();
    std::thread(ThreadAddTransactions, std::ref(db)).detach();
    std::thread(ThreadMinMaxTransactions, std::ref(db), std::ref(mutex)).detach();
    std::thread(ThreadDeleteTransactions, std::ref(db), 12, 9).detach();

    std::this_thread::sleep_for(std::chrono::hours(1));
    return 0;
}


void ThreadAddTransactions(l7::Database &db) {
    std::random_device rd;
    std::mt19937_64 mersenne(rd());

    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(2));
        auto transact = mersenne() % 10000 - 5000;
        db.add_transaction((int32_t) transact);
    }
}


void ThreadTransactionOutput(l7::Database &db, std::mutex &mutex) {
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(2));
        {
            std::lock_guard<std::mutex> lg(mutex);
            std::cout << db << std::endl;
        }
    }
}


void ThreadMinMaxTransactions(l7::Database &db, std::mutex &mutex) {
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(2));
        {
            std::lock_guard<std::mutex> lg(mutex);
            auto [min, max] = db.get_minmax_transactions();
            std::cout << "Minimum transaction: " << min << std::endl <<
                         "Maximum transaction: " << max << std::endl << std::endl;
        }
    }
}


void ThreadDeleteTransactions(l7::Database &db, uint32_t limit_transact, uint32_t num_delete) {
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(2));

        if (db.get_transactions_size() > limit_transact) {
            db.delete_range_transactions(0, num_delete);
        }
    }
}
