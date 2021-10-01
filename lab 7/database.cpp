#include <memory>
#include <numeric>

#include "database.h"


l7::Database::Database() {
    this->balance = 0;
}


void l7::Database::add_transaction(int32_t _transact) {
    std::lock_guard<std::mutex> lg(this->mutex);

    if (this->balance + _transact < 0) {
        return;
    }

    this->transactions.push_back(_transact);
    this->balance += _transact;

}

std::ostream &l7::operator<<(std::ostream &out, l7::Database &db) {
    std::lock_guard<std::mutex> lg(db.mutex);
    uint32_t max_indent = std::to_string(db.transactions.size()).size() + 2;

    out << "id" << std::string(max_indent - 2, ' ') << "transaction\n";

    for (uint32_t i = 0; i < db.transactions.size(); i++) {
        out << i << std::string(max_indent - std::to_string(i).size(), ' ') << db.transactions[i] << "\n";
    }

    out << std::string(max_indent, ' ') << db.balance << "\n";

    return out;
}

std::tuple<int32_t , int32_t> l7::Database::get_minmax_transactions() {
    std::lock_guard<std::mutex> lg(this->mutex);

    if (this->transactions.empty()) {
        return std::make_tuple(0, 0);
    }

    auto [min, max] = std::minmax_element(this->transactions.cbegin(), this->transactions.cend());
    return std::make_tuple(*min, *max);
}

uint32_t l7::Database::get_transactions_size() {
    std::lock_guard<std::mutex> lg(this->mutex);
    return this->transactions.size();
}

void l7::Database::delete_range_transactions(uint32_t begin, uint32_t end) {
    std::lock_guard<std::mutex> lg(this->mutex);

    if (begin > end or end >= this->transactions.size()) {
        return;
    }

    uint32_t delta = end - begin;
    std::vector<int32_t> new_transactions(this->transactions.size() - delta);

    for (uint32_t i = 0; i < begin; i++) {
        new_transactions[i] = this->transactions[i];
    }

    for (uint32_t i = end; i < this->transactions.size(); i++) {
        new_transactions[i - delta] = this->transactions[i];
    }

    this->transactions = new_transactions;
}

