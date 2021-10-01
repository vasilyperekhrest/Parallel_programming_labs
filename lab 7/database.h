#ifndef LAB_7_DATABASE_H
#define LAB_7_DATABASE_H

#include <iostream>
#include <vector>
#include <tuple>
#include <algorithm>
#include <mutex>


namespace l7 {

    class Database {
    private:
        std::vector<int32_t> transactions;
        int32_t balance;

        std::mutex mutex;

    public:
        Database();
        ~Database() = default;

        friend std::ostream& operator<< (std::ostream &out, Database &db);

        void add_transaction(int32_t _transact);
        void delete_range_transactions(uint32_t begin, uint32_t end);
        uint32_t get_transactions_size();

        std::tuple<int32_t, int32_t> get_minmax_transactions();

    };



}

#endif //LAB_7_DATABASE_H
