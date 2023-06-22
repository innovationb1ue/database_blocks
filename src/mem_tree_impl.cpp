#include "mem_tree_impl.h"

#include <iostream>
#include <fstream>

#define separator ";"

database_blocks::mem_tree::mem_tree() {
    this->_store = std::make_shared<std::map<std::string, std::string>>();
    this->max_size = 4096;
    this->immutable = false;
}

void database_blocks::mem_tree::flush() {
    if (!immutable) {
        set_immutable();
    }
    std::ofstream myfile;
    // todo: make the file path a parameter.
    myfile.open("db.txt");
    auto it = _store->begin();
    while (it != _store->end()) {
        myfile << it->first << separator << it->second << "\n";
        it++;
    }
    myfile.close();
    return;
}

bool database_blocks::mem_tree::put(std::string &key, std::string &val) {
    if (immutable) {
        return false;
    }
    auto res = _store->insert_or_assign(std::move(key), std::move(val));
    // set immutable if maximum size is reached
    if (_store->size() > max_size) {
        this->set_immutable();
    }
    return res.second;
}

void database_blocks::mem_tree::set_immutable() {
    immutable = true;
}

bool database_blocks::mem_tree::is_immutable() const {
    return immutable;
}

bool database_blocks::mem_tree::merge(const database_blocks::mem_tree &&other) {
    this->_store->merge(std::move(*other.get_store().get()));
    return true;
}

database_blocks::store_type database_blocks::mem_tree::get_store() const {
    return _store;
}

bool database_blocks::mem_tree::remove(std::string &key) {
    return this->_store->erase(key);
}

std::optional<std::string> database_blocks::mem_tree::get(std::string &key) {
    auto res = this->_store->find(key);
    return res == this->_store->end() ?
           std::nullopt :
           std::optional<std::reference_wrapper<std::string>>{res->second};
}

void database_blocks::mem_tree::load() {
    std::ifstream myfile;
    // todo: make the file path a parameter.
    myfile.open("db.txt");
    std::string line;
    while (std::getline(myfile, line)) {
        auto split_pos = line.find(';');
        auto key = std::string_view(line.data(), split_pos);
        auto val = std::string_view(line.data() + split_pos + 1, line.size() - split_pos - 1);
        this->_store->emplace(key, val);
    }
}
