//
// Created by 钟卓凡 on 23/06/2023.
//

#include "db.h"
#include "mem_tree_impl.h"
//#include "mem_tree_impl.cpp"

#include <utility>

#include "config.h"
#include <vector>
#include "spdlog/spdlog.h"

database_blocks::db::db(database_blocks::configs config) : config(std::move(config)) {
    tree = std::make_shared<mem_tree>(config);
}

database_blocks::db::db() {
    tree = std::make_shared<mem_tree>(config);
};

void database_blocks::db::swap_mem_tree() {
    std::scoped_lock l(tree_lock);
    this->immutable_trees.push_back(this->tree);
    // place a new mem tree
    this->tree = std::make_shared<mem_tree>();
}