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
}

database_blocks::db::db() = default;

void database_blocks::db::swap_mem_tree() {
    std::scoped_lock l(tree_lock);
    this->tree;
}