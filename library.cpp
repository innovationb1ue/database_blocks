#include "library.h"

#include <iostream>
#include <fstream>

#define separator ";"

database_blocks::mem_tree::mem_tree() {
    this->_store = std::make_shared<std::map<std::string, std::string>>();
    this->max_size = 4096;
}

void database_blocks::mem_tree::flush() {
    std::ofstream myfile;
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
    if (is_immutable) {
        return false;
    }
    auto res = _store->emplace(key, val);
    if (_store->size() > max_size) {
        this->set_immutable();
    }
    return res.second;
}

void database_blocks::mem_tree::set_immutable() {
    is_immutable = true;
}
